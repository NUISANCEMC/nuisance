#include "T2KWeightEngine.h"
#ifdef __T2KREW_ENABLED__
#ifdef T2KRW_OA2021_INTERFACE
#include "T2KReWeight/WeightEngines/NEUT/T2KNEUTUtils.h"
#else
#include "T2KNeutUtils.h"
#endif
#endif

T2KWeightEngine::T2KWeightEngine(std::string name) {
#ifdef __T2KREW_ENABLED__

#if defined(__NEUT_VERSION__) && (__NEUT_VERSION__ >= 541)

  // No need to vomit the contents of the card file all over my screen
  StopTalking();

#ifdef T2KRW_OA2021_INTERFACE
  if (!t2krew::T2KNEUTUtils::CardIsSet()) {
    std::string neut_card = FitPar::Config().GetParS("NEUT_CARD");
    if (neut_card.size()) {
      t2krew::T2KNEUTUtils::SetCardFile(neut_card);
    }
  }
#else
  std::string neut_card = FitPar::Config().GetParS("NEUT_CARD");
  if (!neut_card.size()) {
    NUIS_ABORT(
        "[ERROR]: When using T2KReWeight must set NEUT_CARD config option.");
  }
  NUIS_LOG(Fit, "Using NEUT card file: " << neut_card);
  t2krew::T2KNeutUtils::SetCardFile(neut_card);
#endif
  StartTalking();
#endif

  // Setup the NEUT Reweight engien
  fCalcName = name;
  NUIS_LOG(FIT, "Setting up T2K RW: " << fCalcName);

  // Create RW Engine suppressing cout
  StopTalking();

#ifdef T2KRW_OA2021_INTERFACE
  fT2KRW = t2krew::MakeT2KReWeightInstance();
#else
  // Create Main RW Engine
  fT2KRW = new t2krew::T2KReWeight();

  // Setup Sub RW Engines (Only activated for neut and niwg)
  fT2KNeutRW = new t2krew::T2KNeutReWeight();
  fT2KNIWGRW = new t2krew::T2KNIWGReWeight();

  fT2KRW->AdoptWghtEngine("fNeutRW", fT2KNeutRW);
  fT2KRW->AdoptWghtEngine("fNIWGRW", fT2KNIWGRW);

  fT2KRW->Reconfigure();

  // allow cout again
  StartTalking();

  // Set Abs Twk Config
  fIsAbsTwk = (FitPar::Config().GetParB("setabstwk"));
#endif
#else
  NUIS_ABORT("T2K RW NOT ENABLED");
#endif
};

void T2KWeightEngine::IncludeDial(std::string name, double startval) {
#ifdef __T2KREW_ENABLED__
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
#ifdef T2KRW_OA2021_INTERFACE
    int gensyst = t2krew::T2KSystToInt(fT2KRW->DialFromString(name));
#else
    t2krew::T2KSyst_t gensyst = t2krew::T2KSyst::FromString(name);
#endif

    // Fill Maps
    int index = fValues.size();
    fValues.push_back(0.0);
    fT2KSysts.push_back(gensyst);

    // Initialize dial
    NUIS_LOG(REC, "Registering " << singlename << " from " << name);
#ifndef T2KRW_OA2021_INTERFACE
    fT2KRW->Systematics().Include(gensyst);

    // If Absolute
    if (fIsAbsTwk) {
      fT2KRW->Systematics().SetAbsTwk(gensyst);
    }
#endif

    // Setup index
    fEnumIndex[nuisenum].push_back(index);
    fNameIndex[name].push_back(index);
  }

  // Set Value if given
  if (startval != _UNDEF_DIAL_VALUE_) {
    SetDialValue(nuisenum, startval);
  }
#endif
}

void T2KWeightEngine::SetDialValue(int nuisenum, double val) {
#ifdef __T2KREW_ENABLED__
  std::vector<size_t> indices = fEnumIndex[nuisenum];
  for (uint i = 0; i < indices.size(); i++) {
    fValues[indices[i]] = val;
#ifdef T2KRW_OA2021_INTERFACE
    fT2KRW->SetDial_To_Value(t2krew::IntToT2KSyst(fT2KSysts[indices[i]]), val);
#else
    fT2KRW->Systematics().SetTwkDial(fT2KSysts[indices[i]], val);
#endif
  }
#endif
}

void T2KWeightEngine::SetDialValue(std::string name, double val) {
#ifdef __T2KREW_ENABLED__
  std::vector<size_t> indices = fNameIndex[name];
  for (uint i = 0; i < indices.size(); i++) {
    fValues[indices[i]] = val;
#ifdef T2KRW_OA2021_INTERFACE
    fT2KRW->SetDial_To_Value(t2krew::IntToT2KSyst(fT2KSysts[indices[i]]), val);
#else
    fT2KRW->Systematics().SetTwkDial(fT2KSysts[indices[i]], val);
#endif
  }
#endif
}

void T2KWeightEngine::Reconfigure(bool silent) {
#ifdef __T2KREW_ENABLED__
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
#endif
}

double T2KWeightEngine::CalcWeight(BaseFitEvt *evt) {
  double rw_weight = 1.0;

#ifdef __T2KREW_ENABLED__
  // Skip Non-NEUT
  if (evt->fType != kNEUT) {
    return 1.0;
  }

  // Hush now
  StopTalking();

// Get Weight For NEUT
#ifdef T2KRW_OA2021_INTERFACE
  rw_weight = fT2KRW->CalcWeight(t2krew::Event::Make(evt->fNeutVect));
#else
  rw_weight = fT2KRW->CalcWeight(evt->fNeutVect);
#endif
  // Speak Now
  StartTalking();
#endif

  if (!std::isnormal(rw_weight)) {
    NUIS_ERR(WRN, "NEUT returned weight: " << rw_weight);
    rw_weight = 0;
  }

  // Return rw_weight
  return rw_weight;
}
