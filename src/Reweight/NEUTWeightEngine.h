#ifndef WEIGHT_ENGINE_NEUT_H
#define WEIGHT_ENGINE_NEUT_H

#include "FitLogger.h"

#include "NReWeight.h"

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

  std::vector<neut::rew::NSyst_t> fNEUTSysts;
  neut::rew::NReWeight *fNeutRW;
};

#endif
