#include "NUANCEInputHandler.h"

NUANCEInputHandler::NUANCEInputHandler(std::string const& handle, std::string const& rawinputs){

	// Run a joint input handling
	fName = handle;
	
	ERR(FTL) << "NUANCE DOESNT WORK! " << std::endl;

};


FitEvent* NUANCEInputHandler::GetNuisanceEvent(const UInt_t entry){

	// Make sure events setup
	// if (!fNUISANCEEvent) fNUISANCEEvent = new FitEvent(fNeutVect);

	// Read Entry from TTree to fill NEUT Vect in BaseFitEvt;
	fNUANCETree->GetEntry(entry);

	// Setup Input scaling for joint inputs
	if (jointinput){
		fNUISANCEEvent->InputWeight = GetInputWeight(entry);
	} else {
		fNUISANCEEvent->InputWeight = 1.0;
	}

	// Run NUISANCE Vector Filler
	CalcNUISANCEKinematics();

	return fNUISANCEEvent;
}


void NUANCEInputHandler::CalcNUISANCEKinematics(){

	// Reset all variables
	fNUISANCEEvent->ResetEvent();

	
	// Run Initial, FSI, Final, Other ordering. 
	fNUISANCEEvent-> OrderStack();
	return;
}


double NUANCEInputHandler::GetInputWeight(int entry){

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


BaseFitEvt* NUANCEInputHandler::GetBaseEvent(const UInt_t entry){

	// Make sure events setup
	// if (!fBaseEvent) fBaseEvent = new BaseFitEvt(fNeutVect);

	// Read entry from TTree to fill NEUT Vect in BaseFitEvt;
	fNUANCETree->GetEntry(entry);

	// Set joint scaling if required
	if (jointinput){
		fBaseEvent->InputWeight = GetInputWeight(entry);
	} else {
		fBaseEvent->InputWeight = 1.0;
	}

	return fBaseEvent;
}

void NUANCEInputHandler::Print(){}

	
