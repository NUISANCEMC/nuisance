#ifndef WEIGHT_ENGINE_NIWG_H
#define WEIGHT_ENGINE_NIWG_H

#include "NIWGReWeight.h"
#include "NIWGReWeight1piAngle.h"
#include "NIWGReWeight2010a.h"
#include "NIWGReWeight2012a.h"
#include "NIWGReWeight2014a.h"
#include "NIWGReWeightDeltaMass.h"
#include "NIWGReWeightEffectiveRPA.h"

#ifdef NIWGRW_HAVE_LOWQ2
#include "NIWGReWeightEffectiveQELowQ2Suppression.h"
#endif
#ifdef NIWGRW_HAVE_2P2HENU
#include "NIWGReWeight2p2hEdep.h"
#endif
#ifdef NIWGRW_HAVE_RESLOWQ2
#include "NIWGReWeightSPPLowQ2Suppression.h"
#endif

#include "NIWGReWeightHadronMultSwitch.h"
#include "NIWGReWeightMEC.h"
#include "NIWGReWeightPiMult.h"
#include "NIWGReWeightProtonFSIbug.h"
#include "NIWGReWeightRPA.h"
#include "NIWGReWeightSpectralFunc.h"
#include "NIWGReWeightSplineEnu.h"

#include "NIWGSyst.h"
#include "NIWGSystUncertainty.h"

#include "FitLogger.h"

#include "GeneratorUtils.h"
#include "WeightEngineBase.h"
#include "FitWeight.h"

class NIWGWeightEngine : public WeightEngineBase {
public:
	NIWGWeightEngine(std::string name);
	~NIWGWeightEngine() {};

	void IncludeDial(std::string name, double startval);

	void SetDialValue(std::string name, double val);
	void SetDialValue(int nuisenum, double val);

	void Reconfigure(bool silent = false);

	double CalcWeight(BaseFitEvt* evt);

	inline bool NeedsEventReWeight() { return true; };

	std::vector<niwg::rew::NIWGSyst_t> fNIWGSysts;
	niwg::rew::NIWGEvent* GetNIWGEventLocal(NeutVect* nvect);
	niwg::rew::NIWGReWeight* fNIWGRW;
};

#endif
