#include "NIWGWeightEngine.h"

NIWGWeightEngine::NIWGWeightEngine(std::string name) {
  // Setup the NEUT Reweight engien
  fCalcName = name;
  NUIS_LOG(FIT, "Setting up NIWG RW : " << fCalcName);

  // Create RW Engine suppressing cout
  StopTalking();
  fNIWGRW = new niwg::rew::NIWGReWeight();

  // Get List of Veto Calcs (For Debugging)
  std::string rw_engine_list =
      FitPar::Config().GetParS("FitWeight_fNIWGRW_veto");

  bool niwg_2012a = rw_engine_list.find("niwg_2012a") == std::string::npos;
  bool niwg_2014a = rw_engine_list.find("niwg_2014a") == std::string::npos;
  bool niwg_pimult = rw_engine_list.find("niwg_pimult") == std::string::npos;
  bool niwg_mec = rw_engine_list.find("niwg_mec") == std::string::npos;
  bool niwg_rpa = rw_engine_list.find("niwg_rpa") == std::string::npos;
  bool niwg_eff_rpa = rw_engine_list.find("niwg_eff_rpa") == std::string::npos;
  bool niwg_proton =
      rw_engine_list.find("niwg_protonFSIbug") == std::string::npos;
  bool niwg_hadron =
      rw_engine_list.find("niwg_HadronMultSwitch") == std::string::npos;
  bool niwg_qelowq2 = rw_engine_list.find("niwg_LowQEQ2") == std::string::npos;
  bool niwg_2p2henu = rw_engine_list.find("niwg_2p2hEnu") == std::string::npos;

  // Add the RW Calcs
  if (niwg_2012a)
    fNIWGRW->AdoptWghtCalc("niwg_2012a", new niwg::rew::NIWGReWeight2012a);
  if (niwg_2014a)
    fNIWGRW->AdoptWghtCalc("niwg_2014a", new niwg::rew::NIWGReWeight2014a);
  if (niwg_pimult)
    fNIWGRW->AdoptWghtCalc("niwg_pimult", new niwg::rew::NIWGReWeightPiMult);
  if (niwg_mec)
    fNIWGRW->AdoptWghtCalc("niwg_mec", new niwg::rew::NIWGReWeightMEC);
  if (niwg_rpa)
    fNIWGRW->AdoptWghtCalc("niwg_rpa", new niwg::rew::NIWGReWeightRPA);
  if (niwg_eff_rpa)
    fNIWGRW->AdoptWghtCalc("niwg_eff_rpa",
                           new niwg::rew::NIWGReWeightEffectiveRPA);
  if (niwg_proton)
    fNIWGRW->AdoptWghtCalc("niwg_protonFSIbug",
                           new niwg::rew::NIWGReWeightProtonFSIbug);
  if (niwg_hadron)
    fNIWGRW->AdoptWghtCalc("niwg_HadronMultSwitch",
                           new niwg::rew::NIWGReWeightHadronMultSwitch);
// Add in Luke's low Q2 suppression
#ifdef NIWGRW_HAVE_LOWQ2
  if (niwg_qelowq2)
    fNIWGRW->AdoptWghtCalc(
        "niwg_QELowQ2", new niwg::rew::NIWGReWeightEffectiveQELowQ2Suppression);
#endif

#ifdef NIWGRW_HAVE_RESLOWQ2
  // Add in the low Q2 correction through T2KReWeight
  // N.B. this already exists in NUISANCE's MINERvA weights
  fNIWGRW->AdoptWghtCalc("niwg_NIWGReWeightSPPLowQ2Suppression", new niwg::rew::NIWGReWeightSPPLowQ2Suppression);
#endif

// Add in Kevin and Laura's 2p2h Enu dependent dial
#ifdef NIWGRW_HAVE_2P2HENU
  if (niwg_2p2henu)
    fNIWGRW->AdoptWghtCalc(
        "niwg_2p2enu", new niwg::rew::NIWGReWeight2p2hEdep);
#endif

  fNIWGRW->Reconfigure();

  // Set Abs Twk Config
  fIsAbsTwk = (FitPar::Config().GetParB("setabstwk"));

  // allow cout again
  StartTalking();
};

void NIWGWeightEngine::IncludeDial(std::string name, double startval) {
  // Get First enum
  int nuisenum = Reweight::ConvDial(name, kNIWG);

  // Setup Maps
  fEnumIndex[nuisenum]; // = std::vector<size_t>(0);
  fNameIndex[name];     // = std::vector<size_t>(0);

  // Split by commas
  std::vector<std::string> allnames = GeneralUtils::ParseToStr(name, ",");
  for (uint i = 0; i < allnames.size(); i++) {
    std::string singlename = allnames[i];

    // Get RW
    niwg::rew::NIWGSyst_t gensyst = niwg::rew::NIWGSyst::FromString(name);

    // Fill Maps
    int index = fValues.size();
    fValues.push_back(0.0);
    fNIWGSysts.push_back(gensyst);

    // Initialize dial
    NUIS_LOG(FIT, "Registering " << singlename << " from " << name);
    fNIWGRW->Systematics().Init(fNIWGSysts[index]);

    // If Absolute
    if (fIsAbsTwk) {
      niwg::rew::NIWGSystUncertainty::Instance()->SetUncertainty(
          fNIWGSysts[index], 1.0, 1.0);
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

void NIWGWeightEngine::SetDialValue(int nuisenum, double val) {
  std::vector<size_t> indices = fEnumIndex[nuisenum];
  for (uint i = 0; i < indices.size(); i++) {
    fValues[indices[i]] = val;
    NUIS_LOG(FIT, "Setting NIWG Dial number " << i << " to index " << indices[i] << " " << fNIWGSysts[indices[i]] << " to value " << val);
    fNIWGRW->Systematics().Set(fNIWGSysts[indices[i]], val);
  }
}

void NIWGWeightEngine::SetDialValue(std::string name, double val) {
  std::vector<size_t> indices = fNameIndex[name];
  for (uint i = 0; i < indices.size(); i++) {
    NUIS_LOG(FIT, "Setting NIWG Dial Name " << name << " with number " << i << " to index " << indices[i] << " " << fNIWGSysts[indices[i]] << " to value " << val);
    fValues[indices[i]] = val;
    fNIWGRW->Systematics().Set(fNIWGSysts[indices[i]], val);
  }
}

void NIWGWeightEngine::Reconfigure(bool silent) {
  // Hush now...
  if (silent)
    StopTalking();

  // Reconf
  fNIWGRW->Reconfigure();

  // Shout again
  if (silent)
    StartTalking();
}

double NIWGWeightEngine::CalcWeight(BaseFitEvt *evt) {
  double rw_weight = 1.0;


  // Skip Non GENIE
  if (evt->fType != kNEUT)
    return 1.0;

  // Hush now
  StopTalking();

  niwg::rew::NIWGEvent *niwg_event =
      NIWGWeightEngine::GetNIWGEventLocal(evt->fNeutVect);
  rw_weight *= fNIWGRW->CalcWeight(*niwg_event);
  delete niwg_event;

  // Speak Now
  StartTalking();

  // Return rw_weight
  return rw_weight;
}

niwg::rew::NIWGEvent *NIWGWeightEngine::GetNIWGEventLocal(NeutVect *nvect) {
  niwg::rew::NIWGEvent *fDummyNIWGEvent = NULL;

  fDummyNIWGEvent = new niwg::rew::NIWGEvent();
  fDummyNIWGEvent->detid = 1; // MiniBooNE (apply CCQE LowE variations)
  fDummyNIWGEvent->neutmode = nvect->Mode;
  fDummyNIWGEvent->targetA = nvect->TargetA;
  fDummyNIWGEvent->recenu_ccqe_sk = -1;
  if (nvect->Ibound == 0)
    fDummyNIWGEvent->targetA = 1; // RT: identifies as H, rather than O16

  // Fill initial particle stack
  for (int ip = 0; ip < nvect->Npart(); ++ip) {

    niwg::rew::NIWGPartStack fDummyPartStack;

    fDummyPartStack.p = (nvect->PartInfo(ip)->fP) * 0.001; // Convert to GeV

    fDummyPartStack.pdg = nvect->PartInfo(ip)->fPID;
    fDummyPartStack.chase = nvect->PartInfo(ip)->fIsAlive;
    fDummyPartStack.parent =
        nvect->ParentIdx(ip) -
        1; // WARNING: this needs to be tested with a NeutRoot file

    fDummyNIWGEvent->part_stack.push_back(fDummyPartStack);
  }
  fDummyNIWGEvent->CalcKinematics();

  return fDummyNIWGEvent;
}
