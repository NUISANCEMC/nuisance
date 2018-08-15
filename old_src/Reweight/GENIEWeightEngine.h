#ifndef WEIGHT_ENGINE_GENIE_H
#define WEIGHT_ENGINE_GENIE_H

#include "FitLogger.h"


#ifdef __GENIE_ENABLED__
#include "EVGCore/EventRecord.h"
#include "EVGCore/EventRecord.h"
#include "GHEP/GHepRecord.h"
#include "GSyst.h"
#include "GSystUncertainty.h"
#include "Ntuple/NtpMCEventRecord.h"
#include "ReWeight/GReWeight.h"
#include "ReWeight/GReWeightAGKY.h"
#include "ReWeight/GReWeightDISNuclMod.h"
#include "ReWeight/GReWeightFGM.h"
#include "ReWeight/GReWeightFZone.h"
#include "ReWeight/GReWeightINuke.h"
#include "ReWeight/GReWeightNonResonanceBkg.h"
#include "ReWeight/GReWeightNuXSecCCQE.h"
#include "ReWeight/GReWeightNuXSecCCQEvec.h"
#include "ReWeight/GReWeightNuXSecCCRES.h"
#include "ReWeight/GReWeightNuXSecCOH.h"
#include "ReWeight/GReWeightNuXSecDIS.h"
#include "ReWeight/GReWeightNuXSecNC.h"
#include "ReWeight/GReWeightNuXSecNCEL.h"
#include "ReWeight/GReWeightNuXSecNCRES.h"
#include "ReWeight/GReWeightResonanceDecay.h"

#if __GENIE_VERSION__ >= 212
#include "ReWeight/GReWeightNuXSecCCQEaxial.h"
#endif

using namespace genie;
using namespace genie::rew;
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
	std::vector<genie::rew::GSyst_t> fGENIESysts;
	genie::rew::GReWeight* fGenieRW;  //!< Genie RW Object
#endif

};

#endif
