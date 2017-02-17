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

		void IncludeDial(int nuisenum, double startval);
		void SetDialValue(int rwenum, double val);
		void Reconfigure(bool silent = false);
		inline double CalcWeight(BaseFitEvt* evt) {return 1.0;};
		inline bool NeedsEventReWeight(){ return false; };

		std::map<std::string, int> fLikeWeightNameSysts;
		std::map<int, int> fLikeWeightEnumSysts;
		std::map<int, double> fLikeWeightValues;
};

#endif