#include "NUISANCEWeightEngine.h"
#include "NUISANCEWeightCalcs.h"

#ifdef __MINERVA_RW_ENABLED__
#ifdef __GENIE_ENABLED__
#include "MINERvAWeightCalcs.h"
#endif
#endif

NUISANCEWeightEngine::NUISANCEWeightEngine(std::string name) {
  // Setup the NUISANCE Reweight engine
  fCalcName = name;
  NUIS_LOG(FIT, "Setting up NUISANCE Custom RW : " << fCalcName);

  // Load in all Weight Calculations
  GaussianModeCorr *GaussianMode = new GaussianModeCorr();
  std::string Gaussian_Method =
      FitPar::Config().GetParS("Gaussian_Enhancement");
  if (Gaussian_Method == "Tilt-Shift") {
    GaussianMode->SetMethod(true);
  } else if (Gaussian_Method == "Normal") {
    GaussianMode->SetMethod(false);
  } else {
    NUIS_ABORT("I do not recognise method "
           << Gaussian_Method
           << " for the Gaussian enhancement, so will die now...");
  }
  fWeightCalculators.push_back(GaussianMode);
  fWeightCalculators.push_back(new ModeNormCalc());
  fWeightCalculators.push_back(new SBLOscWeightCalc());
  fWeightCalculators.push_back(new BeRPACalc());

#ifdef __MINERVA_RW_ENABLED__
#ifdef __GENIE_ENABLED__
  fWeightCalculators.push_back(new nuisance::reweight::MINERvAReWeight_QE());
  fWeightCalculators.push_back(new nuisance::reweight::MINERvAReWeight_MEC());
  fWeightCalculators.push_back(new nuisance::reweight::MINERvAReWeight_RES());
  fWeightCalculators.push_back(new nuisance::reweight::MINOSRPA());
  fWeightCalculators.push_back(new nuisance::reweight::LagrangeRPA());
  fWeightCalculators.push_back(new nuisance::reweight::RikRPA());
  fWeightCalculators.push_back(new nuisance::reweight::COHBrandon());
  fWeightCalculators.push_back(new nuisance::reweight::WEnhancement());
#endif
#endif

  // Set Abs Twk Config
  fIsAbsTwk = true;
};

void NUISANCEWeightEngine::IncludeDial(std::string name, double startval) {
  // Get First enum
  int nuisenum = Reweight::ConvDial(name, kCUSTOM);

  // Setup Maps
  fEnumIndex[nuisenum]; // = std::vector<size_t>(0);
  fNameIndex[name];     // = std::vector<size_t>(0);

  // Split by commas
  std::vector<std::string> allnames = GeneralUtils::ParseToStr(name, ",");
  for (uint i = 0; i < allnames.size(); i++) {
    std::string singlename = allnames[i];

    // Get RW
    int singleenum = Reweight::ConvDial(singlename, kCUSTOM);

    // Fill Maps
    int index = fValues.size();
    fValues.push_back(0.0);
    fNUISANCEEnums.push_back(singleenum);

    // Initialize dial
    NUIS_LOG(FIT, "Registering " << singlename << " from " << name);

    // Setup index
    fEnumIndex[nuisenum].push_back(index);
    fNameIndex[name].push_back(index);
  }

  // Set Value if given
  if (startval != _UNDEF_DIAL_VALUE_) {
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
    // Is this parameter handled
    bool IsHandledSomewhere = false;
    for (std::vector<NUISANCEWeightCalc *>::iterator calciter =
             fWeightCalculators.begin();
         calciter != fWeightCalculators.end(); calciter++) {
      NUISANCEWeightCalc *nuiscalc =
          static_cast<NUISANCEWeightCalc *>(*calciter);

      if (nuiscalc->IsHandled(fNUISANCEEnums[i])) {
        nuiscalc->SetDialValue(fNUISANCEEnums[i], fValues[i]);
        IsHandledSomewhere = true;
      }
    }

    // Check if this parameter is actually handled
    if (!IsHandledSomewhere) {
      std::string name = GetNameFromEnum(i);
      NUIS_ABORT("NUISANCE parameter " << name << " (enum " << i << ") not enabled, can not continue!");
    } // End check of it's handled
  } // End loop over enums
} // Return

double NUISANCEWeightEngine::CalcWeight(BaseFitEvt *evt) {
  double rw_weight = 1.0;

  // Cast as usable class
  for (std::vector<NUISANCEWeightCalc *>::iterator iter =
           fWeightCalculators.begin();
       iter != fWeightCalculators.end(); iter++) {
    NUISANCEWeightCalc *nuiscalc = static_cast<NUISANCEWeightCalc *>(*iter);

    rw_weight *= nuiscalc->CalcWeight(evt);
  }

  // Return rw_weight
  return rw_weight;
}
