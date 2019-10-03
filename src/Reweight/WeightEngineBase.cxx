#include "WeightEngineBase.h"

bool WeightEngineBase::IsDialIncluded(std::string name) {
  return (fNameIndex.find(name) != fNameIndex.end());
}

bool WeightEngineBase::IsDialIncluded(int nuisenum) {
  return (fEnumIndex.find(nuisenum) != fEnumIndex.end());
}

double WeightEngineBase::GetDialValue(std::string name) {
  if (!IsDialIncluded(name)) {
    NUIS_ABORT("Dial " << name << " not included in " << fCalcName);
  }
  return fValues[fNameIndex[name][0]];
}

double WeightEngineBase::GetDialValue(int nuisenum) {
  if (!IsDialIncluded(nuisenum)) {
    NUIS_ABORT("Dial Enum " << nuisenum << " not included in " << fCalcName);
  }
  return fValues[fEnumIndex[nuisenum][0]];
}
