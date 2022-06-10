#pragma once

#include "WeightEngineBase.h"

#ifdef NEUTReWeight_LEGACY_API_ENABLED
#include "NReWeight.h"
#else
namespace neut {
namespace rew {
class NReWeight;
}
} // namespace neut
#endif

#include <memory>
#include <vector>

class NEUTWeightEngine : public WeightEngineBase {
public:
  NEUTWeightEngine(std::string name);
  ~NEUTWeightEngine();

  void IncludeDial(std::string name, double startval);

  void SetDialValue(std::string name, double val);
  void SetDialValue(int nuisenum, double val);

  void Reconfigure(bool silent = false);

  double CalcWeight(BaseFitEvt *evt);

  inline bool NeedsEventReWeight() { return true; };

#ifdef NEUTReWeight_LEGACY_API_ENABLED
  std::vector<neut::rew::NSyst_t> fNEUTSysts;
#endif
  std::unique_ptr<neut::rew::NReWeight> fNeutRW;
};
