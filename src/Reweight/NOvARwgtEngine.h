#pragma once

#include "WeightEngineBase.h"

namespace novarwgt {
class ISystKnob;
class Tune;
} // namespace novarwgt

class NOvARwgtEngine : public WeightEngineBase {
public:
  NOvARwgtEngine() {
    InitializeKnobs();
    InitializeGENIE();
  };
  virtual ~NOvARwgtEngine(){};

  void InitializeKnobs();
  void InitializeGENIE();
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

  std::map<size_t, size_t> fTuneEnums;
  std::vector<novarwgt::Tune const *> fTunes;
  std::vector<double> fTuneValues;
  std::vector<bool> fTuneInUse;

  std::map<size_t, size_t> fKnobEnums;
  std::vector<novarwgt::ISystKnob const *> fKnobs;
  std::vector<novarwgt::Tune const *> fKnobTunes;
  std::vector<size_t> fKnobTuneidx;
  std::vector<double> fKnobValues;
  std::vector<bool> fKnobInUse;
};
