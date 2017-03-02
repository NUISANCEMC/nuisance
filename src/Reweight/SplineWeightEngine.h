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

		void IncludeDial(std::string name, int type, double startval);
		void IncludeDial(int nuisenum, double startval);
		void SetDialValue(int rwenum, double val);
		void Reconfigure(bool silent = false);
		inline double CalcWeight(BaseFitEvt* evt);
		inline bool NeedsEventReWeight(){ return true; };

		std::map<std::string, int> fSplineNameSysts;
		std::map<int, std::string> fSplineSystNames;
		std::map<int, int> fSplineEnumSysts;
		std::map<int, double> fSplineValues;
		std::map< std::string, double > fSplineValueMap;

		double* fDialValues;
};

#endif