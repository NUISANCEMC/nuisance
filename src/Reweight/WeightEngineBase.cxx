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

std::string WeightEngineBase::GetNameFromEnum(int nuisenum) {

  // Find the name in the map; need to iterate through the map
  for (std::map<std::string, std::vector<size_t> >::iterator it = fNameIndex.begin(); it!=fNameIndex.end(); ++it) {
    std::string name = (*it).first;
    std::vector<size_t> Enums = (*it).second;
    bool found = false;
    for (std::vector<size_t>::iterator EnumIt = Enums.begin(); EnumIt != Enums.end(); ++EnumIt) {
      size_t Enum = (*EnumIt);
      if (Enum == (size_t)nuisenum) {
        found = true;
        break;
      }
    }
    if (found) {
      return name;
    }
  } // Finish looping over the map

  NUIS_ABORT("Could not find a matching name for parameter enum " << nuisenum);

  return std::string("EMPTY");

}
