#ifdef GiBUU_ENABLED
#include "GIBUUInputHandler.h"
#include "InputUtils.h"

GIBUUGeneratorInfo::~GIBUUGeneratorInfo() { DeallocateParticleStack(); }

void GIBUUGeneratorInfo::AddBranchesToTree(TTree *tn) {
  (void)tn;
  // tn->Branch("NEUTParticleN",          fNEUTParticleN, "NEUTParticleN/I");
  // tn->Branch("NEUTParticleStatusCode", fNEUTParticleStatusCode,
  // "NEUTParticleStatusCode[NEUTParticleN]/I");
  // tn->Branch("NEUTParticleAliveCode",  fNEUTParticleAliveCode,
  // "NEUTParticleAliveCode[NEUTParticleN]/I");
}

void GIBUUGeneratorInfo::SetBranchesFromTree(TTree *tn) {
  (void)tn;
  // tn->SetBranchAddress("NEUTParticleN",          &fNEUTParticleN );
  // tn->SetBranchAddress("NEUTParticleStatusCode", &fNEUTParticleStatusCode );
  // tn->SetBranchAddress("NEUTParticleAliveCode",  &fNEUTParticleAliveCode  );
}

void GIBUUGeneratorInfo::AllocateParticleStack(int stacksize) {
  (void)stacksize;
  // fNEUTParticleN = 0;
  // fNEUTParticleStatusCode = new int[stacksize];
  // fNEUTParticleStatusCode = new int[stacksize];
}

void GIBUUGeneratorInfo::DeallocateParticleStack() {
  // delete fNEUTParticleStatusCode;
  // delete fNEUTParticleAliveCode;
}

void GIBUUGeneratorInfo::FillGeneratorInfo(GiBUUStdHepReader *nevent) {
  (void)nevent;
  Reset();
  // for (int i = 0; i < nevent->Npart(); i++) {
  // fNEUTParticleStatusCode[i] = nevent->PartInfo(i)->fStatus;
  // fNEUTParticleAliveCode[i]  = nevent->PartInfo(i)->fIsAlive;
  // fNEUTParticleN++;
  // }
}

void GIBUUGeneratorInfo::Reset() {
  // for (int i = 0; i < fNEUTParticleN; i++) {
  // fNEUTParticleStatusCode[i] = -1;
  // fNEUTParticleAliveCode[i]  = 9;
  // }
  // fNEUTParticleN = 0;
}

GIBUUInputHandler::GIBUUInputHandler(std::string const &handle,
                                     std::string const &rawinputs) {
  NUIS_LOG(SAM, "Creating GiBUUInputHandler : " << handle);

  // Run a joint input handling
  fName = handle;
  fEventType = kGiBUU;
  fGIBUUTree = new TChain("giRooTracker");

  // Loop over all inputs and grab flux, eventhist, and nevents
  std::vector<std::string> inputs = InputUtils::ParseInputFileList(rawinputs);
  for (size_t inp_it = 0; inp_it < inputs.size(); ++inp_it) {
    // Open File for histogram access
    NUIS_LOG(SAM, "Opening event file " << inputs[inp_it]);
    TFile *inp_file = new TFile(inputs[inp_it].c_str(), "READ");
    if ((!inp_file) || (!inp_file->IsOpen())) {
      NUIS_ABORT(
          "GiBUU file !IsOpen() at : '"
          << inputs[inp_it] << "'" << std::endl
          << "Check that your file paths are correct and the file exists!");
    }

    int NFluxes = bool(dynamic_cast<TH1D *>(inp_file->Get("numu_flux"))) +
                  bool(dynamic_cast<TH1D *>(inp_file->Get("numub_flux"))) +
                  bool(dynamic_cast<TH1D *>(inp_file->Get("nue_flux"))) +
                  bool(dynamic_cast<TH1D *>(inp_file->Get("nueb_flux"))) +
                  bool(dynamic_cast<TH1D *>(inp_file->Get("e_flux")));

    if (NFluxes != 1) {
      NUIS_ABORT("Found " << NFluxes << " input fluxes in " << inputs[inp_it]
                          << ". The NUISANCE GiBUU interface expects to be "
                             "passed multiple species vectors as separate "
                             "input files like: "
                             "\"GiBUU:(MINERVA_FHC_numu_evts.root,MINERVA_FHC_"
                             "numubar_evts.root,[...])\"");
    }

    // Get Flux/Event hist
    TH1D *fluxhist = dynamic_cast<TH1D *>(inp_file->Get("flux"));
    TH1D *eventhist = dynamic_cast<TH1D *>(inp_file->Get("evt"));
    if (!fluxhist || !eventhist) {
      NUIS_ERR(FTL, "Input File Contents: " << inputs[inp_it]);
      inp_file->ls();
      NUIS_ABORT("GiBUU FILE doesn't contain flux/xsec info. You may have to "
                 "regenerate your MC!");
    }

    // Get N Events
    TTree *giRooTracker = dynamic_cast<TTree *>(inp_file->Get("giRooTracker"));
    if (!giRooTracker) {
      NUIS_ERR(FTL, "giRooTracker Tree not located in NEUT file: "
                        << inputs[inp_it]);
      NUIS_ABORT(
          "Check your inputs, they may need to be completely regenerated!");
      throw;
    }
    int nevents = giRooTracker->GetEntries();
    if (nevents <= 0) {
      NUIS_ABORT("Trying to a TTree with "
                 << nevents << " to TChain from : " << inputs[inp_it]);
    }

    // Register input to form flux/event rate hists
    RegisterJointInput(inputs[inp_it], nevents, fluxhist, eventhist);

    // Add To TChain
    fGIBUUTree->AddFile(inputs[inp_it].c_str());
  }

  // Registor all our file inputs
  SetupJointInputs();

  // Create Fit Event
  fNUISANCEEvent = new FitEvent();

  fGiReader = new GiBUUStdHepReader();
  fGiReader->SetBranchAddresses(fGIBUUTree);

  fNUISANCEEvent->HardReset();
};

FitEvent *GIBUUInputHandler::GetNuisanceEvent(const UInt_t ent,
                                              const bool lightweight) {
  UInt_t entry = ent + fSkip;
  // Check out of bounds
  if (entry >= (UInt_t)fNEvents)
    return NULL;

  // Read Entry from TTree to fill NEUT Vect in BaseFitEvt;
  fGIBUUTree->GetEntry(entry);

  // Run NUISANCE Vector Filler
  if (!lightweight) {
    CalcNUISANCEKinematics();
  }
#ifdef Prob3plusplus_ENABLED
  else {
    for (int i = 0; i < fGiReader->StdHepN; i++) {
      int state = GetGIBUUParticleStatus(fGiReader->StdHepStatus[i],
                                         fGiReader->StdHepPdg[i]);
      if (state != kInitialState) {
        continue;
      }
      if (std::count(PhysConst::pdg_neutrinos, PhysConst::pdg_neutrinos + 4,
                     fGiReader->StdHepPdg[i])) {
        fNUISANCEEvent->probe_E = fGiReader->StdHepP4[i][3] * 1.E3;
        fNUISANCEEvent->probe_pdg = fGiReader->StdHepPdg[i];
        break;
      }
    }
  }
#endif

  fNUISANCEEvent->InputWeight *= GetInputWeight(entry);
  fNUISANCEEvent->GiRead = fGiReader;

  return fNUISANCEEvent;
}

int GetGIBUUParticleStatus(int status, int pdg) {
  int state = kUndefinedState;
  switch (status) {
  case 0:  // Incoming
  case 11: // Struck nucleon
    state = kInitialState;
    break;

  case 1: // Good Final State
    state = kFinalState;
    break;

  default: // Other
    break;
  }

  // Set Nuclear States Flag
  if (pdg > 1000000) {
    if (state == kInitialState)
      state = kNuclearInitial;
    else if (state == kFinalState)
      state = kNuclearRemnant;
    else
      state = kUndefinedState;
  }

  return state;
}

void GIBUUInputHandler::CalcNUISANCEKinematics() {
  // Reset all variables
  fNUISANCEEvent->ResetEvent();
  FitEvent *evt = fNUISANCEEvent;
  evt->Mode = fGiReader->GiBUU2NeutCode;
  evt->fEventNo = 0.0;
  evt->fTotCrs = 0;
  evt->fTargetA = 0.0; // Change to get these from nuclear remnant.
  evt->fTargetZ = 0.0;
  evt->fTargetH = 0;
  evt->fBound = 0.0;

  // Extra GiBUU Input Weight
  evt->InputWeight = fGiReader->EvtWght;

  // Check Stack N
  int npart = fGiReader->StdHepN;
  int kmax = evt->kMaxParticles;
  if ((UInt_t)npart > (UInt_t)kmax) {
    NUIS_ERR(WRN, "GiBUU has too many particles. Expanding Stack.");
    fNUISANCEEvent->ExpandParticleStack(npart);
  }

  // Create Stack
  evt->fNParticles = 0;
  for (int i = 0; i < npart; i++) {
    // State
    int state = GetGIBUUParticleStatus(fGiReader->StdHepStatus[i],
                                       fGiReader->StdHepPdg[i]);
    int curpart = evt->fNParticles;

    // Set State
    evt->fParticleState[evt->fNParticles] = state;

    // Mom
    evt->fParticleMom[curpart][0] = fGiReader->StdHepP4[i][0] * 1.E3;
    evt->fParticleMom[curpart][1] = fGiReader->StdHepP4[i][1] * 1.E3;
    evt->fParticleMom[curpart][2] = fGiReader->StdHepP4[i][2] * 1.E3;
    evt->fParticleMom[curpart][3] = fGiReader->StdHepP4[i][3] * 1.E3;

    // PDG
    evt->fParticlePDG[curpart] = fGiReader->StdHepPdg[i];

    // Add to total particles
    evt->fNParticles++;
  }

  // Run Initial, FSI, Final, Other ordering.
  fNUISANCEEvent->OrderStack();

  FitParticle *ISAnyLepton = fNUISANCEEvent->GetHMISAnyLeptons();
  if (ISAnyLepton) {
    fNUISANCEEvent->probe_E = ISAnyLepton->E();
    fNUISANCEEvent->probe_pdg = ISAnyLepton->PDG();
  }

  return;
}

void GIBUUInputHandler::Print() {}

void GIBUUInputHandler::SetupJointInputs() {
  if (jointeventinputs.size() <= 1) {
    jointinput = false;
  } else if (jointeventinputs.size() > 1) {
    jointinput = true;
    jointindexswitch = 0;
  }
  fMaxEvents = FitPar::Config().GetParI("MAXEVENTS");
  if (fMaxEvents != -1 and jointeventinputs.size() > 1) {
    NUIS_ABORT("Can only handle joint inputs when config MAXEVENTS = -1!");
  }

  for (size_t i = 0; i < jointeventinputs.size(); i++) {
    double scale = double(fNEvents) / fEventHist->Integral("width");
    scale *= jointfluxinputs.at(i)->Integral("width");

    jointindexscale.push_back(scale);
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

  // Print out Status
  if (LOG_LEVEL(SAM)) {
    std::cout << "\t\t|-> Total Entries    : " << fNEvents << std::endl
              << "\t\t|-> Event Integral   : "
              << fEventHist->Integral("width") * 1.E-38 << " events/nucleon"
              << std::endl
              << "\t\t|-> Flux Integral    : " << fFluxHist->Integral("width")
              << " /cm2" << std::endl
              << "\t\t|-> Event/Flux       : "
              << fEventHist->Integral("width") * 1.E-38 /
                     fFluxHist->Integral("width")
              << " cm2/nucleon" << std::endl;
  }
}

#endif
