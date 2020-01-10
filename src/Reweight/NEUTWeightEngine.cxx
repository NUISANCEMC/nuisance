#include "NEUTWeightEngine.h"

NEUTWeightEngine::NEUTWeightEngine(std::string name) {
#if defined(__NEUT_ENABLED__) and !defined(__NO_REWEIGHT__)

  // For newer NEUT we need to set up our defaults
#if __NEUT_VERSION__ >= 541
  neut::CommonBlockIFace::Initialize(std::string(std::getenv("NUISANCE"))+"/data/neut/neut_minimal_6t.card");
#endif

  // Setup the NEUT Reweight engien
  fCalcName = name;
  NUIS_LOG(FIT, "Setting up NEUT RW : " << fCalcName);

  // Create RW Engine suppressing cout
  StopTalking();
  fNeutRW = new neut::rew::NReWeight();
  TDirectory *olddir = gDirectory;

  // get list of vetoed calc engines (just for debug really)
  std::string rw_engine_list =
      FitPar::Config().GetParS("FitWeight_fNeutRW_veto");
  bool xsec_ccqe = rw_engine_list.find("xsec_ccqe") == std::string::npos;
  bool xsec_res = rw_engine_list.find("xsec_res") == std::string::npos;
  bool xsec_ccres = rw_engine_list.find("xsec_ccres") == std::string::npos;
  bool xsec_coh = rw_engine_list.find("xsec_coh") == std::string::npos;
  bool xsec_dis = rw_engine_list.find("xsec_dis") == std::string::npos;
  bool xsec_ncel = rw_engine_list.find("xsec_ncel") == std::string::npos;
  bool xsec_nc = rw_engine_list.find("xsec_nc") == std::string::npos;
  bool xsec_ncres = rw_engine_list.find("xsec_ncres") == std::string::npos;
  bool nucl_casc = rw_engine_list.find("nucl_casc") == std::string::npos;
  bool nucl_piless = rw_engine_list.find("nucl_piless") == std::string::npos;

  // Activate each calc engine
  if (xsec_ccqe)
    fNeutRW->AdoptWghtCalc("xsec_ccqe", new neut::rew::NReWeightNuXSecCCQE);
  if (xsec_res)
    fNeutRW->AdoptWghtCalc("xsec_res", new neut::rew::NReWeightNuXSecRES);

  // Dials removed in NEUT 5.4.1
#if __NEUT_VERSION__ < 541
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

#else
  NUIS_ABORT("NEUT RW NOT ENABLED!");
#endif
};

void NEUTWeightEngine::IncludeDial(std::string name, double startval) {
#if defined(__NEUT_ENABLED__) and !defined(__NO_REWEIGHT__)

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
#if __NEUT_VERSION__ < 541
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
#if __NEUT_VERSION__ < 541
      NSystUncertainty::Instance()->SetUncertainty(fNEUTSysts[index], 1.0, 1.0);
#else
      neut::rew::NSystUncertainty::Instance()->SetUncertainty(fNEUTSysts[index], 1.0, 1.0);
#endif
    }

    // Setup index
    fEnumIndex[nuisenum].push_back(index);
    fNameIndex[name].push_back(index);
  }

  // Set Value if given
  if (startval != -999.9) {
    SetDialValue(nuisenum, startval);
  }
#endif
}

void NEUTWeightEngine::SetDialValue(int nuisenum, double val) {
#if defined(__NEUT_ENABLED__) and !defined(__NO_REWEIGHT__)
  std::vector<size_t> indices = fEnumIndex[nuisenum];
  for (uint i = 0; i < indices.size(); i++) {
    fValues[indices[i]] = val;
    NUIS_LOG(FIT, "Setting Dial Value for "
                      << nuisenum << " " << i << " " << indices[i] << " "
                      << fValues[indices[i]]
                      << " Enum: " << fNEUTSysts[indices[i]]);
    fNeutRW->Systematics().Set(fNEUTSysts[indices[i]], val);
  }
#endif
}

void NEUTWeightEngine::SetDialValue(std::string name, double val) {
#if defined(__NEUT_ENABLED__) and !defined(__NO_REWEIGHT__)
  std::vector<size_t> indices = fNameIndex[name];
  for (uint i = 0; i < indices.size(); i++) {
    fValues[indices[i]] = val;
    NUIS_LOG(FIT, "Setting Dial Value for "
                      << name << " = " << i << " " << indices[i] << " "
                      << fValues[indices[i]]
                      << "  Enum: " << fNEUTSysts[indices[i]]);
    fNeutRW->Systematics().Set(fNEUTSysts[indices[i]], val);
  }
#endif
}

void NEUTWeightEngine::Reconfigure(bool silent) {
#if defined(__NEUT_ENABLED__) and !defined(__NO_REWEIGHT__)
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
#endif
}

double NEUTWeightEngine::CalcWeight(BaseFitEvt *evt) {
  double rw_weight = 1.0;

#if defined(__NEUT_ENABLED__) and !defined(__NO_REWEIGHT__)
  // Skip Non NEUT
  if (evt->fType != kNEUT)
    return 1.0;

  // Hush now
  StopTalking();

  // Fill NEUT Common blocks
  NEUTUtils::FillNeutCommons(evt->fNeutVect);

  // Call Weight calculation
  rw_weight = fNeutRW->CalcWeight();

  // Speak Now
  StartTalking();
#endif

  // Return rw_weight
  return rw_weight;
}
