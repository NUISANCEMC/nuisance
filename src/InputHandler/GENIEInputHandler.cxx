#include "GENIEInputHandler.h"
#ifdef __GENIE_ENABLED__

GENIEGeneratorInfo::~GENIEGeneratorInfo() {
  DeallocateParticleStack();
}

void GENIEGeneratorInfo::AddBranchesToTree(TTree* tn) {
  tn->Branch("GenieParticlePDGs", &fGenieParticlePDGs, "GenieParticlePDGs/I");
}

void GENIEGeneratorInfo::SetBranchesFromTree(TTree* tn) {
  tn->SetBranchAddress("GenieParticlePDGs", &fGenieParticlePDGs);
}

void GENIEGeneratorInfo::AllocateParticleStack(int stacksize) {
  fGenieParticlePDGs = new int[stacksize];
}

void GENIEGeneratorInfo::DeallocateParticleStack() {
  delete fGenieParticlePDGs;
}

void GENIEGeneratorInfo::FillGeneratorInfo(NtpMCEventRecord* ntpl) {
  Reset();

  // Check for GENIE Event
  if (!ntpl) return;
  if (!ntpl->event) return;

  // Cast Event Record
  GHepRecord* ghep = static_cast<GHepRecord*>(ntpl->event);
  if (!ghep) return;

  // Fill Particle Stack
  GHepParticle* p = 0;
  TObjArrayIter iter(ghep);

  // Loop over all particles
  int i = 0;
  while ((p = (dynamic_cast<genie::GHepParticle*>((iter).Next())))) {
    if (!p) continue;

    // Get PDG
    fGenieParticlePDGs[i] = p->Pdg();
    i++;
  }
}

void GENIEGeneratorInfo::Reset() {
  for (int i = 0; i < kMaxParticles; i++) {
    fGenieParticlePDGs[i] = 0;
  }
}


GENIEInputHandler::GENIEInputHandler(std::string const& handle, std::string const& rawinputs) {
  LOG(SAM) << "Creating GENIEInputHandler : " << handle << std::endl;

  // Run a joint input handling
  fName = handle;

  // Setup the TChain
  fGENIETree = new TChain("gtree");
  fSaveExtra = FitPar::Config().GetParB("SaveExtraGenie");
  fCacheSize = FitPar::Config().GetParI("CacheSize");
  fMaxEvents = FitPar::Config().GetParI("MAXEVENTS");

  // Loop over all inputs and grab flux, eventhist, and nevents
  std::vector<std::string> inputs = InputUtils::ParseInputFileList(rawinputs);
  for (size_t inp_it = 0; inp_it < inputs.size(); ++inp_it) {

    // Open File for histogram access
    TFile* inp_file = new TFile(InputUtils::ExpandInputDirectories(inputs[inp_it]).c_str(), "READ");
    if (!inp_file or inp_file->IsZombie()) {
      THROW( "GENIE File IsZombie() at : '" << inputs[inp_it] << "'" << std::endl
             << "Check that your file paths are correct and the file exists!" << std::endl
             << "$ ls -lh " << inputs[inp_it] );
    }

    // Get Flux/Event hist
    TH1D* fluxhist  = (TH1D*)inp_file->Get("nuisance_flux");
    TH1D* eventhist = (TH1D*)inp_file->Get("nuisance_events");
    if (!fluxhist or !eventhist) {
      ERROR(FTL, "Input File Contents: " << inputs[inp_it] );
      inp_file->ls();
      THROW( "GENIE FILE doesn't contain flux/xsec info." << std::endl
              << "Try running the app PrepareGENIE first on :" << inputs[inp_it] << std::endl
              << "$ PrepareGENIE -h" );
    }

    // Get N Events
    TTree* genietree = (TTree*)inp_file->Get("gtree");
    if (!genietree) {
      ERROR(FTL, "gtree not located in GENIE file: " << inputs[inp_it]);
      THROW("Check your inputs, they may need to be completely regenerated!");
      throw;
    }
    int nevents = genietree->GetEntries();
    if (nevents <= 0){
      THROW("Trying to a TTree with " << nevents << " to TChain from : " << inputs[inp_it]);
    }


    // Register input to form flux/event rate hists
    RegisterJointInput(inputs[inp_it], nevents, fluxhist, eventhist);

    // Add To TChain
    fGENIETree->AddFile( inputs[inp_it].c_str() );
  }

  // Registor all our file inputs
  SetupJointInputs();

  // Assign to tree
  fEventType = kGENIE;
  fGenieNtpl = NULL;
  fGENIETree->SetBranchAddress("gmcrec", &fGenieNtpl);
  fGENIETree->GetEntry(0);
  
  // Create Fit Event
  fNUISANCEEvent = new FitEvent();
  fNUISANCEEvent->SetGenieEvent(fGenieNtpl);

  if (fSaveExtra) {
    fGenieInfo     = new GENIEGeneratorInfo();
    fNUISANCEEvent->AddGeneratorInfo(fGenieInfo);
  }

  fNUISANCEEvent->HardReset();

};

GENIEInputHandler::~GENIEInputHandler() {
  if (fGenieGHep) delete fGenieGHep;
  if (fGenieNtpl) delete fGenieNtpl;
  if (fGENIETree) delete fGENIETree;
  if (fGenieInfo) delete fGenieInfo;
}

void GENIEInputHandler::CreateCache() {
  if (fCacheSize > 0) {
    // fGENIETree->SetCacheEntryRange(0, fNEvents);
    fGENIETree->AddBranchToCache("*", 1);
    fGENIETree->SetCacheSize(fCacheSize);
  }
}

void GENIEInputHandler::RemoveCache() {
  // fGENIETree->SetCacheEntryRange(0, fNEvents);
  fGENIETree->AddBranchToCache("*", 0);
  fGENIETree->SetCacheSize(0);
}

FitEvent* GENIEInputHandler::GetNuisanceEvent(const UInt_t entry, const bool lightweight) {
  if (entry >= (UInt_t)fNEvents) return NULL;

  // Read Entry from TTree to fill NEUT Vect in BaseFitEvt;
  fGENIETree->GetEntry(entry);

  // Setup Input scaling for joint inputs
  fNUISANCEEvent->InputWeight = GetInputWeight(entry);

  // Run NUISANCE Vector Filler
  if (!lightweight) {
    CalcNUISANCEKinematics();
  }

  return fNUISANCEEvent;
}


int GENIEInputHandler::GetGENIEParticleStatus(genie::GHepParticle* p, int mode) {
  /*
    kIStUndefined                  = -1,
    kIStInitialState               =  0,   / generator-level initial state /
    kIStStableFinalState           =  1,   / generator-level final state:
    particles to be tracked by detector-level MC /
    kIStIntermediateState          =  2,
    kIStDecayedState               =  3,
    kIStCorrelatedNucleon          = 10,
    kIStNucleonTarget              = 11,
    kIStDISPreFragmHadronicState   = 12,
    kIStPreDecayResonantState      = 13,
    kIStHadronInTheNucleus         = 14,   / hadrons inside the nucleus: marked
    for hadron transport modules to act on /
    kIStFinalStateNuclearRemnant   = 15,   / low energy nuclear fragments
    entering the record collectively as a 'hadronic blob' pseudo-particle /
    kIStNucleonClusterTarget       = 16,   // for composite nucleons before
    phase space decay
  */

  int state = kUndefinedState;
  switch (p->Status()) {
  case genie::kIStNucleonTarget:
  case genie::kIStInitialState:
  case genie::kIStCorrelatedNucleon:
  case genie::kIStNucleonClusterTarget:
    state = kInitialState;
    break;

  case genie::kIStStableFinalState:
    state = kFinalState;
    break;

  case genie::kIStHadronInTheNucleus:
    if (abs(mode) == 2)
      state = kInitialState;
    else
      state = kFSIState;
    break;

  case genie::kIStPreDecayResonantState:
  case genie::kIStDISPreFragmHadronicState:
  case genie::kIStIntermediateState:
    state = kFSIState;
    break;

  case genie::kIStFinalStateNuclearRemnant:
  case genie::kIStUndefined:
  case genie::kIStDecayedState:
  default:
    break;
  }

  // Flag to remove nuclear part in genie
  if (p->Pdg() > 1000000) {
    if (state == kInitialState)
      state = kNuclearInitial;
    else if (state == kFinalState)
      state = kNuclearRemnant;
  }

  return state;
}
#endif

#ifdef __GENIE_ENABLED__
int GENIEInputHandler::ConvertGENIEReactionCode(GHepRecord* gheprec) {

  // Electron Scattering
  if (gheprec->Summary()->ProcInfo().IsEM()) {
    if (gheprec->Summary()->InitState().ProbePdg() == 11) {
      if (gheprec->Summary()->ProcInfo().IsQuasiElastic()) return 1;
      else if (gheprec->Summary()->ProcInfo().IsMEC()) return 2;
      else if (gheprec->Summary()->ProcInfo().IsResonant()) return 13;
      else if (gheprec->Summary()->ProcInfo().IsDeepInelastic()) return 26;
      else {
        ERROR(WRN, "Unknown GENIE Electron Scattering Mode!" << std::endl
                 << "ScatteringTypeId = " << gheprec->Summary()->ProcInfo().ScatteringTypeId() << " "
                 << "InteractionTypeId = " << gheprec->Summary()->ProcInfo().InteractionTypeId() << std::endl
                 << genie::ScatteringType::AsString(gheprec->Summary()->ProcInfo().ScatteringTypeId()) << " "
                 << genie::InteractionType::AsString(gheprec->Summary()->ProcInfo().InteractionTypeId()) << " "
                 << gheprec->Summary()->ProcInfo().IsMEC());
        return 0;
      }
    }

    // Weak CC
  } else if (gheprec->Summary()->ProcInfo().IsWeakCC()) {

    // CC MEC
    if (gheprec->Summary()->ProcInfo().IsMEC()) {
      if (pdg::IsNeutrino(gheprec->Summary()->InitState().ProbePdg()))  return 2;
      else if (pdg::IsAntiNeutrino(gheprec->Summary()->InitState().ProbePdg())) return -2;

      // CC OTHER
    } else {
      return utils::ghep::NeutReactionCode(gheprec);
    }

    // Weak NC
  } else if (gheprec->Summary()->ProcInfo().IsWeakNC()) {
    // NC MEC
    if (gheprec->Summary()->ProcInfo().IsMEC()) {
      if (pdg::IsNeutrino(gheprec->Summary()->InitState().ProbePdg()))  return 32;
      else if (pdg::IsAntiNeutrino(gheprec->Summary()->InitState().ProbePdg())) return -32;

      // NC OTHER
    } else {
      return utils::ghep::NeutReactionCode(gheprec);
    }
  }

  return 0;
}

void GENIEInputHandler::CalcNUISANCEKinematics() {

  // Reset all variables
  fNUISANCEEvent->ResetEvent();

  // Check for GENIE Event
  if (!fGenieNtpl) return;
  if (!fGenieNtpl->event) return;

  // Cast Event Record
  fGenieGHep = static_cast<GHepRecord*>(fGenieNtpl->event);
  if (!fGenieGHep) return;

  // Convert GENIE Reaction Code
  fNUISANCEEvent->fMode = ConvertGENIEReactionCode(fGenieGHep);

  // Set Event Info
  fNUISANCEEvent->Mode = fNUISANCEEvent->fMode;
  fNUISANCEEvent->fEventNo = 0.0;
  fNUISANCEEvent->fTotCrs = fGenieGHep->XSec();
  fNUISANCEEvent->fTargetA = 0.0;
  fNUISANCEEvent->fTargetZ = 0.0;
  fNUISANCEEvent->fTargetH = 0;
  fNUISANCEEvent->fBound   = 0.0;
  fNUISANCEEvent->InputWeight = 1.0; //(1E+38 / genie::units::cm2) * fGenieGHep->XSec();

  // Get N Particle Stack
  unsigned int npart = fGenieGHep->GetEntries();
  unsigned int kmax = fNUISANCEEvent->kMaxParticles;
  if (npart > kmax) {
    ERR(WRN) << "GENIE has too many particles, expanding stack." << std::endl;
    fNUISANCEEvent->ExpandParticleStack(npart);
  }

  // Fill Particle Stack
  GHepParticle* p = 0;
  TObjArrayIter iter(fGenieGHep);
  fNUISANCEEvent->fNParticles = 0;

  // Loop over all particles
  while ((p = (dynamic_cast<genie::GHepParticle*>((iter).Next())))) {
    if (!p) continue;

    // Get Status
    int state = GetGENIEParticleStatus(p, fNUISANCEEvent->fMode);

    // Remove Undefined
    if (kRemoveUndefParticles &&
        state == kUndefinedState) continue;

    // Remove FSI
    if (kRemoveFSIParticles &&
        state == kFSIState) continue;

    if (kRemoveNuclearParticles &&
        (state == kNuclearInitial || state == kNuclearRemnant)) continue;

    // Fill Vectors
    int curpart = fNUISANCEEvent->fNParticles;
    fNUISANCEEvent->fParticleState[curpart] = state;

    // Mom
    fNUISANCEEvent->fParticleMom[curpart][0] = p->Px() * 1.E3;
    fNUISANCEEvent->fParticleMom[curpart][1] = p->Py() * 1.E3;
    fNUISANCEEvent->fParticleMom[curpart][2] = p->Pz() * 1.E3;
    fNUISANCEEvent->fParticleMom[curpart][3] = p->E() * 1.E3;

    // PDG
    fNUISANCEEvent->fParticlePDG[curpart] = p->Pdg();

    // Add to N particle count
    fNUISANCEEvent->fNParticles++;

    // Extra Check incase GENIE fails.
    if ((UInt_t)fNUISANCEEvent->fNParticles == kmax) {
      ERR(WRN) << "Number of GENIE Particles exceeds maximum!" << std::endl;
      ERR(WRN) << "Extend kMax, or run without including FSI particles!" << std::endl;
      break;
    }
  }

  // Fill Extra Stack
  if (fSaveExtra) fGenieInfo->FillGeneratorInfo(fGenieNtpl);

  // Run Initial, FSI, Final, Other ordering.
  fNUISANCEEvent-> OrderStack();

  return;
}

void GENIEInputHandler::Print() {
}

#endif


