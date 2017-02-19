#ifndef SplineWeightEngine_H
#define SplineWeightEngine_H

#include "FitLogger.h"
#include "GeneratorUtils.h"
#include "WeightEngineBase.h"

class SplineWeightEngine : public WeightEngineBase {
	public:
		SplineWeightEngine(std::string name);
		~SplineWeightEngine(){};

		void IncludeDial(int nuisenum, double startval);
		void SetDialValue(int rwenum, double val);
		void Reconfigure(bool silent = false);
		inline double CalcWeight(BaseFitEvt* evt) {return 1.0;};
		inline bool NeedsEventReWeight(){ return true; };

		std::map<std::string, int> fNormNameSysts;
		std::map<int, int> fNormEnumSysts;
		std::map<int, double> fNormValues;
};

#endif