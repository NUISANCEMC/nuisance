#ifndef WEIGHT_ENGINE_NUWRO_H
#define WEIGHT_ENGINE_NUWRO_H

#include "FitLogger.h"
#include "GeneratorUtils.h"
#include "WeightEngineBase.h"
#include "WeightUtils.h"

class NuWroWeightEngine : public WeightEngineBase {
	public:
		NuWroWeightEngine(std::string name);
		~NuWroWeightEngine(){};

		void IncludeDial(int nuisenum, double startval);
		void SetDialValue(int rwenum, double val);
		void Reconfigure(bool silent = false);
		double CalcWeight(BaseFitEvt* evt);
		inline bool NeedsEventReWeight(){ return true; };

		std::map<std::string, nuwro::rew::NuwroSyst_t> fNuwroNameSysts;
		std::map<int, nuwro::rew::NuwroSyst_t> fNuwroEnumSysts;
		nuwro::rew::NuwroReWeight* fNuwroRW;
};

#endif