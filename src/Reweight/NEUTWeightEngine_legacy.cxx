#include "NEUTWeightEngine.h"

#include "WeightUtils.h"

// Dials removed in NEUT 5.4.1
#if NEUT_VERSION < 541
#include "NReWeightCasc.h"
#include "NReWeightNuXSecCCRES.h"
#include "NReWeightNuXSecCOH.h"
#include "NReWeightNuXSecDIS.h"
#include "NReWeightNuXSecNC.h"
#include "NReWeightNuXSecNCEL.h"
#include "NReWeightNuXSecNCRES.h"
#include "NReWeightNuclPiless.h"
#endif

#if NEUT_VERSION >= 541
#include "CommonBlockIFace.h"
#endif

#if NEUT_BUILTIN_FILL_NEUT_COMMONS
#include "NEUTInputHandler.h"
#endif

#include "NSyst.h"
#include "NSystUncertainty.h"
#include "neutpart.h"
#include "neutvect.h"

#include "NReWeightNuXSecCCQE.h"
#include "NReWeightNuXSecRES.h"

#include <algorithm>
#include <memory>

NEUTWeightEngine::NEUTWeightEngine(std::string name) {

#if NEUT_VERSION >= 541
  std::string neut_card = FitPar::Config().GetParS("NEUT_CARD");
  if (!neut_card.size()) {
    NUIS_ABORT(
        "[ERROR]: When using NEUTReWeight must set NEUT_CARD config option.");
  }
  // No need to vomit the contents of the card file all over my screen
  StopTalking();
  neut::CommonBlockIFace::Initialize(neut_card);
  StartTalking();
#endif

  // Setup the NEUT Reweight engien
  fCalcName = name;
  NUIS_LOG(FIT, "Setting up NEUT RW : " << fCalcName);

  // Create RW Engine suppressing cout
  StopTalking();
  fNeutRW = std::unique_ptr<neut::rew::NReWeight>(new neut::rew::NReWeight());
  TDirectory *olddir = gDirectory;

  // get list of vetoed calc engines (just for debug really)
  std::string rw_engine_list =
      FitPar::Config().GetParS("FitWeight_fNeutRW_veto");
  bool xsec_ccqe = rw_engine_list.find("xsec_ccqe") == std::string::npos;
  bool xsec_res = rw_engine_list.find("xsec_res") == std::string::npos;

  // Activate each calc engine
  if (xsec_ccqe)
    fNeutRW->AdoptWghtCalc("xsec_ccqe", new neut::rew::NReWeightNuXSecCCQE);
  if (xsec_res)
    fNeutRW->AdoptWghtCalc("xsec_res", new neut::rew::NReWeightNuXSecRES);

    // Dials removed in NEUT 5.4.1
#if NEUT_VERSION < 541
  bool xsec_ccres = rw_engine_list.find("xsec_ccres") == std::string::npos;
  bool xsec_coh = rw_engine_list.find("xsec_coh") == std::string::npos;
  bool xsec_dis = rw_engine_list.find("xsec_dis") == std::string::npos;
  bool xsec_ncel = rw_engine_list.find("xsec_ncel") == std::string::npos;
  bool xsec_nc = rw_engine_list.find("xsec_nc") == std::string::npos;
  bool xsec_ncres = rw_engine_list.find("xsec_ncres") == std::string::npos;
  bool nucl_casc = rw_engine_list.find("nucl_casc") == std::string::npos;
  bool nucl_piless = rw_engine_list.find("nucl_piless") == std::string::npos;

  if (nucl_casc)
    fNeutRW->AdoptWghtCalc("nucl_casc", new neut::rew::NReWeightCasc);
  if (xsec_coh)
    fNeutRW->AdoptWghtCalc("xsec_coh", new neut::rew::NReWeightNuXSecCOH);
  if (xsec_nc)
    fNeutRW->AdoptWghtCalc("xsec_nc", new neut::rew::NReWeightNuXSecNC);
  if (nucl_piless)
    fNeutRW->AdoptWghtCalc("nucl_piless", new neut::rew::NReWeightNuclPiless);
  if (xsec_ncres)
    fNeutRW->AdoptWghtCalc("xsec_ncres", new neut::rew::NReWeightNuXSecNCRES);
  if (xsec_ccres)
    fNeutRW->AdoptWghtCalc("xsec_ccres", new neut::rew::NReWeightNuXSecCCRES);
  if (xsec_ncel)
    fNeutRW->AdoptWghtCalc("xsec_ncel", new neut::rew::NReWeightNuXSecNCEL);
  if (xsec_dis)
    fNeutRW->AdoptWghtCalc("xsec_dis", new neut::rew::NReWeightNuXSecDIS);
#endif

  fNeutRW->Reconfigure();
  olddir->cd();

  // Set Abs Twk Config
  fIsAbsTwk = (FitPar::Config().GetParB("setabstwk"));

  // allow cout again
  StartTalking();
};

NEUTWeightEngine::~NEUTWeightEngine(){}

void NEUTWeightEngine::IncludeDial(std::string name, double startval) {
  // Get First enum
  int nuisenum = Reweight::ConvDial(name, kNEUT);

  // Setup Maps
  fEnumIndex[nuisenum]; // = std::vector<size_t>(0);
  fNameIndex[name];     // = std::vector<size_t>(0);

  // Split by commas
  std::vector<std::string> allnames = GeneralUtils::ParseToStr(name, ",");
  for (uint i = 0; i < allnames.size(); i++) {
    std::string singlename = allnames[i];

    // Get Syst
#if NEUT_VERSION < 541
    neut::rew::NSyst_t gensyst = NSyst::FromString(singlename);
#else
    neut::rew::NSyst_t gensyst = neut::rew::NSyst::FromString(singlename);
#endif

    // Fill Maps
    int index = fValues.size();
    fValues.push_back(0.0);
    fNEUTSysts.push_back(gensyst);

    // Initialize dial
    NUIS_LOG(FIT, "Registering " << singlename << " dial.");
    fNeutRW->Systematics().Init(fNEUTSysts[index]);

    // If Absolute
    if (fIsAbsTwk) {
#if NEUT_VERSION < 541
      NSystUncertainty::Instance()->SetUncertainty(fNEUTSysts[index], 1.0, 1.0);
#else
      neut::rew::NSystUncertainty::Instance()->SetUncertainty(fNEUTSysts[index],
                                                              1.0, 1.0);
#endif
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

void NEUTWeightEngine::SetDialValue(int nuisenum, double val) {
  std::vector<size_t> indices = fEnumIndex[nuisenum];
  for (uint i = 0; i < indices.size(); i++) {
    fValues[indices[i]] = val;
    NUIS_LOG(FIT, "Setting Dial Value for "
                      << nuisenum << " " << i << " " << indices[i] << " "
                      << fValues[indices[i]]
                      << " Enum: " << fNEUTSysts[indices[i]]);
    fNeutRW->Systematics().Set(fNEUTSysts[indices[i]], val);
  }
}

void NEUTWeightEngine::SetDialValue(std::string name, double val) {
  std::vector<size_t> indices = fNameIndex[name];
  for (uint i = 0; i < indices.size(); i++) {
    fValues[indices[i]] = val;
    NUIS_LOG(FIT, "Setting Dial Value for "
                      << name << " = " << i << " " << indices[i] << " "
                      << fValues[indices[i]]
                      << "  Enum: " << fNEUTSysts[indices[i]]);
    fNeutRW->Systematics().Set(fNEUTSysts[indices[i]], val);
  }
}

void NEUTWeightEngine::Reconfigure(bool silent) {
  // Hush now...
  if (silent)
    StopTalking();

  // Reconf
  fNeutRW->Reconfigure();

  // if (LOG_LEVEL(DEB)){
  fNeutRW->Print();
  //	}

  // Shout again
  if (silent)
    StartTalking();
}

double NEUTWeightEngine::CalcWeight(BaseFitEvt *evt) {
  double rw_weight = 1.0;
  // Skip Non NEUT
  if (evt->fType != kNEUT)
    return 1.0;

  // Hush now
  StopTalking();

#ifdef NEUT_BUILTIN_FILL_NEUT_COMMONS
  // Fill NEUT Common blocks
  NEUTUtils::FillNeutCommons(evt->fNeutVect);
#else
  neut::CommonBlockIFace::Get().ReadVect(evt->fNeutVect);
#endif

  // Call Weight calculation
  rw_weight = fNeutRW->CalcWeight();

  // Speak Now
  StartTalking();

  if (!std::isnormal(rw_weight)) {
    NUIS_ERR(WRN, "NEUT returned weight: " << rw_weight);
    rw_weight = 0;
  }

  // Return rw_weight
  return rw_weight;
}
