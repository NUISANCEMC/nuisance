#ifndef WEIGHT_ENGINE_NIWG_H
#define WEIGHT_ENGINE_NIWG_H

#ifdef __NIWG_ENABLED__
#ifdef __NEUT_ENABLED__
#include "NIWGReWeight.h"
#include "NIWGReWeight1piAngle.h"
#include "NIWGReWeight2010a.h"
#include "NIWGReWeight2012a.h"
#include "NIWGReWeight2014a.h"
#include "NIWGReWeightDeltaMass.h"
#include "NIWGReWeightEffectiveRPA.h"
#ifdef HAVE_NIWGRW_LOWQ2
#include "NIWGReWeightEffectiveQELowQ2Suppression.h"
#endif
#ifdef HAVE_NIWGRW_2P2HENU
#include "NIWGReWeight2p2hEdep.h"
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

#include "NReWeight.h"

#include "NReWeightNuXSecCCQE.h"
#include "NReWeightNuXSecRES.h"
// Dials removed in NEUT 5.4.1
#if __NEUT_VERSION__ < 541
#include "NReWeightCasc.h"
#include "NReWeightNuclPiless.h"
#include "NReWeightNuXSecNCRES.h"
#include "NReWeightNuXSecCCRES.h"
#include "NReWeightNuXSecNC.h"
#include "NReWeightNuXSecCOH.h"
#include "NReWeightNuXSecNCEL.h"
#include "NReWeightNuXSecDIS.h"
#endif
#include "NSyst.h"
#include "NSystUncertainty.h"

#include "neutpart.h"
#include "neutvect.h"
#include "NEUTInputHandler.h"
#endif
#endif

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

#ifdef __NIWG_ENABLED__
#ifdef __NEUT_ENABLED__
	std::vector<niwg::rew::NIWGSyst_t> fNIWGSysts;
	niwg::rew::NIWGEvent* GetNIWGEventLocal(NeutVect* nvect);
	niwg::rew::NIWGReWeight* fNIWGRW;
#endif
#endif
};

#endif
