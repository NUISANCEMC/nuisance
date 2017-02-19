#include "SplineWeightEngine.h"

SplineWeightEngine::SplineWeightEngine(std::string name) {

	// Setup the NEUT Reweight engien
	fName = name;
	LOG(FIT) << "Setting up Sample Norm RW : " << fName << endl;

	// Set Abs Twk Config
	fIsAbsTwk = true;

};


void SplineWeightEngine::IncludeDial(int nuisenum, double startval) {

	// Get RW Enum and name
	int rwenum = (nuisenum % 1000);

	// Fill Maps
	fNormEnumSysts[nuisenum] = rwenum;

	// Initialise dial
	fNormValues[rwenum] = 1.0;

	// Set Value if given
	if (startval != -999.9) {
		SetDialValue(nuisenum, startval);
	}

};


void SplineWeightEngine::SetDialValue(int nuisenum, double val) {
	// Set RW engine values
	int rwenum = (nuisenum % 1000);
	fNormValues[rwenum] = val;
}


void SplineWeightEngine::Reconfigure(bool silent) {
	// Empty placeholder incase we want print statements...
}











