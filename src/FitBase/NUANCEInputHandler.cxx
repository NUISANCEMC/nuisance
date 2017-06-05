#ifdef __NUANCE_ENABLED__
#include "NuanceEvent.h"
#include "NUANCEInputHandler.h"

NUANCEInputHandler::NUANCEInputHandler(std::string const& handle, std::string const& rawinputs) {

	// Run a joint input handling
	fName = handle;

	// Read in Nuance output ROOT file (converted from hbook)
	LOG(SAM) << " Reading NUANCE " << std::endl;
	fEventType = kNUANCE;

	std::vector<std::string> inputs = GeneralUtils::ParseToStr(rawinputs, ",");
	if (inputs.size() > 1) {
		ERR(FTL) << "NUANCE is not currently setup to handle joint inputs sorry!" << std::endl
		         << "If you know how to correctly normalise the events for this"
		         << " please let us know!" << std::endl;
	}

	// Read in NUANCE Tree
	fNUANCETree = new TChain("h3");
	fNUANCETree->AddFile(rawinputs.c_str());

	// Get entries and fNuwroEvent
	fNEvents = fNUANCETree->GetEntries();
	fNuanceEvent = new NuanceEvent();
	fNuanceEvent->SetBranchAddresses(fNUANCETree);

	double EnuMin = 0.0;     // tn->GetMinimum("p_neutrino[3]");
	double EnuMax = 1000.0;  // tn->GetMaximum("p_neutrino[3]");

	fFluxHist = new TH1D((fName + "_FLUX").c_str(), (fName + "_FLUX").c_str(),
	                     100, EnuMin, EnuMax);
	for (int i = 0; i < fFluxHist->GetNbinsX(); i++) {
		fFluxHist->SetBinContent(i + 1, 1.0);
	}
	fFluxHist->Scale(1.0 / fFluxHist->Integral());

	fEventHist = new TH1D((fName + "_EVT").c_str(), (fName + "_EVT").c_str(), 100,
	                      EnuMin, EnuMax);
	for (int i = 0; i < fFluxHist->GetNbinsX(); i++) {
		fEventHist->SetBinContent(i + 1, 1.0);
	}
	fEventHist->Scale(1.0 / fEventHist->Integral());

	fXSecHist = (TH1D*)fEventHist->Clone();
	fXSecHist->Divide(fFluxHist);

};


FitEvent* NUANCEInputHandler::GetNuisanceEvent(const UInt_t entry) {

	// Make sure events setup
	if (!fNUISANCEEvent) fNUISANCEEvent = new FitEvent(fNuanceEvent);

	// Read Entry from TTree to fill NEUT Vect in BaseFitEvt;
	fNUANCETree->GetEntry(entry);

	// Setup Input scaling for joint inputs
	if (jointinput) {
		fNUISANCEEvent->InputWeight = GetInputWeight(entry);
	} else {
		fNUISANCEEvent->InputWeight = 1.0;
	}

	// Run NUISANCE Vector Filler
	CalcNUISANCEKinematics();

	return fNUISANCEEvent;
}


void NUANCEInputHandler::CalcNUISANCEKinematics() {

	// Reset all variables
	fNUISANCEEvent->ResetEvent();

	// Get shortened pointer
	FitEvent* evt = fNUISANCEEvent;

	// Fill Global
	evt->fMode = GeneratorUtils::ConvertNuanceMode(fNuanceEvent);
	evt->Mode = evt->fMode;
	evt->fEventNo = 0.0;
	evt->fTotCrs = 1.0;
	evt->fTargetA = 0.0;
	evt->fTargetZ = 0.0;
	evt->fTargetH = 0;
	evt->fBound = 0.0;

	// Fill particle Stack
	evt->fNParticles = 0;

	// Check Particle Stack
	UInt_t npart = 2 + fNuanceEvent->n_leptons + fNuanceEvent->n_hadrons;
	UInt_t kmax = evt->kMaxParticles;
	if (npart > kmax) {
		ERR(FTL) << "NUANCE has too many particles" << std::endl;
		ERR(FTL) << "npart=" << npart << " kMax=" << kmax << std::endl;
		throw;
	}

	// Fill Neutrino
	evt->fParticleState[0] = kInitialState;
	evt->fParticleMom[0][0] = fNuanceEvent->p_neutrino[0];
	evt->fParticleMom[0][1] = fNuanceEvent->p_neutrino[1];
	evt->fParticleMom[0][2] = fNuanceEvent->p_neutrino[2];
	evt->fParticleMom[0][3] = fNuanceEvent->p_neutrino[3];
	evt->fParticlePDG[0] = fNuanceEvent->neutrino;

	// Fill Target Nucleon
	evt->fParticleState[1] = kInitialState;
	evt->fParticleMom[1][0] = fNuanceEvent->p_targ[0];
	evt->fParticleMom[1][1] = fNuanceEvent->p_targ[1];
	evt->fParticleMom[1][2] = fNuanceEvent->p_targ[2];
	evt->fParticleMom[1][3] = fNuanceEvent->p_targ[3];
	evt->fParticlePDG[1] = fNuanceEvent->target;
	evt->fNParticles = 2;

	// Fill Outgoing Leptons
	for (int i = 0; i < fNuanceEvent->n_leptons; i++) {
		evt->fParticleState[evt->fNParticles] = kFinalState;
		evt->fParticleMom[evt->fNParticles][0] = fNuanceEvent->p_lepton[i][0];
		evt->fParticleMom[evt->fNParticles][1] = fNuanceEvent->p_lepton[i][1];
		evt->fParticleMom[evt->fNParticles][2] = fNuanceEvent->p_lepton[i][2];
		evt->fParticleMom[evt->fNParticles][3] = fNuanceEvent->p_lepton[i][3];
		evt->fParticlePDG[evt->fNParticles] = fNuanceEvent->lepton[i];
		evt->fNParticles++;
	}

	// Fill Outgoing Hadrons
	for (int i = 0; i < fNuanceEvent->n_hadrons; i++) {
		evt->fParticleState[evt->fNParticles] = kFinalState;
		evt->fParticleMom[evt->fNParticles][0] = fNuanceEvent->p_hadron[i][0];
		evt->fParticleMom[evt->fNParticles][1] = fNuanceEvent->p_hadron[i][1];
		evt->fParticleMom[evt->fNParticles][2] = fNuanceEvent->p_hadron[i][2];
		evt->fParticleMom[evt->fNParticles][3] = fNuanceEvent->p_hadron[i][3];
		evt->fParticlePDG[evt->fNParticles] = fNuanceEvent->hadron[i];
		evt->fNParticles++;
	}

	// Run Initial, FSI, Final, Other ordering.
	fNUISANCEEvent-> OrderStack();
	return;
}


double NUANCEInputHandler::GetInputWeight(int entry) {

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
};


BaseFitEvt* NUANCEInputHandler::GetBaseEvent(const UInt_t entry) {

	// Make sure events setup
	// if (!fBaseEvent) fBaseEvent = new BaseFitEvt(fNeutVect);

	// Read entry from TTree to fill NEUT Vect in BaseFitEvt;
	fNUANCETree->GetEntry(entry);

	// Set joint scaling if required
	if (jointinput) {
		fBaseEvent->InputWeight = GetInputWeight(entry);
	} else {
		fBaseEvent->InputWeight = 1.0;
	}

	return fBaseEvent;
}

void NUANCEInputHandler::Print() {}

#endif

