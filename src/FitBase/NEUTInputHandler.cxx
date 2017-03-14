#ifdef __NEUT_ENABLED__
#include "NEUTInputHandler.h"

void NEUTGeneratorInfo::AddBranchesToTree(TTree * tn) {
	tn->Branch("NEUT_ParticleStatusCode",
	           fNEUT_ParticleStatusCode, "NEUT_ParticleStatusCode[NParticles]/I");
	tn->Branch("NEUT_ParticleAliveCode",
	           fNEUT_ParticleStatusCode, "NEUT_ParticleAliveCode[NParticles]/I");
}

void NEUTGeneratorInfo::Reset() {
	for (size_t i = 0; i < kMaxParticles; i++) {
		fNEUT_ParticleStatusCode[i] = -1;
		fNEUT_ParticleAliveCode[i]  = 9;
	}
}

NEUTInputHandler::NEUTInputHandler(std::string const& handle, std::string const& rawinputs) {

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
		LOG(SAM) << "  -> Found input file: " << inputs[inp_it] << std::endl;
	}

	// Setup the TChain
	fNEUTTree = new TChain("neuttree");

	// Need to keep a map of flux histograms for PDG. And allow measurements to redfine flux.

	// Loop over all inputs and grab flux, eventhist, and nevents
	// Also add it to the TChain
	int evtcounter = 0;
	if (inputs.size() > 1) jointinput = true;
	for (size_t inp_it = 0; inp_it < inputs.size(); ++inp_it) {

		// Add to TChain
		fNEUTTree->Add( inputs[inp_it].c_str() );

		// Open File for histogram access
		TFile* inp_file = new TFile(inputs[inp_it].c_str(), "READ");

		// Get Flux/Event hist
		TH1D* fluxhist  = (TH1D*)inp_file->Get(
		                      (PlotUtils::GetObjectWithName(inp_file, "flux")).c_str());
		TH1D* eventhist = (TH1D*)inp_file->Get(
		                      (PlotUtils::GetObjectWithName(inp_file, "evt")).c_str());

		// Get N events
		TTree* neuttree = (TTree*)inp_file->Get("neuttree");
		int nevents = neuttree->GetEntries();

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

	}

	// Setup NEvents and the FitEvent
	fNEvents = fNEUTTree->GetEntries();
	fEventType = kNEUT;
	fNeutVect = NULL;
	fNEUTTree->SetBranchAddress("vectorbranch", &fNeutVect);

	fNUISANCEEvent = new FitEvent(fNeutVect);
	fNUISANCEEvent->HardReset();
	fBaseEvent = static_cast<BaseFitEvt*>(fNUISANCEEvent);


	// Normalise event histograms for relative flux contributions.
	for (size_t i = 0; i < jointeventinputs.size(); i++) {
		TH1D* eventhist = (TH1D*) jointeventinputs.at(i)->Clone();

		// Determine nallowed
		int nallowed = jointindexhigh[i] - jointindexlow[i];
		if (fMaxEvents != -1) {
			nallowed = int( double(nallowed) *
			                (double(fMaxEvents) / double(fNEvents)) );
		}
		jointindexallowed.push_back(nallowed);

		LOG(SAM) << " -> Reading " << nallowed
		         << " events from " << inputs[i] << std::endl;

		// Set scale, undoing other scale factor.
		double scale = double(fNEvents) / fEventHist->Integral("width");
		scale *= eventhist->Integral("width");
		scale /= double(jointindexallowed[i]);

		jointindexscale .push_back(scale);
	}

	fEventHist->SetNameTitle((fName + "_EVT").c_str(), (fName + "_EVT").c_str());
	fFluxHist->SetNameTitle((fName + "_FLUX").c_str(), (fName + "_FLUX").c_str());

	// Setup extra flags
	fSaveExtra = FitPar::Config().GetParB("save_extra_neut_info");
	if (fSaveExtra) {
		fNeutInfo = new NEUTGeneratorInfo();
		fNUISANCEEvent->fGenInfo = fNeutInfo;
	}

	// Setup Max Events
	if (fMaxEvents > 1 && fMaxEvents < fNEvents) {
		LOG(SAM) << " -> Reading only " << fMaxEvents << " events from total."
		         << std::endl;
		fNEvents = fMaxEvents;
	}

};


FitEvent* NEUTInputHandler::GetNuisanceEvent(const UInt_t entry) {

	// Catch too large entries
       if (entry >= (UInt_t)fNEvents) return NULL;

	// Read Entry from TTree to fill NEUT Vect in BaseFitEvt;
	fNEUTTree->GetEntry(entry);
	fNUISANCEEvent->eventid = entry;

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

int NEUTInputHandler::GetNeutParticleStatus(NeutPart* part) {

	// State
	int state = kUndefinedState;

	// fStatus == -1 means initial  state
	if (part->fIsAlive == false && part->fStatus == -1) {
		state = kInitialState;

		// NEUT has a bit of a strange convention for fIsAlive and fStatus
		// combinations
		// for NC and neutrino particle isAlive true/false and status 2 means
		// final state particle
		// for other particles in NC status 2 means it's an FSI particle
		// for CC it means it was an FSI particle
	} else if (part->fStatus == 2) {
		// NC case is a little strange... The outgoing neutrino might be alive or
		// not alive. Remaining particles with status 2 are FSI particles that
		// reinteracted
		if (abs(fNeutVect->Mode) > 30 &&
		        (abs(part->fPID) == 14 || abs(part->fPID) == 12)) {
			state = kFinalState;
			// The usual CC case
		} else if (part->fIsAlive == true) {
			state = kFSIState;
		}
	} else if (part->fIsAlive == true && part->fStatus == 2 &&
	           (abs(part->fPID) == 14 || abs(part->fPID) == 12)) {
		state = kFinalState;

	} else if (part->fIsAlive == true && part->fStatus == 0) {
		state = kFinalState;

	} else if (part->fIsAlive == true) {
		ERR(WRN) << "Undefined NEUT state "
		         << " Alive: " << part->fIsAlive << " Status: " << part->fStatus
		         << " PDG: " << part->fPID << std::endl;
		throw;
	}

	return state;
}

void NEUTInputHandler::CalcNUISANCEKinematics() {

	// Reset all variables
	fNUISANCEEvent->ResetEvent();

	// Fill Globals
	fNUISANCEEvent->fMode    = fNeutVect->Mode;
	fNUISANCEEvent->Mode     = fNeutVect->Mode;
	fNUISANCEEvent->fEventNo = fNeutVect->EventNo;
	fNUISANCEEvent->fTargetA = fNeutVect->TargetA;
	fNUISANCEEvent->fTargetZ = fNeutVect->TargetZ;
	fNUISANCEEvent->fTargetH   = fNeutVect->TargetH;
	fNUISANCEEvent->fBound = bool(fNeutVect->Ibound);

	if (fNUISANCEEvent->fBound) {
		fNUISANCEEvent->fTargetPDG = TargetUtils::GetTargetPDGFromZA(fNUISANCEEvent->fTargetZ,
		                             fNUISANCEEvent->fTargetA);
	} else {
		fNUISANCEEvent->fTargetPDG = 1000010010;
	}

	// Check Particle Stack
	UInt_t npart = fNeutVect->Npart();
	UInt_t kmax = fNUISANCEEvent->kMaxParticles;
	if (npart > kmax) {
		ERR(FTL) << "NEUT has too many particles" << std::endl;
		ERR(FTL) << "npart=" << npart << " kMax=" << kmax << std::endl;
		throw;
	}

	// Fill Particle Stack
	for (size_t i = 0; i < npart; i++) {

		// Get Current Count
		int curpart = fNUISANCEEvent->fNParticles;

		// Get NEUT Particle
		NeutPart* part = fNeutVect->PartInfo(i);

		// State
		int state = GetNeutParticleStatus(part);

		// Remove Undefined
		//    if (kRemoveUndefParticles &&
		//  state == kUndefinedState) continue;

		// Remove FSI
		//    if (kRemoveFSIParticles &&
		//  state == kFSIState) continue;

		// State
		fNUISANCEEvent->fParticleState[curpart] = state;

		// Mom
		fNUISANCEEvent->fParticleMom[curpart][0] = part->fP.X();
		fNUISANCEEvent->fParticleMom[curpart][1] = part->fP.Y();
		fNUISANCEEvent->fParticleMom[curpart][2] = part->fP.Z();
		fNUISANCEEvent->fParticleMom[curpart][3] = part->fP.T();

		// PDG
		fNUISANCEEvent->fParticlePDG[curpart] = part->fPID;

		// Extra
		if (fSaveExtra) {
			fNeutInfo->fNEUT_ParticleStatusCode[curpart] = part->fStatus;
			fNeutInfo->fNEUT_ParticleAliveCode[curpart] = part->fIsAlive;
		}

		// Add up particle count
		fNUISANCEEvent->fNParticles++;
	}

	// Run Initial, FSI, Final, Other ordering.
	fNUISANCEEvent-> OrderStack();
	return;
}


double NEUTInputHandler::GetInputWeight(const UInt_t entry) {

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


BaseFitEvt* NEUTInputHandler::GetBaseEvent(const UInt_t entry) {

	// Read entry from TTree to fill NEUT Vect in BaseFitEvt;
        if (entry >= (UInt_t)fNEvents) return NULL;

	fNEUTTree->GetEntry(entry);
	fBaseEvent->eventid = entry;

	// Set joint scaling if required
	if (jointinput) {
		fBaseEvent->InputWeight = GetInputWeight(entry);
	} else {
		fBaseEvent->InputWeight = 1.0;
	}

	return fBaseEvent;
}
#endif
