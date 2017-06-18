#ifndef SplineWeightEngine_H
#define SplineWeightEngine_H

#include "FitLogger.h"
#include "GeneratorUtils.h"
#include "WeightEngineBase.h"
#include "SplineReader.h"

class SplineWeightEngine : public WeightEngineBase {
	public:
		SplineWeightEngine(std::string name);
		~SplineWeightEngine(){};

		void IncludeDial(std::string name,  double startval);
		void SetDialValue(std::string name, double val);
		void SetDialValue(int rwenum, double val);
		void Reconfigure(bool silent = false);
		inline double CalcWeight(BaseFitEvt* evt);
		inline bool NeedsEventReWeight(){ return true; };

		std::map< std::string, double > fSplineValueMap;
		std::vector<int> fSingleEnums;
		std::vector<std::string> fSingleNames;
};

#endif
