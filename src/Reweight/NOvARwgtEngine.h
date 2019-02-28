#pragma once

#include "WeightEngineBase.h"

namespace novarwgt {
class IWeightGenerator;
class Tune;
}

class NOvARwgtEngine : public WeightEngineBase {
public:
  NOvARwgtEngine(){};
  virtual ~NOvARwgtEngine(){};

  static size_t GetWeightGeneratorIndex(std::string const &);

  // Functions requiring Override
  void IncludeDial(std::string name, double startval);

  void SetDialValue(int nuisenum, double val);
  void SetDialValue(std::string name, double val);

  bool IsDialIncluded(std::string name);
  bool IsDialIncluded(int nuisenum);

  double GetDialValue(std::string name);
  double GetDialValue(int nuisenum);

  void Reconfigure(bool silent){};

  double CalcWeight(BaseFitEvt *evt);
  bool NeedsEventReWeight() { return true; }

  std::map<size_t, size_t> fWeightEngineEnums;
  std::vector<novarwgt::IWeightGenerator *> fWeightEngines;
  std::vector<double> fWeightEngineValues;

  std::map<size_t, size_t> fTuneEnums;
  std::vector<novarwgt::Tune const *> fTunes;
  std::vector<double> fTuneValues;
};
