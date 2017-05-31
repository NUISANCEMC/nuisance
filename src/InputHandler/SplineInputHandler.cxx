#include "SplineInputHandler.h"

SplineInputHandler::SplineInputHandler(std::string const& handle, std::string const& rawinputs) {
	LOG(SAM) << "Creating SplineInputHandler : " << handle << std::endl;

	// Run a joint input handling
	fName = handle;
	fCacheSize = FitPar::Config().GetParI("CacheSize");
	fMaxEvents = FitPar::Config().GetParI("MAXEVENTS");

	// Setup the TChain
	fFitEventTree = new TChain("nuisance_events");

	// Open File for histogram access
	TFile* inp_file = new TFile(rawinputs.c_str(), "READ");
	if (!inp_file or inp_file->IsZombie()) {
		ERR(FTL) << "FitEvent File IsZombie() at " << rawinputs << std::endl;
		throw;
	}

	// Get Flux/Event hist
	TH1D* fluxhist  = (TH1D*)inp_file->Get("nuisance_fluxhist");
	TH1D* eventhist = (TH1D*)inp_file->Get("nuisance_eventhist");
	if (!fluxhist or !eventhist) {
		ERR(FTL) << "FitEvent FILE doesn't contain flux/xsec info" << std::endl;
		throw;
	}

	// Get N Events
	TTree* eventtree = (TTree*)inp_file->Get("nuisance_events");
	if (!eventtree) {
		ERR(FTL) << "nuisance_events not located in FitSpline file! " << rawinputs << std::endl;
		throw;
	}
	int nevents = eventtree->GetEntries();

	// Register input to form flux/event rate hists
	RegisterJointInput(rawinputs, nevents, fluxhist, eventhist);
	SetupJointInputs();

	// Add to TChain
	fFitEventTree->Add( rawinputs.c_str() );

	// Setup NEvents and the FitEvent
	fNEvents = fFitEventTree->GetEntries();
	fEventType = kSPLINEPARAMETER;
	fNUISANCEEvent = new FitEvent();
	fNUISANCEEvent->SetBranchAddress(fFitEventTree);

	// Setup Spline Reader
	LOG(SAM) << "Loading Spline Reader." << std::endl;

	fSplRead = new SplineReader();
	fSplRead->Read( (TTree*)inp_file->Get("spline_reader") );
	fNUISANCEEvent->fSplineRead = this->fSplRead;

	// Setup Matching Spline TTree
	fSplTree = (TTree*)inp_file->Get("spline_tree");
	fSplTree->SetBranchAddress( "SplineCoeff", fSplineCoeff );
	fNUISANCEEvent->fSplineCoeff = this->fSplineCoeff;

	// Load into memory
	for (int j = 0; j < fNEvents; j++) {
		std::vector<float> tempval;
		fFitEventTree->GetEntry(j);
		fStartingWeights.push_back( GetInputWeight(j) );
	}

};

SplineInputHandler::~SplineInputHandler() {
	if (fFitEventTree) delete fFitEventTree;
	if (fSplTree) delete fSplTree;
	if (fSplRead) delete fSplRead;
	fStartingWeights.clear();
}

void SplineInputHandler::CreateCache() {
	if (fCacheSize > 0) {
		fFitEventTree->SetCacheEntryRange(0, fNEvents);
		fFitEventTree->AddBranchToCache("*", 1);
		fFitEventTree->SetCacheSize(fCacheSize);

		fSplTree->SetCacheEntryRange(0, fNEvents);
		fSplTree->AddBranchToCache("*", 1);
		fSplTree->SetCacheSize(fCacheSize);
	}
}

void SplineInputHandler::RemoveCache() {
	fFitEventTree->SetCacheEntryRange(0, fNEvents);
	fFitEventTree->AddBranchToCache("*", 0);
	fFitEventTree->SetCacheSize(0);

	fSplTree->SetCacheEntryRange(0, fNEvents);
	fSplTree->AddBranchToCache("*", 0);
	fSplTree->SetCacheSize(0);
}


FitEvent* SplineInputHandler::GetNuisanceEvent(const UInt_t entry, const bool lightweight) {

	// Make sure events setup
	if (entry >= (UInt_t)fNEvents) return NULL;

	// Reset all variables before tree read
	fNUISANCEEvent->ResetEvent();

	// Read NUISANCE Tree
	if (!lightweight)
		fFitEventTree->GetEntry(entry);

	// Get Spline Coefficients
	fSplTree->GetEntry(entry);
	fNUISANCEEvent->fSplineCoeff = fSplineCoeff;

	// Setup Input scaling for joint inputs
	fNUISANCEEvent->InputWeight = fStartingWeights[entry];

	// Run Initial, FSI, Final, Other ordering.
	fNUISANCEEvent-> OrderStack();

	return fNUISANCEEvent;
}

double SplineInputHandler::GetInputWeight(int entry) {
	double w = InputHandlerBase::GetInputWeight(entry);
	return w * fNUISANCEEvent->SavedRWWeight;
}

void SplineInputHandler::Print() {}


