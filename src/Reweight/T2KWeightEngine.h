#ifndef WEIGHT_ENGINE_T2K_H
#define WEIGHT_ENGINE_T2K_H

#include "FitLogger.h"

#ifdef __T2KREW_ENABLED__
#include "T2KGenieReWeight.h"
#include "T2KNIWGReWeight.h"
#include "T2KNIWGUtils.h"
#include "T2KNeutReWeight.h"
#include "T2KNeutUtils.h"
#include "T2KReWeight.h"
using namespace t2krew;
#endif

#include "GeneratorUtils.h"
#include "WeightEngineBase.h"
#include "FitWeight.h"

class T2KWeightEngine : public WeightEngineBase {
public:
	T2KWeightEngine(std::string name);
	~T2KWeightEngine() {};

	void IncludeDial(std::string name, double startval);

	void SetDialValue(std::string name, double val);
	void SetDialValue(int nuisenum, double val);

	void Reconfigure(bool silent = false);

	double CalcWeight(BaseFitEvt* evt);

	inline bool NeedsEventReWeight() { return true; };
	
#ifdef __T2KREW_ENABLED__
	std::vector<t2krew::T2KSyst_t> fT2KSysts;
	t2krew::T2KReWeight* fT2KRW;  //!< T2K RW Object
	t2krew::T2KNeutReWeight* fT2KNeutRW;
	t2krew::T2KNIWGReWeight* fT2KNIWGRW;
#endif
};

#endif