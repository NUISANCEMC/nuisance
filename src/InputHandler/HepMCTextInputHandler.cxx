#ifdef __HEPMC_ENABLED__
#include "HepMCTextInputHandler.h"

HepMCTextInputHandler::~HepMCTextInputHandler(){

};


HepMCTextInputHandler::HepMCTextInputHandler(std::string const& handle, std::string const& rawinputs) {

	NUIS_LOG(SAM, "Creating HepMCTextInputHandler : " << handle );

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
			std::cout << "\t\t|-> Input File " << inp_it
			          << "      : " << inputs[inp_it] << std::endl;
		}
	}

	fEventType = kHEPMC;

	fASCIIStream = std::ifstream(inputs[0].c_str());
	fHepMCEvent = HepMC::GenEvent();

	// Loop through events and get N
	fNEvents = 0;
	while (fASCIIStream){
	  fHepMCEvent.read(fASCIIStream);
	  fNEvents++;
	}
	fASCIIStream.seekg(0);

	fEventHist = new TH1D("eventhist","eventhist",10,0.0,1.0);
	fFluxHist = new TH1D("eventhist","eventhist",10,0.0,1.0);

	fNUISANCEEvent = new FitEvent(kHEPMC);
        fNUISANCEEvent->HardReset();
        fBaseEvent = static_cast<BaseFitEvt*>(fNUISANCEEvent);

};


FitEvent* HepMCTextInputHandler::GetNuisanceEvent(const UInt_t entry) {

	// Catch too large entries
  if (entry >= (UInt_t)fNEvents){
    fASCIIStream.seekg(0);
    return NULL;
  }

  // If no event get the first one
  if (!fHepMCEvent.is_valid()){
    fHepMCEvent.read(fASCIIStream);
  }

  // Read Entry from TTree to fill NEUT Vect in BaseFitEvt;
  if (fHepMCEvent.event_number() != entry){

    // If have to read backwords skip to start of ifstream
    if (fHepMCEvent.event_number() > entry){
      fASCIIStream.seekg(0);
      fHepMCEvent.read(fASCIIStream);
    }

    // Loop to event before if reading forwards
    while (fHepMCEvent.event_number() < entry - 1 and fASCIIStream){
      fHepMCEvent.read(fASCIIStream);
    }

    fHepMCEvent.read(fASCIIStream);
  }

  // Setup Input scaling for joint inputs
  if (jointinput) {
    fNUISANCEEvent->InputWeight = GetInputWeight(entry);
  } else {
    fNUISANCEEvent->InputWeight = 1.0;
  }

  // Run NUISANCE Vector Filler
  CalcNUISANCEKinematics();

  // Return event pointer
  return fNUISANCEEvent;
}

int HepMCTextInputHandler::ConvertHepMCStatus() {

  // Status for HepMC convertor
  return 0;
}

void HepMCTextInputHandler::CalcNUISANCEKinematics() {

	// Reset all variables
	fNUISANCEEvent->ResetEvent();

	// Read all particles from fHepMCEvent
	std::vector<HepMC::GenParticle*> allstateparticles;
	for ( HepMC::GenEvent::particle_iterator p = fHepMCEvent.particles_begin();
	      p != fHepMCEvent.particles_end(); ++p ){
	  allstateparticles.push_back(*p);

	  int pdg = (*p)->pdg_id();
	  int status = (*p)->status();
	  //	  FourVector vect = (*p)->momentum();

	  int state = ConvertHepMCStatus(status);

	  int curpart = fNUISANCEEvent->fNParticles;
	  fNUISANCEEvent->fParticleState[curpart] = state;

	  // Mom
	  //	  fNUISANCEEvent->fParticleMom[curpart][0] = vect.px();
	  //	  fNUISANCEEvent->fParticleMom[curpart][1] = vect.py();
	  //	  fNUISANCEEvent->fParticleMom[curpart][2] = vect.pz();
	  //	  fNUISANCEEvent->fParticleMom[curpart][3] = vect.e();

	  // PDG
	  fNUISANCEEvent->fParticlePDG[curpart] = pdg;

	  // Add up particle count
	  fNUISANCEEvent->fNParticles++;

	}

		// Run Initial, FSI, Final, Other ordering.
	fNUISANCEEvent-> OrderStack();
	return;
}


double HepMCTextInputHandler::GetInputWeight(const UInt_t entry) {

	// Find Switch Scale
	while ( entry < (UInt_t)jointindexlow[jointindexswitch] ||
	        entry >= (UInt_t)jointindexhigh[jointindexswitch] ) {
		jointindexswitch++;

		// Loop Around
		if (jointindexswitch == jointindexlow.size()) {
			jointindexswitch = 0;
		}
	}
	return jointindexscale[jointindexswitch];
};


BaseFitEvt* HepMCTextInputHandler::GetBaseEvent(const UInt_t entry) {

	// Read entry from TTree to fill NEUT Vect in BaseFitEvt;
	if (entry >= (UInt_t)fNEvents) return NULL;
	return (BaseFitEvent*)GetNuisanceEvent(entry);
}
#endif
