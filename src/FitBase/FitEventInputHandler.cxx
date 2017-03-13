#include "FitEventInputHandler.h"

FitEventInputHandler::FitEventInputHandler(std::string const& handle, std::string const& rawinputs){

	// Run a joint input handling
	fName = handle;
	jointinput = false;
	jointindexswitch = 0;

	// Form list of all inputs, remove brackets if required.
	std::vector<std::string> inputs = GeneralUtils::ParseToStr(rawinputs, ",");
	if (inputs.front()[0] == '('){
		inputs.front() = inputs.front().substr(1);
	} 
	if (inputs.back()[inputs.back().size()-1] == ')'){
		inputs.back() = inputs.back().substr(0, inputs.back().size() - 1);
	}
    for (size_t inp_it = 0; inp_it < inputs.size(); ++inp_it) {
      LOG(SAM) << "\t -> Found input file: " << inputs[inp_it] << std::endl;
    }

    // Setup the TChain
	fFitEventTree = new TChain("nuisance_events");
	
    // Loop over all inputs and grab flux, eventhist, and nevents
    // Also add it to the TChain
    int evtcounter = 0;
    if (inputs.size() > 1) jointinput = true;
    for (size_t inp_it = 0; inp_it < inputs.size(); ++inp_it) {

    	// Add to TChain
    	fFitEventTree->Add( inputs[inp_it].c_str() );

    	// Open File for histogram access
    	TFile* inp_file = new TFile(inputs[inp_it].c_str(),"READ");

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

    	// Remove file
    	//inp_file->Close();
    	//delete inp_file;
	}

	// Setup NEvents and the FitEvent
    fNEvents = fFitEventTree->GetEntries();
    fEventType = kINPUTFITEVENT;
    fNUISANCEEvent = new FitEvent(kINPUTFITEVENT);
    fNUISANCEEvent->HardReset();
    fNUISANCEEvent->SetBranchAddress(fFitEventTree);

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

};


FitEvent* FitEventInputHandler::GetNuisanceEvent(const UInt_t entry){

	// Return NULL if out of bounds
  if (entry >= (UInt_t)fNEvents) return NULL;

	// Reset all variables before tree read
	fNUISANCEEvent->ResetEvent();

	// Read NUISANCE Tree
	fFitEventTree->GetEntry(entry);
	fNUISANCEEvent->eventid = entry;

	// Run Initial, FSI, Final, Other ordering. 
	// fNUISANCEEvent-> OrderStack();

	// Setup Input scaling for joint inputs
	if (jointinput){
		fNUISANCEEvent->InputWeight = fNUISANCEEvent->SavedRWWeight * fNUISANCEEvent->InputWeight * GetInputWeight(entry);
	} else {
		fNUISANCEEvent->InputWeight = fNUISANCEEvent->SavedRWWeight * fNUISANCEEvent->InputWeight;
	}

	return fNUISANCEEvent;
}


double FitEventInputHandler::GetInputWeight(int entry){

	// Find Switch Scale
	while( entry < jointindexlow[jointindexswitch] ||
	       entry >= jointindexhigh[jointindexswitch] ){
		jointindexswitch++;

		// Loop Around
		if (jointindexswitch == jointindexlow.size()){ 
			jointindexswitch = 0;
		}
	}
	return jointindexscale[jointindexswitch];
};


BaseFitEvt* FitEventInputHandler::GetBaseEvent(const UInt_t entry){

	// Read entry from TTree to fill NEUT Vect in BaseFitEvt;
	fFitEventTree->GetEntry(entry);
	fBaseEvent->eventid = entry;
	
	// Set joint scaling if required
	if (jointinput){
		fNUISANCEEvent->InputWeight = fNUISANCEEvent->SavedRWWeight * fNUISANCEEvent->InputWeight * GetInputWeight(entry);
	} else {
		fNUISANCEEvent->InputWeight = fNUISANCEEvent->SavedRWWeight * fNUISANCEEvent->InputWeight;
	}

	return fBaseEvent;
}

void FitEventInputHandler::Print(){}

	
