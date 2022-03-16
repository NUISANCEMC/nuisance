#ifndef WEIGHT_ENGINE_NUWRO_H
#define WEIGHT_ENGINE_NUWRO_H

#include "FitLogger.h"
#include "GeneratorUtils.h"
#include "WeightEngineBase.h"
#include "WeightUtils.h"

#include "NuwroReWeight.h"
#include "event1.h"
#include "NuwroReWeight_FlagNorm.h"
#include "NuwroReWeight_QEL.h"
#include "NuwroReWeight_SPP.h"
#include "NuwroSyst.h"
#include "NuwroSystUncertainty.h"

class NuWroWeightEngine : public WeightEngineBase {
public:
	NuWroWeightEngine(std::string name);
	~NuWroWeightEngine() {};

	void IncludeDial(std::string name, double startval);

	void SetDialValue(std::string name, double val);
	void SetDialValue(int rwenum, double val);

	void Reconfigure(bool silent = false);
	double CalcWeight(BaseFitEvt* evt);

	inline bool NeedsEventReWeight() { return true; };

	std::vector<nuwro::rew::NuwroSyst_t> fNUWROSysts;
	nuwro::rew::NuwroReWeight* fNuwroRW;
};

#endif
