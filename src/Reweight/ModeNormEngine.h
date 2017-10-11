#ifndef ModeNormEngine_H
#define ModeNormEngine_H

#include "FitLogger.h"
#include "GeneratorUtils.h"
#include "WeightEngineBase.h"

class ModeNormEngine : public WeightEngineBase {
 public:
  ModeNormEngine(std::string name="ModeNormEngine") : fName(name){};
  ~ModeNormEngine(){};

  void IncludeDial(std::string name, double startval) {
    int rwenum = Reweight::ConvDial(name, kMODENORM);
    int mode = Reweight::RemoveDialType(rwenum);
    if (fDialEnumIndex.count(mode)) {
      THROW("Mode dial: " << mode
                          << " already included. Cannot include twice.");
    }
    fDialEnumIndex[mode] = fDialValues.size();
    fDialValues.push_back(startval);
    QLOG(FIT, "Added mode dial for mode: " << mode);
  }
  void SetDialValue(int rwenum, double val) {
    int mode = Reweight::RemoveDialType(rwenum);
    if (!fDialEnumIndex.count(mode)) {
      THROW("Mode dial: " << mode
                          << " has not been included. Cannot set value.");
    }
    fDialValues[fDialEnumIndex[mode]] = val;
  }
  void SetDialValue(std::string name, double val) {
    SetDialValue(Reweight::ConvDial(name, kMODENORM), val);
  }

  void Reconfigure(bool silent = false) { (void)silent; }
  double CalcWeight(BaseFitEvt* evt) {
    int mode = abs(evt->Mode);
    if (!fDialEnumIndex.count(mode)) {
      return 1;
    }
    return fDialValues[fDialEnumIndex[mode]];
  };
  bool NeedsEventReWeight() { return false; };

  double GetDialValue(std::string name) {
    int rwenum = Reweight::ConvDial(name, kMODENORM);
    int mode = Reweight::RemoveDialType(rwenum);
    if (fDialEnumIndex.count(mode)) {
      return fDialValues[fDialEnumIndex[mode]];
    } else {
      return 0xdeadbeef;
    }
  }

  static int SystEnumFromString(std::string const& name) {
    std::vector<std::string> splits = GeneralUtils::ParseToStr(name, "_");
    if (splits.size() != 2) {
      ERR(FTL) << "Attempting to parse dial name: \"" << name
               << "\" as a mode norm dial but failed. Expect e.g. \"mode_2\"."
               << std::endl;
    }

    int mode_num = GeneralUtils::StrToInt(splits[1]);
    if (!mode_num) {
      ERR(FTL) << "Attempting to parse dial name: \"" << name
               << "\" as a mode norm dial but failed." << std::endl;
      throw;
    }
    return 60 + mode_num;
  }

  std::map<int, int> fDialEnumIndex;
  std::vector<double> fDialValues;

  std::string fName;
};

#endif
