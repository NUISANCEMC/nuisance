#ifndef WEIGHT_ENGINE_T2K_H
#define WEIGHT_ENGINE_T2K_H

#include "FitLogger.h"

#ifdef T2KReWeight_LEGACY_API_ENABLED
#include "T2KNIWGReWeight.h"
#include "T2KNeutReWeight.h"
#include "T2KReWeight.h"
#else
#include "T2KReWeight/WeightEngines/T2KReWeightFactory.h"
#endif

#include "FitWeight.h"
#include "GeneratorUtils.h"
#include "WeightEngineBase.h"

class T2KWeightEngine : public WeightEngineBase {
public:
  T2KWeightEngine(std::string name);
  ~T2KWeightEngine(){};

  void IncludeDial(std::string name, double startval);

  void SetDialValue(std::string name, double val);
  void SetDialValue(int nuisenum, double val);

  void Reconfigure(bool silent = false);

  double CalcWeight(BaseFitEvt *evt);

  inline bool NeedsEventReWeight() { return true; };

#ifdef T2KReWeight_LEGACY_API_ENABLED
  std::vector<t2krew::T2KSyst_t> fT2KSysts;
  t2krew::T2KReWeight *fT2KRW; //!< T2K RW Object
  t2krew::T2KNeutReWeight *fT2KNeutRW;
  t2krew::T2KNIWGReWeight *fT2KNIWGRW;
#else
  std::vector<int> fT2KSysts;
  std::unique_ptr<t2krew::T2KReWeight> fT2KRW;
#endif
};

#endif
