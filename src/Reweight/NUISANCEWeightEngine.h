#ifndef WEIGHT_ENGINE_NUISANCE_H
#define WEIGHT_ENGINE_NUISANCE_H

#include "FitLogger.h"
#include "GeneratorUtils.h"
#include "WeightEngineBase.h"
#include "FitWeight.h"
#include "NUISANCESyst.h"
#include "NUISANCEWeightCalcs.h"

class NUISANCEWeightEngine : public WeightEngineBase {
public:
	NUISANCEWeightEngine(std::string name);
	~NUISANCEWeightEngine() {};

	void IncludeDial(std::string name, double startval);

	void SetDialValue(std::string name, double val);
	void SetDialValue(int nuisenum, double val);

	void Reconfigure(bool silent = false);

	double CalcWeight(BaseFitEvt* evt);

	inline bool NeedsEventReWeight() { return true; };

	std::vector<NUISANCEWeightCalc*> fWeightCalculators;
	std::vector<int> fNUISANCEEnums;

};




#endif
