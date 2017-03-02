#include "SplineWeightEngine.h"

SplineWeightEngine::SplineWeightEngine(std::string name) {

	// Setup the NEUT Reweight engien
	fName = name;
	LOG(FIT) << "Setting up Spline RW : " << fName << endl;
	sleep(2);
	// Set Abs Twk Config
	fIsAbsTwk = true;

};


void SplineWeightEngine::IncludeDial(std::string name, int type, double startval) {
	int nuisenum = FitBase::GetDialEnum(type, name);
	int rwenum = (nuisenum % 1000);

	fSplineNameSysts[name] = rwenum;
	fSplineSystNames[rwenum] = name;

	IncludeDial(rwenum, startval);
	std::cout << "Included Spline Dial " << name << " " << startval << std::endl;
	sleep(1);
}

void SplineWeightEngine::IncludeDial(int nuisenum, double startval) {

	// Get RW Enum and name
	int rwenum = (nuisenum % 1000);

	// Fill Maps
	fSplineEnumSysts[nuisenum] = rwenum;

	// Initialise dial
	fSplineValues[rwenum] = 1.0;

	// Set Value if given
	if (startval != -999.9) {
		SetDialValue(nuisenum, startval);
	}

};


void SplineWeightEngine::SetDialValue(int nuisenum, double val) {
	// Set RW engine values
	int rwenum = (nuisenum % 1000);
	fSplineValues[rwenum] = val;
	std::cout << "Setting Dial Value " << nuisenum << " to " << val << std::endl;
	// sleep(10);
}


void SplineWeightEngine::Reconfigure(bool silent) {
	// Reconfigure here does nothing as spline readers in events need to be
	// reconfigured.
	for (std::map<std::string, int>::iterator iter = fSplineNameSysts.begin();
	        iter != fSplineNameSysts.end(); iter++) {

		std::cout << "Saved into spline value map " << iter->first << " " << fSplineValues[ iter->second ] <<" " << iter->second << std::endl;
		fSplineValueMap[ iter->first ] = fSplineValues[ iter->second ];
	}
	// sleep(5);
}


double SplineWeightEngine::CalcWeight(BaseFitEvt* evt) {

	// return 1.0;
	if (!evt->fSplineRead) return 1.0;

	// if (!evt->fSplineRead->NeedsReconfigure()) {
	evt->fSplineRead->Reconfigure(fSplineValueMap);
	// return 1.0;
	// }
	double rw_weight = evt->fSplineRead->CalcWeight( evt->fSplineCoeff );
	
	// std::cout << "Returning Weight = " << rw_weight << std::endl;
	return rw_weight;
	
}










