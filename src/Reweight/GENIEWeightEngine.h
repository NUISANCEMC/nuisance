#ifndef WEIGHT_ENGINE_GENIE_H
#define WEIGHT_ENGINE_GENIE_H

#ifdef GENIE3_API_ENABLED
#include "RwFramework/GSyst.h"
#include "RwFramework/GReWeight.h"
using namespace genie;
using namespace genie::rew;
#else
#include "ReWeight/GSyst.h"
#include "ReWeight/GReWeight.h"
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

	std::vector<genie::rew::GSyst_t> fGENIESysts;
	genie::rew::GReWeight* fGenieRW;  //!< Genie RW Object
};

#endif
