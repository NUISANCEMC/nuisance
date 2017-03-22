#include "SplineInputHandler.h"

SplineInputHandler::SplineInputHandler(std::string const& handle, std::string const& rawinputs) {

	LOG(SAM) << "Creating SplineInputHandler : " << handle << std::endl;

	// Run a joint input handling
	fName = handle;
	jointinput = false;
	jointindexswitch = 0;

	// Setup the TChain
	fFitEventTree = new TChain("nuisance_events");
	fMaxEvents = FitPar::Config().GetParI("MAXEVENTS");

	// Add to TChain
	fFitEventTree->Add( rawinputs.c_str() );

	// Open File for histogram access
	int evtcounter = 0;
	TFile* inp_file = new TFile(rawinputs.c_str(), "READ");

	if (LOG_LEVEL(SAM)) {
		std::cout << "\t\t|-> Input File 0"
		          << "      : " << rawinputs << std::endl;
	}

	// Get Flux/Event hist
	TH1D* fluxhist  = (TH1D*)inp_file->Get(
	                      (PlotUtils::GetObjectWithName(inp_file, "nuisance_fluxhist")).c_str());
	TH1D* eventhist = (TH1D*)inp_file->Get(
	                      (PlotUtils::GetObjectWithName(inp_file, "nuisance_eventhist")).c_str());

	// Get N events
	TTree* neuttree = (TTree*)inp_file->Get("nuisance_events");
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


	// Setup NEvents and the FitEvent
	fNEvents = fFitEventTree->GetEntries();
	fEventType = kNEWSPLINE;
	fNUISANCEEvent = new FitEvent();
	fNUISANCEEvent->SetBranchAddress(fFitEventTree);

	if (LOG_LEVEL(SAM)) {
		std::cout << "\t\t|-> Total Entries    : " << fNEvents << std::endl
		          << "\t\t|-> Event Integral   : " << fEventHist->Integral("width") * 1.E-38 << " events/nucleon" << std::endl
		          << "\t\t|-> Flux Integral    : " << fFluxHist->Integral("width") << " /cm2" << std::endl
		          << "\t\t|-> Event/Flux       : "
		          << fEventHist->Integral("width") * 1.E-38 / fFluxHist->Integral("width") << " cm2/nucleon" <<  std::endl;
	}

	// Load into memory
	for (int j = 0; j < fNEvents; j++) {
		std::vector<float> tempval;

		fFitEventTree->GetEntry(j);
		//fSplTree->GetEntry(j);

		//for (int i = 0; i < fNPar; i++){
		//  tempval.push_back( fSplineCoeff[i] );
		//}
		//fAllSplineCoeff.push_back(tempval);
		//	  if (j % (fNEvents/10) == 0 and j != 0) {
		//  LOG(SAM) << "Pushed " << int(j*100/fNEvents)+1 << "% of spline sets into memory for " << fName
		//	     << " (~" << int(sizeof(float)*tempval.size()*fAllSplineCoeff.size()/1.E6) <<"MB)" << std::endl;
		//}

		fStartingWeights.push_back(fNUISANCEEvent->SavedRWWeight * fNUISANCEEvent->InputWeight);
	}

	fEventType = kSPLINEPARAMETER;

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

	// Setup Spline Reader
	LOG(SAM) << "Loading Spline Reader." << std::endl;

	fSplRead = new SplineReader();
	fSplRead->Read( (TTree*)inp_file->Get("spline_reader") );
	fNUISANCEEvent->fSplineRead = this->fSplRead;

	fNPar = fSplRead->GetNPar();

	// Setup Friend Spline TTree
	fSplTree = (TTree*)inp_file->Get("spline_tree");
	fSplTree->SetBranchAddress( "SplineCoeff", fSplineCoeff );
	fNUISANCEEvent->fSplineCoeff = this->fSplineCoeff;

	fBaseEvent = static_cast<BaseFitEvt*>(fNUISANCEEvent);

};


FitEvent* SplineInputHandler::GetNuisanceEvent(const UInt_t entry) {

	// Make sure events setup
	if (entry >= (UInt_t)fNEvents) return NULL;

	// Reset all variables before tree read
	fNUISANCEEvent->ResetEvent();

	// Read NUISANCE Tree
	fFitEventTree->GetEntry(entry);

	fSplTree->GetEntry(entry);
	fNUISANCEEvent->fSplineCoeff = fSplineCoeff;

	fNUISANCEEvent->eventid = entry;

	// Run Initial, FSI, Final, Other ordering.
	fNUISANCEEvent-> OrderStack();

	// Setup Input scaling for joint inputs
	if (jointinput) {
		fNUISANCEEvent->InputWeight = fStartingWeights[entry] * GetInputWeight(entry);
	} else {
		fNUISANCEEvent->InputWeight = fStartingWeights[entry];
	}

	return fNUISANCEEvent;
}


double SplineInputHandler::GetInputWeight(int entry) {

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


BaseFitEvt* SplineInputHandler::GetBaseEvent(const UInt_t entry) {

	// Make sure events setup
	if (entry >= (UInt_t)fNEvents) return NULL;

	fBaseEvent->eventid = entry;

	// Set joint scaling if required
	if (jointinput) {
		fBaseEvent->InputWeight = fStartingWeights[entry] * GetInputWeight(entry);
	} else {
		fBaseEvent->InputWeight = fStartingWeights[entry];
	}

	return fBaseEvent;
}

void SplineInputHandler::Print() {}


