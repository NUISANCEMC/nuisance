#ifndef WEIGHT_ENGINE_GENIE_H
#define WEIGHT_ENGINE_GENIE_H

#include "FitLogger.h"


#ifdef __GENIE_ENABLED__
#ifdef GENIE_PRE_R3
#include "Algorithm/AlgConfigPool.h"
#include "EVGCore/EventRecord.h"
#include "GHEP/GHepRecord.h"
#include "ReWeight/GSyst.h"
#include "ReWeight/GSystUncertainty.h"
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
#else
#include "Framework/EventGen/EventRecord.h"
#include "Framework/GHEP/GHepRecord.h"
#include "Framework/GHEP/GHepParticle.h"
#include "Framework/Ntuple/NtpMCEventRecord.h"
#include "Framework/Utils/RunOpt.h"
#include "Framework/Utils/AppInit.h"
#include "RwFramework/GSyst.h"
#include "RwFramework/GSystUncertainty.h"
#include "RwFramework/GReWeight.h"
#include "RwCalculators/GReWeightAGKY.h"
#include "RwCalculators/GReWeightDISNuclMod.h"
#include "RwCalculators/GReWeightFGM.h"
#include "RwCalculators/GReWeightFZone.h"
#include "RwCalculators/GReWeightINuke.h"
#include "RwCalculators/GReWeightNonResonanceBkg.h"
#include "RwCalculators/GReWeightNuXSecCCQE.h"
#include "RwCalculators/GReWeightNuXSecCCQEvec.h"
#include "RwCalculators/GReWeightNuXSecCCRES.h"
#include "RwCalculators/GReWeightNuXSecCOH.h"
#include "RwCalculators/GReWeightNuXSecDIS.h"
#include "RwCalculators/GReWeightNuXSecNC.h"
#include "RwCalculators/GReWeightNuXSecNCEL.h"
#include "RwCalculators/GReWeightNuXSecNCRES.h"
#include "RwCalculators/GReWeightResonanceDecay.h"
#include "RwCalculators/GReWeightNuXSecCCQEaxial.h"
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
#ifndef __NO_GENIE_REWEIGHT__
	std::vector<genie::rew::GSyst_t> fGENIESysts;
	genie::rew::GReWeight* fGenieRW;  //!< Genie RW Object
#endif
#endif

};

#endif
