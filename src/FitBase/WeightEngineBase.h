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
	virtual void IncludeDial(int nuisenum, double startval) = 0;
	virtual void SetDialValue(int rwenum, double val) = 0;
	virtual void Reconfigure(bool silent) = 0;
	virtual double CalcWeight(BaseFitEvt* evt) = 0;
	virtual double GetDialValue(int rwenum);
	virtual bool NeedsEventReWeight() = 0;

	bool fHasChanged;
	std::map<int, double> fEnumCurValues;
	std::string fName;
	bool fIsAbsTwk;
};


#endif
