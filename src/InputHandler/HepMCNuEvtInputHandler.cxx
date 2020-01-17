#include "HepMCNuEvtInputHandler.h"

#include "FitEvent.h"

HepMCNuEvtInputHandler::~HepMCNuEvtInputHandler() {
  if (rdr) {
    rdr->close();
  }
  delete rdr;
};

void HepMCNuEvtInputHandler::Reset() {
  if (rdr) {
    rdr->close();
  }
  delete rdr;

  rdr = new HepMC3Nu::ReaderRootTree(fInputFile);
  fNEvents = rdr->get_entries();
  fEntriesUsed = 0;

  HepMC3Nu::genruninfo::GRIHelper grih(rdr->run_info());

  fEventHist = new TH1D("eventhist", "eventhist", 1, 0.4, 1.4);
  fEventHist->SetBinContent(1, grih.GetFluxAverageTotalCrossSection() * 1E38);
  fFluxHist = new TH1D("fluxhist", "fluxhist", 1, 0.4, 1.4);
  fFluxHist->SetBinContent(1, 1);
}

HepMCNuEvtInputHandler::HepMCNuEvtInputHandler(std::string const &handle,
                                               std::string const &rawinputs)
    : rdr(0) {
  fEventHist = 0;
  fFluxHist = 0;
  NUIS_LOG(SAM, "Creating HepMCNuEvtInputHandler : " << handle);

  // Run a joint input handling
  fName = handle;
  // Get initial flags
  fMaxEvents = FitPar::Config().GetParI("MAXEVENTS");
  fEventType = kHEPMC;

  std::vector<std::string> inputs = InputUtils::ParseInputFileList(rawinputs);
  if (inputs.size() != 1) {
    NUIS_ABORT(
        "HEPMCNUEVT Input handler can only handler a single input at a time.");
  }
  fInputFile = inputs.front();

  Reset();

  fNUISANCEEvent = new FitEvent();
  fNUISANCEEvent->HardReset();
  fNUISANCEEvent->SetType(fEventType);
  fBaseEvent = static_cast<BaseFitEvt *>(fNUISANCEEvent);
};

FitEvent *HepMCNuEvtInputHandler::GetNuisanceEvent(const UInt_t entry) {

  // Catch too large entries
  if (entry >= (UInt_t)fNEvents) {
    return NULL;
  }

  if (entry < fEntriesUsed) {
    Reset();
    rdr->skip(entry - 1);
  } else if (entry > fEntriesUsed) {
    rdr->skip(entry - fEntriesUsed);
  }

  if (rdr->failed()) {
    return NULL;
  }

  HepMC3::GenEvent evt;

  rdr->read_event(evt);

  fNUISANCEEvent->ResetEvent();

  auto LabFrameVertex = HepMC3Nu::GetLabFrameVertex(evt);

  fNUISANCEEvent->Mode = 1; // fNeutVect->Mode;
  fNUISANCEEvent->fEventNo = evt.event_number();

  UInt_t npart = LabFrameVertex->particles_out().size();
  UInt_t kmax = fNUISANCEEvent->kMaxParticles;
  if (npart > kmax) {
    NUIS_ERR(WRN, "NEUT has too many particles. Expanding stack.");
    fNUISANCEEvent->ExpandParticleStack(npart);
  }

  for (auto pin : LabFrameVertex->particles_in()) {
    int curpart = fNUISANCEEvent->fNParticles;
    // State
    fNUISANCEEvent->fParticleState[curpart] = kInitialState;

    fNUISANCEEvent->fPrimaryVertex[curpart] = true;

    // Mom
    fNUISANCEEvent->fParticleMom[curpart][0] = pin->momentum().x();
    fNUISANCEEvent->fParticleMom[curpart][1] = pin->momentum().y();
    fNUISANCEEvent->fParticleMom[curpart][2] = pin->momentum().z();
    fNUISANCEEvent->fParticleMom[curpart][3] = pin->momentum().e();

    // PDG
    fNUISANCEEvent->fParticlePDG[curpart] = pin->pid();
    fNUISANCEEvent->fNParticles++;
  }
  for (auto pout : LabFrameVertex->particles_out()) {
    int curpart = fNUISANCEEvent->fNParticles;
    // State
    fNUISANCEEvent->fParticleState[curpart] = kFinalState;

    fNUISANCEEvent->fPrimaryVertex[curpart] = false;

    // Mom
    fNUISANCEEvent->fParticleMom[curpart][0] = pout->momentum().x();
    fNUISANCEEvent->fParticleMom[curpart][1] = pout->momentum().y();
    fNUISANCEEvent->fParticleMom[curpart][2] = pout->momentum().z();
    fNUISANCEEvent->fParticleMom[curpart][3] = pout->momentum().e();

    // PDG
    fNUISANCEEvent->fParticlePDG[curpart] = pout->pid();
    fNUISANCEEvent->fNParticles++;
  }

  // Add up particle count
  fNUISANCEEvent->OrderStack();

  FitParticle *ISAnyLepton = fNUISANCEEvent->GetHMISAnyLeptons();
  if (ISAnyLepton) {
    fNUISANCEEvent->probe_E = ISAnyLepton->E();
    fNUISANCEEvent->probe_pdg = ISAnyLepton->PDG();
  }

  // Return event pointer
  return fNUISANCEEvent;
}
