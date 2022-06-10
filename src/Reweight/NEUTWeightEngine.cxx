#include "NEUTWeightEngine.h"

#include "CommonBlockIFace.h"
#include "NReWeight.h"
#include "NReWeightFactory.h"

#include "WeightUtils.h"

NEUTWeightEngine::NEUTWeightEngine(std::string name) {

  std::string neut_card = FitPar::Config().GetParS("NEUT_CARD");
  if (!neut_card.size()) {
    NUIS_ABORT(
        "[ERROR]: When using NEUTReWeight must set NEUT_CARD config option.");
  }
  // No need to vomit the contents of the card file all over my screen
  StopTalking();
  neut::CommonBlockIFace::Initialize(neut_card);
  StartTalking();

  fNeutRW = neut::rew::MakeNReWeightInstance();
  fCalcName = name;
};

NEUTWeightEngine::~NEUTWeightEngine() {}

void NEUTWeightEngine::IncludeDial(std::string name, double startval) {
  // Get First enum
  int nuisenum = Reweight::ConvDial(name, kNEUT);

  // Split by commas
  std::vector<std::string> allnames = GeneralUtils::ParseToStr(name, ",");
  for (uint i = 0; i < allnames.size(); i++) {
    std::string singlename = allnames[i];

    // Get RW
    int gensyst = fNeutRW->DialFromString(singlename);

    // Initialize dial
    NUIS_LOG(REC, "Registering " << singlename << "(NEUT Enum: " << gensyst
                                 << ") from " << name);

    fEnumIndex[nuisenum].push_back(gensyst);
    fNameIndex[name].push_back(gensyst);
  }

  // Set Value if given
  if (startval != UNDEF_DIAL_VALUE) {
    SetDialValue(nuisenum, startval);
  }
}

void NEUTWeightEngine::SetDialValue(int nuisenum, double val) {
  for (auto gensyst : fEnumIndex[nuisenum]) {
    fNeutRW->SetDial_To_Value(gensyst, val);
  }
}

void NEUTWeightEngine::SetDialValue(std::string name, double val) {
  for (auto gensyst : fNameIndex[name]) {
    fNeutRW->SetDial_To_Value(gensyst, val);
  }
}

void NEUTWeightEngine::Reconfigure(bool silent) {
  // Hush now...
  if (silent)
    StopTalking();

  // Reconf
  StopTalking();
  fNeutRW->Reconfigure();
  StartTalking();

  // Shout again
  if (silent)
    StartTalking();
}

double NEUTWeightEngine::CalcWeight(BaseFitEvt *evt) {
  double rw_weight = 1.0;

  // Skip Non-NEUT
  if (evt->fType != kNEUT) {
    return 1.0;
  }

  // Hush now
  StopTalking();

  neut::CommonBlockIFace::Get().ReadVect(evt->fNeutVect);
  rw_weight = fNeutRW->CalcWeight();
  // Speak Now
  StartTalking();

  if (!std::isnormal(rw_weight)) {
    NUIS_ERR(WRN, "T2KReWeight returned weight: " << rw_weight);
    rw_weight = 0;
  }

  // Return rw_weight
  return rw_weight;
}
