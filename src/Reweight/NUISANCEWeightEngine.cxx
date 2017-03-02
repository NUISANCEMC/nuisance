#include "NUISANCEWeightEngine.h"


// NUISANCE Weight Calc Base
double NUISANCEWeightCalc::CalcWeight(BaseFitEvt* evt){
	return 1.0; // DEFAULT
}
	
void NUISANCEWeightCalc::SetDialValue(int rwenum, double val){

	// Check found
	if (fDialSystIndices.find(rwenum) == fDialSystIndices.end()){
		"Trying to set unknown dial value!"
	}




			
}

bool NUISANCEWeightCalc::IsHandled(int rwenum){
	return false;
}

void NUISANCEWeightCalc::Print(){

	// Loop over enum names
	for (std::map<int, std::string>::iterator iter = fNUISWEIGHTCalcs.begin();
		iter != fNUISWEIGHTCalcs.end(); iter++){
		std::cout << "NUISANCE Weight Calc " << iter->second << " " << fDialValues->at(iter->first) << std::endl;
	}

}








// Main NUISANCE Weight Engine
NUISANCEWeightEngine::NUISANCEWeightEngine(std::string name) {

	// Setup the NEUT Reweight engien
	fName = name;
	LOG(FIT) << "Setting up NUISANCE RW : " << fName << endl;

};


void NUISANCEWeightEngine::IncludeDial(int nuisenum, double startval){

	// Get RW Enum and name
	int rwenum = (nuisenum % 1000);
	
	// Check RW Enum not handled by any of the 

};


void NUISANCEWeightEngine::SetDialValue(int nuisenum, double val){
	// Set RW engine values
	int rwenum = (nuisenum % 1000);
	fNeutRW->Systematics().Set(static_cast<neut::rew::NSyst_t>(rwenum), val);
}


void NUISANCEWeightEngine::Reconfigure(bool silent){
	// Hush now...
}


double NUISANCEWeightEngine::CalcWeight(BaseFitEvt* evt){
	double rw_weight = 1.0;

    // Return rw_weight
    return rw_weight;
}










