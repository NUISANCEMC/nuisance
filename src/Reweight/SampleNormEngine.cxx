#include "SampleNormEngine.h"

SampleNormEngine::SampleNormEngine(std::string name) {

	// Setup the NEUT Reweight engien
	fCalcName = name;
	LOG(FIT) << "Setting up Sample Norm RW : " << fCalcName << std::endl;

	// Set Abs Twk Config
	fIsAbsTwk = true;

};


void SampleNormEngine::IncludeDial(std::string name, double startval) {

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


void SampleNormEngine::SetDialValue(int nuisenum, double val) {
	fValues[fEnumIndex[nuisenum]] = val;
}

void SampleNormEngine::SetDialValue(std::string name, double val){
	fValues[fNameIndex[name]] = val;
}

void SampleNormEngine::Reconfigure(bool silent) {
	// Empty placeholder incase we want print statements...
}














