#include "LikelihoodWeightEngine.h"

LikelihoodWeightEngine::LikelihoodWeightEngine(std::string name) {

	// Setup the NEUT Reweight engien
	fName = name;
	LOG(FIT) << "Setting up Likelihood Weight RW : " << fName << endl;

	// Set Abs Twk Config
	fIsAbsTwk = true;

};


void LikelihoodWeightEngine::IncludeDial(int nuisenum, double startval) {

	// Get RW Enum and name
	int rwenum = (nuisenum % 1000);

	// Fill Maps
	fLikeWeightEnumSysts[nuisenum] = rwenum;

	// Initialise dial
	fLikeWeightValues[rwenum] = 1.0;

	// Set Value if given
	if (startval != -999.9) {
		SetDialValue(nuisenum, startval);
	}

};


void LikelihoodWeightEngine::SetDialValue(int nuisenum, double val) {
	// Set RW engine values
	int rwenum = (nuisenum % 1000);
	fLikeWeightValues[rwenum] = val;
}


void LikelihoodWeightEngine::Reconfigure(bool silent) {
	// Empty placeholder incase we want print statements...
}











