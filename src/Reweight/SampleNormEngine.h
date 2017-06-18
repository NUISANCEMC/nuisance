#ifndef SampleNormEngine_H
#define SampleNormEngine_H

#include "FitLogger.h"
#include "GeneratorUtils.h"
#include "WeightEngineBase.h"

class SampleNormEngine : public WeightEngineBase {
	public:
		SampleNormEngine(std::string name);
		~SampleNormEngine(){};

		void IncludeDial(std::string name, double startval);
		void SetDialValue(int rwenum, double val);
		void SetDialValue(std::string name, double val);
		
		void Reconfigure(bool silent = false);
		inline double CalcWeight(BaseFitEvt* evt) {return 1.0;};
		inline bool NeedsEventReWeight(){ return false; };

		double GetDialValue(std::string name);
};

#endif
