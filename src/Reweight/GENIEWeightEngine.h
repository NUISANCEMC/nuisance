#ifndef WEIGHT_ENGINE_GENIE_H
#define WEIGHT_ENGINE_GENIE_H

#ifdef __GENIE_ENABLED__
#ifdef GENIE_PRE_R3
#ifndef __NO_GENIE_REWEIGHT__
#include "ReWeight/GSyst.h"
#include "ReWeight/GReWeight.h"
#endif
#else
#ifndef __NO_GENIE_REWEIGHT__
#include "RwFramework/GSyst.h"
#include "RwFramework/GReWeight.h"
using namespace genie;
using namespace genie::rew;
#endif
#endif
#endif

#include "GeneratorUtils.h"
#include "WeightEngineBase.h"
#include "FitWeight.h"

class GENIEWeightEngine : public WeightEngineBase {
public:
	GENIEWeightEngine(std::string name);
	~GENIEWeightEngine() {};

	void IncludeDial(std::string name, double startval);
	void SetDialValue(int rwenum, double val);
	void SetDialValue(std::string name, double val);

	void Reconfigure(bool silent = false);
	double CalcWeight(BaseFitEvt* evt);
	inline bool NeedsEventReWeight() { return true; };

#ifdef __GENIE_ENABLED__
#ifndef __NO_GENIE_REWEIGHT__
	std::vector<genie::rew::GSyst_t> fGENIESysts;
	genie::rew::GReWeight* fGenieRW;  //!< Genie RW Object
#endif
#endif

};

#endif
