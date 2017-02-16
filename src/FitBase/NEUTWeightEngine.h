#ifndef WEIGHT_ENGINE_NEUT_H
#define WEIGHT_ENGINE_NEUT_H

#include "FitLogger.h"

#ifdef __NEUT_ENABLED__
#include "NReWeight.h"
#include "NReWeightCasc.h"
#include "NReWeightNuXSecCCQE.h"
#include "NReWeightNuXSecCCRES.h"
#include "NReWeightNuXSecCOH.h"
#include "NReWeightNuXSecDIS.h"
#include "NReWeightNuXSecNC.h"
#include "NReWeightNuXSecNCEL.h"
#include "NReWeightNuXSecNCRES.h"
#include "NReWeightNuXSecRES.h"
#include "NReWeightNuclPiless.h"
#include "NSyst.h"
#include "NSystUncertainty.h"
#include "neutpart.h"
#include "neutvect.h"
#endif

#include "GeneratorUtils.h"
#include "WeightEngineBase.h"
#include "FitWeight.h"

class NEUTWeightEngine : public WeightEngineBase {
	public:
		NEUTWeightEngine(std::string name);
		~NEUTWeightEngine(){};

		void IncludeDial(std::string name, double startval);
		void SetDialValue(int rwenum, double val);
		void Reconfigure(bool silent = false);
		double CalcWeight(BaseFitEvt* evt);

		std::map<std::string, neut::rew::NSyst_t> fNeutNameSysts;
		std::map<int, neut::rew::NSyst_t> fNeutEnumSysts;
		neut::rew::NReWeight* fNeutRW;
};

#endif