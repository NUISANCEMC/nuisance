#include "NuHepMCInputHandler.h"

#include "NuHepMC/CrossSectionUtils.hxx"
#include "NuHepMC/EventUtils.hxx"
#include "NuHepMC/ReaderUtils.hxx"

#include "HepMC3/Print.h"
#include "HepMC3/ReaderFactory.h"

#include <stdexcept>

NuHepMCInputHandler::~NuHepMCInputHandler() {}

struct KBAccumulator {
  double sum;
  double corr;
  KBAccumulator(double s = 0, double c = 0) : sum(s), corr(c) {}

  KBAccumulator &operator+=(double el) {
    double y = el - corr;
    double t = sum + y;

    corr = (t - sum) - y;
    sum = t;

    return *this;
  }
  KBAccumulator operator+(double el) {
    *this += el;
    return *this;
  }
};

NuHepMCInputHandler::NuHepMCInputHandler(std::string const &handle,
                                         std::string const &rawinputs)
    : frun_info(nullptr) {

  NUIS_LOG(SAM, "Creating NuHepMCInputHandler : " << handle);

  // Run a joint input handling
  fName = handle;
  jointinput = false;
  jointindexswitch = 0;

  // Get initial flags
  fMaxEvents = FitPar::Config().GetParI("MAXEVENTS");

  // Form list of all inputs, remove brackets if required.
  std::vector<std::string> inputs = GeneralUtils::ParseToStr(rawinputs, ",");
  if (inputs.front()[0] == '(') {
    inputs.front() = inputs.front().substr(1);
  }
  if (inputs.back()[inputs.back().size() - 1] == ')') {
    inputs.back() = inputs.back().substr(0, inputs.back().size() - 1);
  }
  for (size_t inp_it = 0; inp_it < inputs.size(); ++inp_it) {
    if (LOG_LEVEL(SAM)) {
      std::cout << "\t\t|-> Input File " << inp_it << "      : "
                << inputs[inp_it] << std::endl;
    }
  }

  fEventType = kNuHepMC;

  fFilename = inputs[0];

  double fatx_cm2 = NuHepMC::CrossSection::GetFATX(
      fFilename, NuHepMC::CrossSection::Units::XSUnits::cm2_ten38,
      NuHepMC::CrossSection::Units::XSTargetScale::PerTargetMolecularNucleon);

  std::cout << "NuHepMCInputHandler: FATX = " << fatx_cm2 << std::endl;

  fReader = HepMC3::deduce_reader(fFilename);
  if (!fReader) {
    NUIS_ABORT("Failed to instantiate HepMC3::Reader from " << fFilename);
  }
  HepMC3::GenEvent evt;
  fNEvents = 0;
  KBAccumulator sumw;
  double sumw_d = 0;
  while (!fReader->failed()) {
    fReader->read_event(evt);
    if (!fNEvents) {
      fToMeV = NuHepMC::Event::ToMeVFactor(evt);
    }
    if (!fReader->failed()) {
      fNEvents++;
    } else {
      break;
    }

    sumw += evt.weights()[0];
    sumw_d += evt.weights()[0];
  }
  fsumevw = sumw.sum;

  std::cout << "sumw = " << sumw.sum << std::endl;
  std::cout << "sumw_d = " << sumw_d << std::endl;
  // Dupe the FATX
  fEventHist = new TH1D("eventhist", "eventhist", 10, 0.0, 10.0);
  fEventHist->SetBinContent(5, fatx_cm2);
  fFluxHist = new TH1D("fluxhist", "fluxhist", 10, 0.0, 10.0);
  fFluxHist->SetBinContent(5, 1);

  fNUISANCEEvent = new FitEvent();
  fNUISANCEEvent->HardReset();
  fBaseEvent = static_cast<BaseFitEvt *>(fNUISANCEEvent);

  fReader = HepMC3::deduce_reader(fFilename);
};

FitEvent *NuHepMCInputHandler::GetNuisanceEvent(const UInt_t entry, bool) {

  int ntoskip = 0;

  if (nextentry != entry) {
    if (nextentry > entry) {
      // start the file again
      fReader = HepMC3::deduce_reader(fFilename);
      ntoskip = entry;
    } else {
      ntoskip = entry - nextentry;
    }
  }

  if (ntoskip) {
    fReader->skip(ntoskip);
  }

  nextentry = entry + 1;

  fReader->read_event(fHepMC3Evt);

  // Catch too large entries
  if (fReader->failed()) {
    return NULL;
  }

  // Setup Input scaling for joint inputs
  if (jointinput) {
    fNUISANCEEvent->InputWeight = GetInputWeight(entry);
  } else {
    fNUISANCEEvent->InputWeight = 1.0;
  }

  fNUISANCEEvent->InputWeight *=
      fHepMC3Evt.weights()[0] * double(fNEvents) / fsumevw;

  // Run NUISANCE Vector Filler
  CalcNUISANCEKinematics();

  // Return event pointer
  return fNUISANCEEvent;
}

void NuHepMCInputHandler::CalcNUISANCEKinematics() {

  // Reset all variables
  fNUISANCEEvent->ResetEvent();

  fNUISANCEEvent->Mode = NuHepMC::ER3::ReadProcessID(fHepMC3Evt);

  fNUISANCEEvent->fEventNo = fHepMC3Evt.event_number();

  // Read all particles from fHepMCEvent
  fNUISANCEEvent->fNParticles = 0;
  for (auto const &p : fHepMC3Evt.particles()) {

    int status = 0;

    if (p->pid() == NuHepMC::ParticleNumber::NuclearRemnant) {
      // ignore nuclear remnants
      continue;
    }

    if (p->status() == NuHepMC::ParticleStatus::IncomingBeam) {
      status = kInitialState;
    } else if (p->status() == NuHepMC::ParticleStatus::Target) {

      status = kNuclearInitial;

      fNUISANCEEvent->fTargetA = (p->pid() / 10) % 1000;
      fNUISANCEEvent->fTargetZ = (p->pid() / 10000) % 1000;
      fNUISANCEEvent->fTargetH = 0;
      fNUISANCEEvent->fBound = (p->pid() == 1000010010);

    } else if (p->status() == NuHepMC::ParticleStatus::StruckNucleon) {
      status = kInitialState;
    } else if (p->status() == NuHepMC::ParticleStatus::UndecayedPhysical) {
      status = kFinalState;
    } else {
      // Ignore internal particles
      continue;
    }

    fNUISANCEEvent->fPrimaryVertex[fNUISANCEEvent->fNParticles] =
        (p->production_vertex()->status() == NuHepMC::VertexStatus::Primary);

    // Mom
    fNUISANCEEvent->fParticleMom[fNUISANCEEvent->fNParticles][0] =
        p->momentum().px() * fToMeV;
    fNUISANCEEvent->fParticleMom[fNUISANCEEvent->fNParticles][1] =
        p->momentum().py() * fToMeV;
    fNUISANCEEvent->fParticleMom[fNUISANCEEvent->fNParticles][2] =
        p->momentum().pz() * fToMeV;
    fNUISANCEEvent->fParticleMom[fNUISANCEEvent->fNParticles][3] =
        p->momentum().e() * fToMeV;

    // PDG
    fNUISANCEEvent->fParticlePDG[fNUISANCEEvent->fNParticles] = p->pid();
    fNUISANCEEvent->fParticleState[fNUISANCEEvent->fNParticles] = status;

    // Add up particle count
    fNUISANCEEvent->fNParticles++;
  }

  // Run Initial, FSI, Final, Other ordering.
  fNUISANCEEvent->OrderStack();

  return;
}

double NuHepMCInputHandler::GetInputWeight(const UInt_t entry) {

  // Find Switch Scale
  while (entry < (UInt_t)jointindexlow[jointindexswitch] ||
         entry >= (UInt_t)jointindexhigh[jointindexswitch]) {
    jointindexswitch++;

    // Loop Around
    if (jointindexswitch == jointindexlow.size()) {
      jointindexswitch = 0;
    }
  }
  return jointindexscale[jointindexswitch];
};

BaseFitEvt *NuHepMCInputHandler::GetBaseEvent(const UInt_t entry) {

  if (entry >= (UInt_t)fNEvents)
    return NULL;
  return (BaseFitEvt *)GetNuisanceEvent(entry, true);
}
