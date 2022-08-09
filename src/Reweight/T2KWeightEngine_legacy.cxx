#include "T2KNIWGUtils.h"
#include "T2KNeutUtils.h"
#include "T2KWeightEngine.h"

using namespace t2krew;

#include "T2KNeutUtils.h"

T2KWeightEngine::T2KWeightEngine(std::string name) {
#if NEUT_VERSION >= 541
  // No need to vomit the contents of the card file all over my screen
  // StopTalking();

  std::string neut_card = FitPar::Config().GetParS("NEUT_CARD");
  if (neut_card.length() > 0) {
    t2krew::T2KNeutUtils::SetCardFile(neut_card);
    StartTalking();
    NUIS_LOG(FIT, "Using NEUT card file: " << neut_card);
  } else {
    t2krew::T2KNeutUtils::SetCardFile(std::string(std::getenv("NUISANCE")) +
                                      "/data/neut/neut_minimal_6t.card");
    StartTalking();
    NUIS_LOG(FIT,
             "Using NEUT card file: " << std::string(std::getenv("NUISANCE")) +
                                             "/data/neut/neut_minimal_6t.card");
  }

  // StartTalking();
#endif

  // Setup the NEUT Reweight engien
  fCalcName = name;
  NUIS_LOG(FIT, "Setting up T2K RW: " << fCalcName);

  // Create RW Engine suppressing cout
  StopTalking();

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
};

void T2KWeightEngine::IncludeDial(std::string name, double startval) {
  // Get First enum
  int nuisenum = Reweight::ConvDial(name, kT2K);

  // Setup Maps
  fEnumIndex[nuisenum];  // = std::vector<size_t>(0);
  fNameIndex[name];      // = std::vector<size_t>(0);

  // Split by commas
  std::vector<std::string> allnames = GeneralUtils::ParseToStr(name, ",");
  for (uint i = 0; i < allnames.size(); i++) {
    std::string singlename = allnames[i];

    // Get RW
    t2krew::T2KSyst_t gensyst = t2krew::T2KSyst::FromString(name);

    // Fill Maps
    int index = fValues.size();
    fValues.push_back(0.0);
    fT2KSysts.push_back(gensyst);

    // Initialize dial
    NUIS_LOG(REC, "Registering " << singlename << " from " << name);
    fT2KRW->Systematics().Include(gensyst);

    // If Absolute
    if (fIsAbsTwk) {
      fT2KRW->Systematics().SetAbsTwk(gensyst);
    }

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
    fT2KRW->Systematics().SetTwkDial(fT2KSysts[indices[i]], val);
  }
}

void T2KWeightEngine::SetDialValue(std::string name, double val) {
  std::vector<size_t> indices = fNameIndex[name];
  for (uint i = 0; i < indices.size(); i++) {
    fValues[indices[i]] = val;
    fT2KRW->Systematics().SetTwkDial(fT2KSysts[indices[i]], val);
  }
}

void T2KWeightEngine::Reconfigure(bool silent) {
  // Hush now...
  if (silent) StopTalking();

  // Reconf
  StopTalking();
  fT2KRW->Reconfigure();
  StartTalking();

  // Shout again
  if (silent) StartTalking();
}

double T2KWeightEngine::CalcWeight(BaseFitEvt *evt) {
  double rw_weight = 1.0;

  // Skip Non-NEUT
  if (evt->fType != kNEUT) {
    return 1.0;
  }

  // Hush now
  StopTalking();

  rw_weight = fT2KRW->CalcWeight(evt->fNeutVect);
  // Speak Now
  StartTalking();

  if (!std::isnormal(rw_weight)) {
    NUIS_ERR(WRN, "T2KReWeight returned weight: " << rw_weight);
    rw_weight = 0;
  }

  // Return rw_weight
  return rw_weight;
}
