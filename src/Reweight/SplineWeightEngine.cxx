#include "WeightUtils.h"

#include "SplineWeightEngine.h"

SplineWeightEngine::SplineWeightEngine(std::string name) {

  // Setup the Reweight engien
  fCalcName = name;
  NUIS_LOG(FIT, "Setting up Spline RW : " << fCalcName);

  // Set Abs Twk Config
  fIsAbsTwk = true;
};

void SplineWeightEngine::IncludeDial(std::string name, double startval) {

  // Get RW Enum and name
  int nuisenum = Reweight::ConvDial(name, kSPLINEPARAMETER);

  // Initialise new vector
  fEnumIndex[nuisenum];
  fNameIndex[name];

  // Split by commas
  std::vector<std::string> allnames = GeneralUtils::ParseToStr(name, ",");
  for (uint i = 0; i < allnames.size(); i++) {
    std::string singlename = allnames[i];

    // Get Single Enum For this dial
    int singleenum = Reweight::ConvDial(singlename, kCUSTOM);

    // Fill Maps
    int index = fValues.size();
    fValues.push_back(0.0);
    fSingleEnums.push_back(singleenum);
    fSingleNames.push_back(singlename);

    // Setup index
    fEnumIndex[nuisenum].push_back(index);
    fNameIndex[name].push_back(index);
  }

  // Set Value if given
  if (startval != _UNDEF_DIAL_VALUE_) {
    SetDialValue(name, startval);
  }
}

void SplineWeightEngine::SetDialValue(int nuisenum, double val) {
  std::vector<size_t> indices = fEnumIndex[nuisenum];
  for (uint i = 0; i < indices.size(); i++) {
    fValues[indices[i]] = val;
  }
}

void SplineWeightEngine::SetDialValue(std::string name, double val) {
  std::vector<size_t> indices = fNameIndex[name];
  for (uint i = 0; i < indices.size(); i++) {
    fValues[indices[i]] = val;
  }
}

void SplineWeightEngine::Reconfigure(bool silent) {
  for (size_t i = 0; i < fSingleNames.size(); i++) {
    fSplineValueMap[fSingleNames[i]] = fValues[i];
  }
}

double SplineWeightEngine::CalcWeight(BaseFitEvt *evt) {

  if (!evt->fSplineRead)
    return 1.0;

  if (evt->fSplineRead->NeedsReconfigure()) {
    evt->fSplineRead->Reconfigure(fSplineValueMap);
  }

  double rw_weight = evt->fSplineRead->CalcWeight(evt->fSplineCoeff);
  if (rw_weight < 0.0)
    rw_weight = 0.0;

  return rw_weight;
}
