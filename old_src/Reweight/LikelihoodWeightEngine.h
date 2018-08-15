#ifndef LikelihoodWeightEngine_H
#define LikelihoodWeightEngine_H

#include "FitLogger.h"
#include "GeneratorUtils.h"
#include "WeightEngineBase.h"

// Could probably just make a general weight engine for stuff like this...

class LikelihoodWeightEngine : public WeightEngineBase {
	public:
		LikelihoodWeightEngine(std::string name);
		~LikelihoodWeightEngine(){};

		void IncludeDial(std::string name, double startval);
				void SetDialValue(std::string name, double val);

		void SetDialValue(int rwenum, double val);
		void Reconfigure(bool silent = false);
		inline double CalcWeight(BaseFitEvt* evt) {return 1.0;};
		inline bool NeedsEventReWeight(){ return false; };

		double GetDialValue(std::string name);
};

#endif
