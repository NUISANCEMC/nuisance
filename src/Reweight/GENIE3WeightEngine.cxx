#include "GENIEWeightEngine.h"

#include "Framework/Algorithm/AlgConfigPool.h"
#include "Framework/EventGen/EventRecord.h"
#include "Framework/GHEP/GHepParticle.h"
#include "Framework/GHEP/GHepRecord.h"
#include "Framework/Ntuple/NtpMCEventRecord.h"
#include "Framework/Utils/AppInit.h"
#include "Framework/Utils/RunOpt.h"

using namespace genie;

#include "RwCalculators/GReWeightAGKY.h"
#include "RwCalculators/GReWeightDISNuclMod.h"
#include "RwCalculators/GReWeightFGM.h"
#include "RwCalculators/GReWeightFZone.h"
#include "RwCalculators/GReWeightINuke.h"
#include "RwCalculators/GReWeightNonResonanceBkg.h"
#include "RwCalculators/GReWeightNuXSecCCQE.h"
#include "RwCalculators/GReWeightNuXSecCCQEaxial.h"
#include "RwCalculators/GReWeightNuXSecCCQEvec.h"
#include "RwCalculators/GReWeightNuXSecCCRES.h"
#include "RwCalculators/GReWeightNuXSecCOH.h"
#include "RwCalculators/GReWeightNuXSecDIS.h"
#include "RwCalculators/GReWeightNuXSecNC.h"
#include "RwCalculators/GReWeightNuXSecNCEL.h"
#include "RwCalculators/GReWeightNuXSecNCRES.h"
#ifdef GENIE3_XSECMEC_ENABLED
#include "RwCalculators/GReWeightXSecMEC.h"
#endif
#include "RwCalculators/GReWeightResonanceDecay.h"
#include "RwCalculators/GReWeightXSecEmpiricalMEC.h"
#include "RwFramework/GSystUncertainty.h"

using namespace genie::rew;

#include "FitLogger.h"

GENIEWeightEngine::GENIEWeightEngine(std::string name) {
  fCalcName = name;
  NUIS_LOG(DEB, "Setting up GENIE RW : " << fCalcName);

  // Create RW Engine suppressing cout
  StopTalking();
  fGenieRW = new genie::rew::GReWeight();

  // Get List of Vetos (Just for debugging)
  std::string rw_engine_list =
      FitPar::Config().GetParS("FitWeight_fGenieRW_veto");
  bool xsec_ncel = rw_engine_list.find("xsec_ncel") == std::string::npos;
  bool xsec_ccqe = rw_engine_list.find("xsec_ccqe") == std::string::npos;
  bool xsec_coh = rw_engine_list.find("xsec_coh") == std::string::npos;
  bool xsec_nnres = rw_engine_list.find("xsec_nonresbkg") == std::string::npos;
  bool xsec_nudis = rw_engine_list.find("nuclear_dis") == std::string::npos;
  bool xsec_resdec =
      rw_engine_list.find("hadro_res_decay") == std::string::npos;
  bool xsec_fzone = rw_engine_list.find("hadro_intranuke") == std::string::npos;
  bool xsec_intra = rw_engine_list.find("hadro_fzone") == std::string::npos;
  bool xsec_agky = rw_engine_list.find("hadro_agky") == std::string::npos;
  bool xsec_qevec = rw_engine_list.find("xsec_ccqe_vec") == std::string::npos;
  bool xsec_dis = rw_engine_list.find("xsec_dis") == std::string::npos;
  bool xsec_nc = rw_engine_list.find("xsec_nc") == std::string::npos;
  bool xsec_ccres = rw_engine_list.find("xsec_ccres") == std::string::npos;
  bool xsec_ncres = rw_engine_list.find("xsec_ncres") == std::string::npos;
  bool xsec_nucqe = rw_engine_list.find("nuclear_qe") == std::string::npos;
  bool xsec_qeaxial =
      rw_engine_list.find("xsec_ccqe_axial") == std::string::npos;

#ifdef GENIE3_XSECMEC_ENABLED
  bool xsec_MEC = rw_engine_list.find("xsec_MEC") == std::string::npos;
#endif

  genie::RunOpt *grunopt = genie::RunOpt::Instance();
  grunopt->EnableBareXSecPreCalc(true);
  grunopt->SetEventGeneratorList(Config::GetParS("GENIEEventGeneratorList"));

  if (!Config::HasPar("GENIETune")) {
    StartTalking();
    NUIS_ABORT(
        "GENIE tune was not specified, this is required when reweighting GENIE "
        "V3+ events. Add a config parameter like: <config "
        "GENIETune=\"G18_10a_02_11a\" /> to your nuisance card.");
  }
  grunopt->SetTuneName(Config::GetParS("GENIETune"));
  grunopt->BuildTune();
  std::string genv =
      std::string(getenv("GENIE")) + "/config/Messenger_laconic.xml";
  genie::utils::app_init::MesgThresholds(genv);

  // Now actually add the RW Calcs
  if (xsec_ncel)
    fGenieRW->AdoptWghtCalc("xsec_ncel", new genie::rew::GReWeightNuXSecNCEL);
  if (xsec_ccqe) {
    fGenieRW->AdoptWghtCalc("xsec_ccqe", new genie::rew::GReWeightNuXSecCCQE);
    //	  (dynamic_cast<GReWeightNuXSecCCQE*> (fGenieRW->WghtCalc("xsec_ccqe")))
    //	    ->SetXSecModel( FitPar::Config().GetParS("GENIEXSecModelCCQE") );
  }

#ifdef GENIE3_XSECMEC_ENABLED
  if (xsec_MEC) {
    fGenieRW->AdoptWghtCalc("xsec_MEC", new genie::rew::GReWeightXSecMEC);
  }
#endif
  if (xsec_coh) {
    fGenieRW->AdoptWghtCalc("xsec_coh", new genie::rew::GReWeightNuXSecCOH());
    //	  (dynamic_cast<GReWeightNuXSecCOH*> (fGenieRW->WghtCalc("xsec_coh")))
    //	    ->SetXSecModel( FitPar::Config().GetParS("GENIEXSecModelCOH") );
  }

  if (xsec_nnres)
    fGenieRW->AdoptWghtCalc("xsec_nonresbkg",
                            new genie::rew::GReWeightNonResonanceBkg);
  if (xsec_nudis)
    fGenieRW->AdoptWghtCalc("nuclear_dis", new genie::rew::GReWeightDISNuclMod);
  if (xsec_resdec)
    fGenieRW->AdoptWghtCalc("hadro_res_decay",
                            new genie::rew::GReWeightResonanceDecay);
  if (xsec_fzone)
    fGenieRW->AdoptWghtCalc("hadro_fzone", new genie::rew::GReWeightFZone);
  if (xsec_intra)
    fGenieRW->AdoptWghtCalc("hadro_intranuke", new genie::rew::GReWeightINuke);
  if (xsec_agky)
    fGenieRW->AdoptWghtCalc("hadro_agky", new genie::rew::GReWeightAGKY);
  if (xsec_qevec)
    fGenieRW->AdoptWghtCalc("xsec_ccqe_vec",
                            new genie::rew::GReWeightNuXSecCCQEvec);
  if (xsec_qeaxial)
    fGenieRW->AdoptWghtCalc("xsec_ccqe_axial",
                            new genie::rew::GReWeightNuXSecCCQEaxial);

  if (xsec_dis)
    fGenieRW->AdoptWghtCalc("xsec_dis", new genie::rew::GReWeightNuXSecDIS);
  if (xsec_nc)
    fGenieRW->AdoptWghtCalc("xsec_nc", new genie::rew::GReWeightNuXSecNC);
  if (xsec_ccres) {
    fGenieRW->AdoptWghtCalc(
        "xsec_ccres",
        new genie::rew::GReWeightNuXSecCCRES(
            FitPar::Config().GetParS("GENIEXSecModelCCRES"), "Default"));
  }

  if (xsec_ncres)
    fGenieRW->AdoptWghtCalc("xsec_ncres", new genie::rew::GReWeightNuXSecNCRES);
  if (xsec_nucqe)
    fGenieRW->AdoptWghtCalc("nuclear_qe", new genie::rew::GReWeightFGM);

  // Set the CCQE reweighting style
  GReWeightNuXSecCCQE *rwccqe =
      dynamic_cast<GReWeightNuXSecCCQE *>(fGenieRW->WghtCalc("xsec_ccqe"));

  // For MaCCQE reweighting
  std::string ccqetype = FitPar::Config().GetParS("GENIEWeightEngine_CCQEMode");
  if (ccqetype == "kModeMa") {
    NUIS_LOG(DEB, "Setting GENIE ReWeight CCQE to kModeMa");
    rwccqe->SetMode(GReWeightNuXSecCCQE::kModeMa);
  } else if (ccqetype == "kModeNormAndMaShape") {
    NUIS_LOG(DEB, "Setting GENIE ReWeight CCQE to kModeNormAndMaShape");
    rwccqe->SetMode(GReWeightNuXSecCCQE::kModeNormAndMaShape);
    // For z-expansion reweighting, only available after 2.10
  } else if (ccqetype == "kModeZExp") {
    NUIS_LOG(DEB, "Setting GENIE ReWeight CCQE to kModeZExp");
    rwccqe->SetMode(GReWeightNuXSecCCQE::kModeZExp);
  } else {
    NUIS_ERR(FTL, "Did not find specified GENIE ReWeight CCQE mode");
    NUIS_ABORT("You provided: " << ccqetype << " in parameters/config.xml");
  }

  if (xsec_ccres) {
    // Default to include shape and normalization changes for CCRES (can be
    // changed downstream if desired)
    GReWeightNuXSecCCRES *rwccres =
        dynamic_cast<GReWeightNuXSecCCRES *>(fGenieRW->WghtCalc("xsec_ccres"));

    std::string marestype =
        FitPar::Config().GetParS("GENIEWeightEngine_CCRESMode");
    if (!marestype.compare("kModeNormAndMaMvShape")) {
      rwccres->SetMode(GReWeightNuXSecCCRES::kModeNormAndMaMvShape);
    } else if (!marestype.compare("kModeMaMv")) {
      rwccres->SetMode(GReWeightNuXSecCCRES::kModeMaMv);
    } else {
      StartTalking();
      NUIS_ABORT("Unkown MARES Mode in GENIE Weight Engine : " << marestype);
    }
  }
  if (xsec_ncres) {
    // Default to include shape and normalization changes for NCRES (can be
    // changed downstream if desired)
    GReWeightNuXSecNCRES *rwncres =
        dynamic_cast<GReWeightNuXSecNCRES *>(fGenieRW->WghtCalc("xsec_ncres"));
    rwncres->SetMode(GReWeightNuXSecNCRES::kModeMaMv);
  }

  if (xsec_dis) {
    // Default to include shape and normalization changes for DIS (can be
    // changed downstream if desired)
    GReWeightNuXSecDIS *rwdis =
        dynamic_cast<GReWeightNuXSecDIS *>(fGenieRW->WghtCalc("xsec_dis"));
    rwdis->SetMode(GReWeightNuXSecDIS::kModeABCV12u);

    // Set Abs Twk Config
    fIsAbsTwk = (FitPar::Config().GetParB("setabstwk"));
  }

  // allow cout again
  StartTalking();
}

void GENIEWeightEngine::IncludeDial(std::string name, double startval) {
  // Get First enum
  int nuisenum = Reweight::ConvDial(name, kGENIE);

  // Setup Maps
  fEnumIndex[nuisenum];  // = std::vector<size_t>(0);
  fNameIndex[name];      // = std::vector<size_t>(0);

  // Split by commas
  std::vector<std::string> allnames = GeneralUtils::ParseToStr(name, ",");
  for (uint i = 0; i < allnames.size(); i++) {
    std::string singlename = allnames[i];

    // Get RW
    genie::rew::GSyst_t rwsyst = GSyst::FromString(singlename);

    // Fill Maps
    int index = fValues.size();
    fValues.push_back(0.0);
    fGENIESysts.push_back(rwsyst);

    // Initialize dial
    NUIS_LOG(DEB, "Registering " << singlename << " from " << name);
    fGenieRW->Systematics().Init(fGENIESysts[index]);

    // If Absolute
    if (fIsAbsTwk) {
      GSystUncertainty::Instance()->SetUncertainty(rwsyst, 1.0, 1.0);
    }

    // Setup index
    fEnumIndex[nuisenum].push_back(index);
    fNameIndex[name].push_back(index);
  }

  // Set Value if given
  if (startval != _UNDEF_DIAL_VALUE_) {
    SetDialValue(nuisenum, startval);
  }
}

void GENIEWeightEngine::SetDialValue(int nuisenum, double val) {
  std::vector<size_t> indices = fEnumIndex[nuisenum];
  for (uint i = 0; i < indices.size(); i++) {
    fValues[indices[i]] = val;
    fGenieRW->Systematics().Set(fGENIESysts[indices[i]], val);
  }
}

void GENIEWeightEngine::SetDialValue(std::string name, double val) {
  std::vector<size_t> indices = fNameIndex[name];
  for (uint i = 0; i < indices.size(); i++) {
    fValues[indices[i]] = val;
    fGenieRW->Systematics().Set(fGENIESysts[indices[i]], val);
  }
}

void GENIEWeightEngine::Reconfigure(bool silent) {
  // Hush now...
  if (silent) StopTalking();

  // Reconf
  fGenieRW->Reconfigure();
  fGenieRW->Print();

  // Shout again
  if (silent) StartTalking();
}

double GENIEWeightEngine::CalcWeight(BaseFitEvt *evt) {
  double rw_weight = 1.0;

  // Make nom weight
  if (!evt) {
    NUIS_ABORT("evt not found : " << evt);
  }

  // Skip Non GENIE
  if (evt->fType != kGENIE) return 1.0;

  if (!(evt->genie_event)) {
    NUIS_ABORT("evt->genie_event not found!" << evt->genie_event);
  }

  if (!(evt->genie_event->event)) {
    NUIS_ABORT("evt->genie_event->event GHepRecord not found!"
               << (evt->genie_event->event));
  }

  if (!fGenieRW) {
    NUIS_ABORT("GENIE RW Not Found!" << fGenieRW);
  }

  rw_weight = fGenieRW->CalcWeight(*(evt->genie_event->event));
  //	std::cout << "Returning GENIE Weight for electron scattering = " <<
  // rw_weight << std::endl;
  // if (rw_weight != 1.0 )std::cout << "mode=" << evt->Mode << " rw_weight = "
  // << rw_weight << std::endl;

  // Return rw_weight
  return rw_weight;
}
