#include "WeightUtils.h"

#include "LikelihoodWeightEngine.h"

LikelihoodWeightEngine::LikelihoodWeightEngine(std::string name) {

  // Setup the NEUT Reweight engien
  fCalcName = name;
  NUIS_LOG(FIT, "Setting up Likelihood Weight RW : " << fCalcName);

  // Set Abs Twk Config
  fIsAbsTwk = true;
};

void LikelihoodWeightEngine::IncludeDial(std::string name, double startval) {

  // Get NUISANCE Enum
  int nuisenum = Reweight::ConvDial(name, kNORM);

  // Setup Maps
  fEnumIndex[nuisenum]; // = std::vector<size_t>(0);
  fNameIndex[name];     // = std::vector<size_t>(0);

  // Split by commas
  std::vector<std::string> allnames = GeneralUtils::ParseToStr(name, ",");
  for (uint i = 0; i < allnames.size(); i++) {
    std::string singlename = allnames[i];

    // Fill Maps
    int index = fValues.size();
    fValues.push_back(1.0);

    fEnumIndex[nuisenum].push_back(index);
    fNameIndex[name].push_back(index);
  }

  // Set Value if given
  if (startval != _UNDEF_DIAL_VALUE_) {
    SetDialValue(name, startval);
  }
};

void LikelihoodWeightEngine::SetDialValue(int nuisenum, double val) {
  std::vector<size_t> indices = fEnumIndex[nuisenum];
  for (uint i = 0; i < indices.size(); i++) {
    fValues[indices[i]] = val;
  }
}

void LikelihoodWeightEngine::SetDialValue(std::string name, double val) {
  std::vector<size_t> indices = fNameIndex[name];
  for (uint i = 0; i < indices.size(); i++) {
    fValues[indices[i]] = val;
  }
}

double LikelihoodWeightEngine::GetDialValue(std::string name) {

  // Check for exact dial names
  if (fNameIndex.find(name) != fNameIndex.end()) {
    return fValues[fNameIndex[name][0]];

    // If not iterate and check entry in one of the keys
  } else {
    for (std::map<std::string, std::vector<size_t> >::iterator iter =
             fNameIndex.begin();
         iter != fNameIndex.end(); iter++) {
      std::string keyname = iter->first;

      if (keyname.find(name) != std::string::npos) {
        return fValues[iter->second[0]];
      }
    }
  }

  return -1.0;
}

void LikelihoodWeightEngine::Reconfigure(bool silent) {
  // Empty placeholder incase we want print statements...
}
