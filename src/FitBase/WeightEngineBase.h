#ifndef WEIGHTENGINE_BASE_H
#define WEIGHTENGINE_BASE_H

#include "BaseFitEvt.h"
#include "FitLogger.h"
#include "FitUtils.h"

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <deque>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include "GeneratorUtils.h"
#include "TCanvas.h"
#include "TGraph2D.h"

class WeightEngineBase {

public:

	WeightEngineBase(){};
	// ~WeightEngineBase(){};

	// Functions requiring Override
	void IncludeDial(std::string name, int type, double startval);
	void SetDialValue(int rwenum, double val);
	void Reconfigure(bool silent);
	double CalcWeight(BaseFitEvt* evt);
	double GetDialValue(int rwenum);
	

	bool fHasChanged;
	std::map<int, double> fEnumCurValues;
	std::string fName;
	bool fIsAbsTwk;
};


#endif
