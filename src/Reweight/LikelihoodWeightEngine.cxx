#include "LikelihoodWeightEngine.h"

LikelihoodWeightEngine::LikelihoodWeightEngine(std::string name) {

	// Setup the NEUT Reweight engien
	fCalcName = name;
	LOG(FIT) << "Setting up Likelihood Weight RW : " << fCalcName << std::endl;

	// Set Abs Twk Config
	fIsAbsTwk = true;

};


void LikelihoodWeightEngine::IncludeDial(std::string name, double startval) {

	// Get NUISANCE Enum
	int nuisenum = Reweight::ConvDial(name, kNORM);

	// Fill Maps
	int index = fValues.size();
	fValues.push_back(1.0);

	fEnumIndex[nuisenum] = index;
	fNameIndex[name] = index;

	// Set Value if given
	if (startval != -999.9) {
		SetDialValue(name, startval);
	}
};


void LikelihoodWeightEngine::SetDialValue(int nuisenum, double val) {
	fValues[fEnumIndex[nuisenum]] = val;
}

void LikelihoodWeightEngine::SetDialValue(std::string name, double val){
	fValues[fNameIndex[name]] = val;
}

void LikelihoodWeightEngine::Reconfigure(bool silent) {
	// Empty placeholder incase we want print statements...
}














