#include "NUISANCEWeightEngine.h"
#include "NUISANCEWeightCalcs.h"

NUISANCEWeightEngine::NUISANCEWeightEngine(std::string name) {

	// Setup the NUISANCE Reweight engine
	fCalcName = name;
	LOG(FIT) << "Setting up NUISANCE Custom RW : " << fCalcName << std::endl;

	// Load in all Weight Calculations
	fWeightCalculators.push_back( new GaussianModeCorr() );

	// Set Abs Twk Config
	fIsAbsTwk = true;

};


void NUISANCEWeightEngine::IncludeDial(std::string name, double startval) {

	// Get NUISANCE Enum
	int nuisenum = Reweight::ConvDial(name, kCUSTOM);

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


void NUISANCEWeightEngine::SetDialValue(int nuisenum, double val) {
	fValues[fEnumIndex[nuisenum]] = val;
}

void NUISANCEWeightEngine::SetDialValue(std::string name, double val) {
	fValues[fNameIndex[name]] = val;
}

void NUISANCEWeightEngine::Reconfigure(bool silent) {

	// Loop over all names
	for (std::map<int, size_t>::iterator enumiter = fEnumIndex.begin();
	        enumiter != fEnumIndex.end(); enumiter++) {

		for (std::vector<NUISANCEWeightCalc*>::iterator calciter = fWeightCalculators.begin();
		        calciter != fWeightCalculators.end(); calciter++) {

			NUISANCEWeightCalc* nuiscalc = static_cast<NUISANCEWeightCalc*>(*calciter);
			if (nuiscalc->IsHandled(enumiter->first)) {
				nuiscalc->SetDialValue(enumiter->first, fValues[enumiter->second]);
			}
		}
	}
}



double NUISANCEWeightEngine::CalcWeight(BaseFitEvt* evt) {
	double rw_weight = 1.0;

	// Cast as usable class
	for (std::vector<NUISANCEWeightCalc*>::iterator iter = fWeightCalculators.begin();
	        iter != fWeightCalculators.end(); iter++) {
		NUISANCEWeightCalc* nuiscalc = static_cast<NUISANCEWeightCalc*>(*iter);

		rw_weight *= nuiscalc->CalcWeight(evt);
	}

	// Return rw_weight
	return rw_weight;
}




























