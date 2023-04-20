#include "T2KWeightEngine.h"

#ifdef NEUT_ENABLED
#include "T2KReWeight/WeightEngines/NEUT/T2KNEUTUtils.h"
#endif

T2KWeightEngine::T2KWeightEngine(std::string name) {
  // No need to vomit the contents of the card file all over my screen

#ifdef NEUT_ENABLED
  StopTalking();

  if (!t2krew::T2KNEUTUtils::CardIsSet()) {
    std::string neut_card = FitPar::Config().GetParS("NEUT_CARD");
    if (neut_card.size()) {
      t2krew::T2KNEUTUtils::SetCardFile(neut_card);
    } else {
      StartTalking();
      NUIS_ABORT("NEUT is enabled but not <config NEUT_CARD=\"neut.card\" /> "
                 "tag was found in config file.");
    }
  }

  StartTalking();
#endif

  // Setup the NEUT Reweight engien
  fCalcName = name;
  NUIS_LOG(FIT, "Setting up T2K RW: " << fCalcName);

  // Create RW Engine suppressing cout
  StopTalking();

  fT2KRW = t2krew::MakeT2KReWeightInstance(t2krew::Event::kNEUT);
};

void T2KWeightEngine::IncludeDial(std::string name, double startval) {
  // Get First enum
  int nuisenum = Reweight::ConvDial(name, kT2K);

  // Setup Maps
  fEnumIndex[nuisenum]; // = std::vector<size_t>(0);
  fNameIndex[name];     // = std::vector<size_t>(0);

  // Split by commas
  std::vector<std::string> allnames = GeneralUtils::ParseToStr(name, ",");
  for (uint i = 0; i < allnames.size(); i++) {
    std::string singlename = allnames[i];

    // Get RW
    int gensyst = t2krew::T2KSystToInt(fT2KRW->DialFromString(name));

    // Fill Maps
    int index = fValues.size();
    fValues.push_back(0.0);
    fT2KSysts.push_back(gensyst);

    // Initialize dial
    NUIS_LOG(REC, "Registering " << singlename << " from " << name);

    // Setup index
    fEnumIndex[nuisenum].push_back(index);
    fNameIndex[name].push_back(index);
  }

  // Set Value if given
  if (startval != UNDEF_DIAL_VALUE) {
    SetDialValue(nuisenum, startval);
  }
}

void T2KWeightEngine::SetDialValue(int nuisenum, double val) {
  std::vector<size_t> indices = fEnumIndex[nuisenum];
  for (uint i = 0; i < indices.size(); i++) {
    fValues[indices[i]] = val;
    fT2KRW->SetDial_To_Value(t2krew::IntToT2KSyst(fT2KSysts[indices[i]]), val);
  }
}

void T2KWeightEngine::SetDialValue(std::string name, double val) {
  std::vector<size_t> indices = fNameIndex[name];
  for (uint i = 0; i < indices.size(); i++) {
    fValues[indices[i]] = val;
    fT2KRW->SetDial_To_Value(t2krew::IntToT2KSyst(fT2KSysts[indices[i]]), val);
  }
}

void T2KWeightEngine::Reconfigure(bool silent) {
  // Hush now...
  if (silent)
    StopTalking();

  // Reconf
  StopTalking();
  fT2KRW->Reconfigure();
  StartTalking();

  // Shout again
  if (silent)
    StartTalking();
}

double T2KWeightEngine::CalcWeight(BaseFitEvt *evt) {
  double rw_weight = 1.0;

  // Skip Non-NEUT
  if (evt->fType != kNEUT) {
    return 1.0;
  }

  // Hush now
  StopTalking();

  rw_weight = fT2KRW->CalcWeight(t2krew::Event::Make(evt->fNeutVect));
  // Speak Now
  StartTalking();

  if (!std::isnormal(rw_weight)) {
    NUIS_ERR(WRN, "T2KReWeight returned weight: " << rw_weight);
    rw_weight = 0;
  }

  // Return rw_weight
  return rw_weight;
}
