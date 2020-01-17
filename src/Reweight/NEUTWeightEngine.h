#ifndef WEIGHT_ENGINE_NEUT_H
#define WEIGHT_ENGINE_NEUT_H

#include "FitLogger.h"

#ifdef __NEUT_ENABLED__
#ifndef __NO_REWEIGHT__
#include "NEUTInputHandler.h"
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
#if __NEUT_VERSION__ >= 541
#include "CommonBlockIFace.h"
#endif
#include "NSyst.h"
#include "NSystUncertainty.h"
#include "neutpart.h"
#include "neutvect.h"
#endif
#endif

#include "FitWeight.h"
#include "GeneratorUtils.h"
#include "WeightEngineBase.h"

class NEUTWeightEngine : public WeightEngineBase {
public:
  NEUTWeightEngine(std::string name);
  ~NEUTWeightEngine(){};

  void IncludeDial(std::string name, double startval);

  void SetDialValue(std::string name, double val);
  void SetDialValue(int nuisenum, double val);

  void Reconfigure(bool silent = false);

  double CalcWeight(BaseFitEvt *evt);

  inline bool NeedsEventReWeight() { return true; };

#ifdef __NEUT_ENABLED__
#ifndef __NO_REWEIGHT__
  std::vector<neut::rew::NSyst_t> fNEUTSysts;
  neut::rew::NReWeight *fNeutRW;
#endif
#endif
};

#endif
