#include "GENIEInputHandler.h"

GENIEInputHandler::GENIEInputHandler(std::string const& handle, std::string const& rawinputs) {
#ifdef __GENIE_ENABLED__

  // Run a joint input handling
  fName = handle;
  jointinput = false;
  jointindexswitch = 0;

  // Form list of all inputs, remove brackets if required.
  std::vector<std::string> inputs = GeneralUtils::ParseToStr(rawinputs, ",");
  if (inputs.front()[0] == '(') {
    inputs.front() = inputs.front().substr(1);
  }
  if (inputs.back()[inputs.back().size() - 1] == ')') {
    inputs.back() = inputs.back().substr(0, inputs.back().size() - 1);
  }
  for (size_t inp_it = 0; inp_it < inputs.size(); ++inp_it) {
    LOG(SAM) << "\t -> Found input file: " << inputs[inp_it] << std::endl;
  }

  fMaxEvents = FitPar::Config().GetParI("MAXEVENTS");

  // Setup the TChain
  fGENIETree = new TChain("gtree");

  // Loop over all inputs and grab flux, eventhist, and nevents
  // Also add it to the TChain
  int evtcounter = 0;
  if (inputs.size() > 1) jointinput = true;
  for (size_t inp_it = 0; inp_it < inputs.size(); ++inp_it) {

    // Add To TChain
    fGENIETree->AddFile( inputs[inp_it].c_str() );

    // Open File for histogram access
    TFile* inp_file = new TFile(inputs[inp_it].c_str(), "READ");

    // Get Flux/Event hist
    TH1D* fluxhist  = (TH1D*)inp_file->Get(
                        (PlotUtils::GetObjectWithName(inp_file, "nuisance_flux")).c_str());
    TH1D* eventhist = (TH1D*)inp_file->Get(
                        (PlotUtils::GetObjectWithName(inp_file, "nuisance_events")).c_str());

    // Error Check
    if (!fluxhist) {
      ERR(FTL) << "GENIE FILE doesn't contain flux/xsec info" << std::endl;
      ERR(FTL) << "Run app/PrepareGENIE first on :" << inputs[inp_it] << std::endl;
      throw;
    }

    // Get N Events
    TTree* genietree = (TTree*)inp_file->Get("gtree");
    int nevents = genietree->GetEntries();

    // Push into individual input vectors
    jointfluxinputs.push_back( (TH1D*) fluxhist->Clone() );
    jointeventinputs.push_back( (TH1D*) eventhist->Clone() );

    jointindexlow.push_back(evtcounter);
    jointindexhigh.push_back(evtcounter + nevents);
    evtcounter += nevents;

    // Add to the total flux/event hist
    if (!fFluxHist) fFluxHist = (TH1D*) fluxhist->Clone();
    else fFluxHist->Add(fluxhist);

    if (!fEventHist) fEventHist = (TH1D*) eventhist->Clone();
    else fEventHist->Add(eventhist);

    // Remove file
    //inp_file->Close();
    //delete inp_file;
  }

  // Setup NEvents and the FitEvent
  fNEvents = fGENIETree->GetEntries();
  fEventType = kGENIE;
  fGenieGHep = NULL;
  fGenieNtpl = NULL;
  fGENIETree->SetBranchAddress("gmcrec", &fGenieNtpl);

  fNUISANCEEvent = new FitEvent(fGenieNtpl);
  fNUISANCEEvent->HardReset();
  fBaseEvent = static_cast<BaseFitEvt*>(fNUISANCEEvent);

  // Normalise event histograms for relative flux contributions.
  for (size_t i = 0; i < jointeventinputs.size(); i++) {
    TH1D* eventhist = (TH1D*) jointeventinputs.at(i)->Clone();

    double scale = double(fNEvents) / fEventHist->Integral("width");
    scale *= eventhist->Integral("width");
    scale /= double(jointindexhigh[i] - jointindexlow[i]);

    jointindexscale .push_back(scale);
  }

  fEventHist->SetNameTitle((fName + "_EVT").c_str(), (fName + "_EVT").c_str());
  fFluxHist->SetNameTitle((fName + "_FLUX").c_str(), (fName + "_FLUX").c_str());

  // Setup Max Events
  if (fMaxEvents > 1 && fMaxEvents < fNEvents) {
    if (LOG_LEVEL(SAM)) {
      std::cout << "\t\t|-> Read Max Entries : " << fMaxEvents << std::endl;
    }
    fNEvents = fMaxEvents;
  }
#endif
};


FitEvent* GENIEInputHandler::GetNuisanceEvent(const UInt_t entry) {
#ifdef __GENIE_ENABLED__

  if (entry >= (UInt_t)fNEvents) return NULL;

  // Read Entry from TTree to fill NEUT Vect in BaseFitEvt;
  fGENIETree->GetEntry(entry);

  // Setup Input scaling for joint inputs
  fNUISANCEEvent->RWWeight = 1.0;
  fNUISANCEEvent->InputWeight = 1.0;
  // if (jointinput) {
  //   fNUISANCEEvent->InputWeight = GetInputWeight(entry);
  // } else {
  //   fNUISANCEEvent->InputWeight = 1.0;
  // }

  // Run NUISANCE Vector Filler
  CalcNUISANCEKinematics();

  return fNUISANCEEvent;
#endif
  return NULL;
}


#ifdef __GENIE_ENABLED__
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
  if (gheprec->Summary()->ProcInfo().IsEM()){
    if (gheprec->Summary()->InitState().ProbePdg() == 11) {
      if (gheprec->Summary()->ProcInfo().IsQuasiElastic()) return 1;
      else if (gheprec->Summary()->ProcInfo().IsMEC()) return 2;
      else if (gheprec->Summary()->ProcInfo().IsResonant()) return 13;
      else if (gheprec->Summary()->ProcInfo().IsDeepInelastic()) return 26;
      // else if (gheprec->Summary()->ProcInfo().InteractionTypeId() == 1 && gheprec->Summary()->ProcInfo().ScatteringTypeId() == 10) return 2;
      else {
	ERR(WRN) << "Unknown GENIE Electron Scattering Mode!" << std::endl
		 << "ScatteringTypeId = " << gheprec->Summary()->ProcInfo().ScatteringTypeId() << " "
		 << "InteractionTypeId = " << gheprec->Summary()->ProcInfo().InteractionTypeId() << std::endl 
		 << genie::ScatteringType::AsString(gheprec->Summary()->ProcInfo().ScatteringTypeId()) << " " << genie::InteractionType::AsString(gheprec->Summary()->ProcInfo().InteractionTypeId()) << " " << gheprec->Summary()->ProcInfo().IsMEC() << std::endl;
	return 0;
      }
    }

    // Weak CC
  } else if (gheprec->Summary()->ProcInfo().IsWeakCC()){

    // CC MEC
    if (gheprec->Summary()->ProcInfo().IsMEC()) {
      if (pdg::IsNeutrino(gheprec->Summary()->InitState().ProbePdg()))  return 2;
      else if (pdg::IsAntiNeutrino(gheprec->Summary()->InitState().ProbePdg()))	return -2;

    // CC OTHER
    } else {
      return utils::ghep::NeutReactionCode(gheprec);
    }
   
    // Weak NC
  } else if (gheprec->Summary()->ProcInfo().IsWeakNC()){
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
#endif

void GENIEInputHandler::CalcNUISANCEKinematics() {
#ifdef __GENIE_ENABLED__
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
  fNUISANCEEvent->fBound = 0.0;
  fNUISANCEEvent->InputWeight = 1.0; //(1E+38 / genie::units::cm2) * fGenieGHep->XSec();

  // Get N Particle Stack
  unsigned int npart = fGenieGHep->GetEntries();
  unsigned int kmax = fNUISANCEEvent->kMaxParticles;
  if (npart > kmax) {
    ERR(FTL) << "GENIE has too many particles" << std::endl;
    ERR(FTL) << "npart=" << npart << " kMax=" << kmax << std::endl;
    throw;
  }

  // Initialise Extra NEUT Information in NUISANCE Event
  bool save_extra = false;//FitPar::Config().GetParB("save_extra_genie_info");
  if (save_extra) {
    // Add one of these for each exta piece of information you
    // add to FitEvent for specific generators.
    // Example below is the NEUT One
    // if (!fNUISANCEEvent->fNEUT_ParticleStatusCode)
    // fNUISANCEEvent->fNEUT_ParticleStatusCode = new double[kmax];
  }

  // Fill Particle Stack
  GHepParticle* p = 0;
  TObjArrayIter iter(fGenieGHep);
  fNUISANCEEvent->fNParticles = 0;

  // Loop over all particles
  while ((p = (dynamic_cast<genie::GHepParticle*>((iter).Next())))) {
    if (!p) continue;

    // Get Status
    int state = GetGENIEParticleStatus(p);

    // Remove Undefined
    // if (kRemoveUndefParticles &&
    //  state == kUndefinedState) continue;

    // Remove FSI
    // if (kRemoveFSIParticles &&
    //  state == kFSIState) continue;

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

    // Fill extra information, NEUT Example Given
    // if (save_extra){
    //  fNUISANCEEvent->fNEUT_ParticleStatusCode[curpart] = part->fStatus;
    //  fNUISANCEEvent->fNEUT_ParticleStatusCode[curpart] = part->fIsAlive;
    // }

    // Add to N particle count
    fNUISANCEEvent->fNParticles++;

    // Extra Check incase GENIE fails.
    if ((UInt_t)fNUISANCEEvent->fNParticles == kmax) {
      ERR(WRN) << "Number of GENIE Particles exceeds maximum!" << std::endl;
      ERR(WRN) << "Extend kMax, or run without including FSI particles!" << std::endl;
      break;
    }
  }

  // Run Initial, FSI, Final, Other ordering.
  fNUISANCEEvent-> OrderStack();

  return;
#endif
}

double GENIEInputHandler::GetInputWeight(int entry) {
#ifdef __GENIE_ENABLED__
  // Find Switch Scale
  while ( entry < jointindexlow[jointindexswitch] ||
          entry >= jointindexhigh[jointindexswitch] ) {
    jointindexswitch++;

    // Loop Around
    if (jointindexswitch == jointindexlow.size()) {
      jointindexswitch = 0;
    }
  }
  return jointindexscale[jointindexswitch];
#endif
  return 0.0;
};


BaseFitEvt* GENIEInputHandler::GetBaseEvent(const UInt_t entry) {
#ifdef __GENIE_ENABLED__

  if (entry >= (UInt_t)fNEvents) return NULL;

  // Read entry from TTree to fill GENIE Vect in BaseFitEvt;
  fGENIETree->GetEntry(entry);

  // Set joint scaling if required
  if (jointinput) {
    fBaseEvent->InputWeight = GetInputWeight(entry);
  } else {
    fBaseEvent->InputWeight = 1.0;
  }

  return fBaseEvent;
#endif
  return NULL;
}

void GENIEInputHandler::Print() {}


