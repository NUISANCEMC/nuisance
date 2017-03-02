#ifndef WEIGHT_ENGINE_NUISANCE_H
#define WEIGHT_ENGINE_NUISANCE_H

#include "FitLogger.h"
#include "GeneratorUtils.h"
#include "WeightEngineBase.h"
#include "FitWeight.h"

enum NUISANCESyst {
	kGaussianCorr_CCQE = 0,
	kGaussianCorr_MEC
}

class NUISANCEWeightCalc {
public:
	NUISANCEWeightCalc() {};
	~NUISANCEWeightCalc() {};

	double CalcWeight(BaseFitEvt* evt);
	void SetDialValue(int rwenum, double val);
	bool IsHandled(int rwenum);

	void Print();

	std::map<int, std::string> fDialNameIndices;
	std::vector<double> fDialValues;
	std::string fName;
}

class GaussianModeCorrection {
public:
	GaussianModeCorrection(std::string name){fName = name;};
	~GaussianModeCorrection(){};

	double CalcWeight(BaseFitEvt* evt);
	bool IsHandled(std::string name);
	bool IsHandled(int rwenum);

}



class NUISANCEWeightEngine : public WeightEngineBase {
public:
	NUISANCEWeightEngine(std::string name);
	~NUISANCEWeightEngine() {};

	void IncludeDial(int nuisenum, double startval);
	void SetDialValue(int rwenum, double val);
	void Reconfigure(bool silent = false);
	double CalcWeight(BaseFitEvt* evt);
	inline bool NeedsEventReWeight() { return true; };

	std::map<std::string, int> fNUISWEIGHTNameSysts;
	std::map<int, NUISANCEWeightCalc*> fNUISWEIGHTCalcs;
};




#endif