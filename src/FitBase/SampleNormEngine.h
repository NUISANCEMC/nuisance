#ifndef SampleNormEngine_H
#define SampleNormEngine_H

#include "FitLogger.h"
#include "GeneratorUtils.h"
#include "WeightEngineBase.h"

class SampleNormEngine : public WeightEngineBase {
	public:
		SampleNormEngine(std::string name);
		~SampleNormEngine(){};

		void IncludeDial(int nuisenum, double startval);
		void SetDialValue(int rwenum, double val);
		void Reconfigure(bool silent = false);
		inline double CalcWeight(BaseFitEvt* evt) {return 1.0;};
		inline bool NeedsEventReWeight(){ return false; };

		std::map<std::string, int> fNormNameSysts;
		std::map<int, int> fNormEnumSysts;
		std::map<int, double> fNormValues;
};

#endif