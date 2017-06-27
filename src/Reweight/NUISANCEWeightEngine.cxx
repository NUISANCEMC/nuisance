#include "NUISANCEWeightEngine.h"
#include "NUISANCEWeightCalcs.h"

NUISANCEWeightEngine::NUISANCEWeightEngine(std::string name) {

  // Setup the NUISANCE Reweight engine
  fCalcName = name;
  LOG(FIT) << "Setting up NUISANCE Custom RW : " << fCalcName << std::endl;

  // Load in all Weight Calculations
  fWeightCalculators.push_back( new GaussianModeCorr() );
  fWeightCalculators.push_back( new ModeNormCalc() );
  // Set Abs Twk Config
  fIsAbsTwk = true;

};


void NUISANCEWeightEngine::IncludeDial(std::string name, double startval) {

  // Get First enum
  int nuisenum = Reweight::ConvDial(name, kCUSTOM);

  // Setup Maps
  fEnumIndex[nuisenum];// = std::vector<size_t>(0);
  fNameIndex[name]; // = std::vector<size_t>(0);

  // Split by commas
  std::vector<std::string> allnames = GeneralUtils::ParseToStr(name, ",");
  for (uint i = 0; i < allnames.size(); i++) {
    std::string singlename = allnames[i];

    // Get RW
    int singleenum =  Reweight::ConvDial(singlename, kCUSTOM);

    // Fill Maps
    int index = fValues.size();
    fValues.push_back(0.0);
    fNUISANCEEnums.push_back(singleenum);

    // Initialize dial
    std::cout << "Registering " << singlename << " from " << name << std::endl;

    // Setup index
    fEnumIndex[nuisenum].push_back(index);
    fNameIndex[name].push_back(index);
  }

  // Set Value if given
  if (startval != -999.9) {
    SetDialValue(nuisenum, startval);
  }
};


void NUISANCEWeightEngine::SetDialValue(int nuisenum, double val) {
  std::vector<size_t> indices = fEnumIndex[nuisenum];
  for (uint i = 0; i < indices.size(); i++) {
    fValues[indices[i]] = val;
  }
}

void NUISANCEWeightEngine::SetDialValue(std::string name, double val) {
  std::vector<size_t> indices = fNameIndex[name];
  for (uint i = 0; i < indices.size(); i++) {
    fValues[indices[i]] = val;
  }
}

void NUISANCEWeightEngine::Reconfigure(bool silent) {

  for (size_t i = 0; i < fNUISANCEEnums.size(); i++) {

    for (std::vector<NUISANCEWeightCalc*>::iterator calciter = fWeightCalculators.begin();
         calciter != fWeightCalculators.end(); calciter++) {

      NUISANCEWeightCalc* nuiscalc = static_cast<NUISANCEWeightCalc*>(*calciter);

      if (nuiscalc->IsHandled(fNUISANCEEnums[i])) {
        nuiscalc->SetDialValue(fNUISANCEEnums[i], fValues[i]);
      }
    }
  }
}



double NUISANCEWeightEngine::CalcWeight(BaseFitEvt * evt) {
  double rw_weight = 1.0;

  // Cast as usable class
  for (std::vector<NUISANCEWeightCalc*>::iterator iter = fWeightCalculators.begin();
       iter != fWeightCalculators.end(); iter++) {
    NUISANCEWeightCalc* nuiscalc = static_cast<NUISANCEWeightCalc*>(*iter);

    rw_weight *= nuiscalc->CalcWeight(evt);
  }

  // Return rw_weight
  return rw_weight;
}




























