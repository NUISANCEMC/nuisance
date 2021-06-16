#include "GENIEWeightEngine.h"

#ifdef __GENIE_ENABLED__
#ifdef GENIE_PRE_R3
#include "Algorithm/AlgConfigPool.h"
#include "EVGCore/EventRecord.h"
#include "GHEP/GHepRecord.h"
#include "Ntuple/NtpMCEventRecord.h"
#ifndef __NO_REWEIGHT__
#include "ReWeight/GReWeightAGKY.h"
#include "ReWeight/GReWeightDISNuclMod.h"
#include "ReWeight/GReWeightFGM.h"
#include "ReWeight/GReWeightFZone.h"
#include "ReWeight/GReWeightINuke.h"
#include "ReWeight/GReWeightNonResonanceBkg.h"
#include "ReWeight/GReWeightNuXSecCCQE.h"
#include "ReWeight/GReWeightNuXSecCCQEvec.h"
#include "ReWeight/GReWeightNuXSecCCRES.h"
#include "ReWeight/GReWeightNuXSecCOH.h"
#include "ReWeight/GReWeightNuXSecDIS.h"
#include "ReWeight/GReWeightNuXSecNC.h"
#include "ReWeight/GReWeightNuXSecNCEL.h"
#include "ReWeight/GReWeightNuXSecNCRES.h"
#include "ReWeight/GReWeightResonanceDecay.h"
#include "ReWeight/GSystUncertainty.h"
#ifdef __GENIE_EMP_MECRW_ENABLED
#include "ReWeight/GReWeightXSecEmpiricalMEC.h"
#endif
#endif
#if __GENIE_VERSION__ >= 212
#include "ReWeight/GReWeightNuXSecCCQEaxial.h"
#endif
#else // GENIE v3
#include "Framework/Algorithm/AlgConfigPool.h"
#include "Framework/EventGen/EventRecord.h"
#include "Framework/GHEP/GHepParticle.h"
#include "Framework/GHEP/GHepRecord.h"
#include "Framework/Ntuple/NtpMCEventRecord.h"
#include "Framework/Utils/AppInit.h"
#include "Framework/Utils/RunOpt.h"

using namespace genie;

#ifndef __NO_REWEIGHT__
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
#ifdef USE_GENIE_XSECMEC
#include "RwCalculators/GReWeightXSecMEC.h"
#endif
#include "RwCalculators/GReWeightResonanceDecay.h"
#include "RwCalculators/GReWeightXSecEmpiricalMEC.h"
#include "RwFramework/GSystUncertainty.h"
using namespace genie::rew;
#endif
#endif
#endif

#include "FitLogger.h"

GENIEWeightEngine::GENIEWeightEngine(std::string name) {
#ifdef __GENIE_ENABLED__
#ifndef __NO_REWEIGHT__
  // Setup the NEUT Reweight engien
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
#ifdef __GENIE_EMP_MECRW_ENABLED
  bool xsec_empMEC = rw_engine_list.find("xsec_empMEC") == std::string::npos;
#endif
#ifdef USE_GENIE_XSECMEC
  bool xsec_MEC = rw_engine_list.find("xsec_MEC") == std::string::npos;
#endif

#ifndef GENIE_PRE_R3
  genie::RunOpt *grunopt = genie::RunOpt::Instance();
  grunopt->EnableBareXSecPreCalc(true);
  grunopt->SetEventGeneratorList(Config::GetParS("GENIEEventGeneratorList"));
  if (!Config::HasPar("GENIETune")) {
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
#endif

  // Now actually add the RW Calcs
  if (xsec_ncel)
    fGenieRW->AdoptWghtCalc("xsec_ncel", new genie::rew::GReWeightNuXSecNCEL);
  if (xsec_ccqe) {
    fGenieRW->AdoptWghtCalc("xsec_ccqe", new genie::rew::GReWeightNuXSecCCQE);
    //	  (dynamic_cast<GReWeightNuXSecCCQE*> (fGenieRW->WghtCalc("xsec_ccqe")))
    //	    ->SetXSecModel( FitPar::Config().GetParS("GENIEXSecModelCCQE") );
  }
#ifdef __GENIE_EMP_MECRW_ENABLED
  if (xsec_empMEC) {
    fGenieRW->AdoptWghtCalc("xsec_empMEC",
                            new genie::rew::GReWeightXSecEmpiricalMEC);
  }
#endif
#ifdef USE_GENIE_XSECMEC
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
#if __GENIE_VERSION__ >= 212
  if (xsec_qeaxial)
    fGenieRW->AdoptWghtCalc("xsec_ccqe_axial",
                            new genie::rew::GReWeightNuXSecCCQEaxial);
#endif

  if (xsec_dis)
    fGenieRW->AdoptWghtCalc("xsec_dis", new genie::rew::GReWeightNuXSecDIS);
  if (xsec_nc)
    fGenieRW->AdoptWghtCalc("xsec_nc", new genie::rew::GReWeightNuXSecNC);
  if (xsec_ccres) {
#if __GENIE_VERSION__ < 213
    fGenieRW->AdoptWghtCalc("xsec_ccres", new genie::rew::GReWeightNuXSecCCRES);
#else
    fGenieRW->AdoptWghtCalc(
        "xsec_ccres",
        new genie::rew::GReWeightNuXSecCCRES(
            FitPar::Config().GetParS("GENIEXSecModelCCRES"), "Default"));
#endif
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
#if __GENIE_VERSION__ >= 210 
  } else if (ccqetype == "kModeZExp") {
    NUIS_LOG(DEB, "Setting GENIE ReWeight CCQE to kModeZExp");
    rwccqe->SetMode(GReWeightNuXSecCCQE::kModeZExp);
#endif
  } else {
    NUIS_ERR(FTL, "Did not find specified GENIE ReWeight CCQE mode");
    NUIS_ABORT("You provided: " << ccqetype << " in parameters/config.xml");
  }

#if (__GENIE_VERSION__ >= 212) and                                             \
    (__GENIE_VERSION__ <=                                                      \
     300) // This doesn't currently work as is for GENIE v3, but the reweighting
          // in v3 supposedly does similar checks anyway.

  // Check the UserPhysicsOptions too!
  AlgConfigPool *Pool = genie::AlgConfigPool::Instance();
  Registry *full = Pool->GlobalParameterList();

  std::string name_ax = full->GetAlg("AxialFormFactorModel").name;
  std::string config_ax = full->GetAlg("AxialFormFactorModel").config;

  if (name_ax == "genie::DipoleAxialFormFactorModel" &&
      ccqetype == "kModeZExp") {
    NUIS_ERR(
        FTL,
        "Trying to run Z Expansion reweighting with Llewelyn-Smith model.");
    NUIS_ERR(FTL, "Please check your "
                      << std::getenv("GENIE")
                      << "/config/UserPhysicsOptions.xml to match generated");
    NUIS_ERR(FTL, "You're telling me " << name_ax << "/" << config_ax);
    NUIS_ABORT("Also check your "
               << std::getenv("NUISANCE")
               << "/parameters/config.xml GENIEWeightEngine_CCQEMode: "
               << ccqetype);
  }

  if (name_ax == "genie::ZExpAxialFormFactorModel" && ccqetype != "kModeZExp") {
    NUIS_ERR(
        FTL,
        "Trying to run Llewelyn-Smith reweighting with Z Expansion model.");
    NUIS_ERR(FTL, "Please change your "
                      << std::getenv("GENIE")
                      << "/config/UserPhysicsOptions.xml to match generated");
    NUIS_ERR(FTL, "You're telling me " << name_ax << "/" << config_ax);
    NUIS_ABORT("Also check your "
               << std::getenv("NUISANCE")
               << "/parameters/config.xml GENIEWeightEngine_CCQEMode: "
               << ccqetype);
  }

  std::string name_qelcc =
      full->GetAlg("XSecModel@genie::EventGenerator/QEL-CC").name;
  std::string config_qelcc =
      full->GetAlg("XSecModel@genie::EventGenerator/QEL-CC").config;
  if (config_qelcc == "Default" && ccqetype == "kModeZExp") {
    NUIS_ERR(
        FTL,
        "Trying to run Z Expansion reweighting with Llewelyn-Smith model.");
    NUIS_ERR(FTL, "Please change your "
                      << std::getenv("GENIE")
                      << "/config/UserPhysicsOptions.xml to match generated");
    NUIS_ERR(FTL, "You're telling me " << name_qelcc << "/" << config_qelcc);
    NUIS_ABORT("Also check your "
               << std::getenv("NUISANCE")
               << "/parameters/config.xml GENIEWeightEngine_CCQEMode: "
               << ccqetype);
  }

  if (config_qelcc == "ZExp" && ccqetype != "kModeZExp") {
    NUIS_ERR(
        FTL,
        "Trying to run Llewelyn-Smith reweighting with Z Expansion model.");
    NUIS_ERR(FTL, "Please change your "
                      << std::getenv("GENIE")
                      << "/config/UserPhysicsOptions.xml to match generated");
    NUIS_ERR(FTL, "You're telling me " << name_qelcc << "/" << config_qelcc);
    NUIS_ABORT("Also check your "
               << std::getenv("NUISANCE")
               << "/parameters/config.xml GENIEWeightEngine_CCQEMode: "
               << ccqetype);
  }
#endif

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

#else
  NUIS_ERR(FTL,
           "GENIE ReWeight is __NOT ENABLED__ in GENIE and you're trying to "
           "run NUISANCE with it enabled");
  NUIS_ERR(FTL, "Check your genie-config --libs for reweighting");
  NUIS_ERR(FTL, "You might also have run with -DUSE_REWEIGHT=0 (false) in your cmake build?");
  NUIS_ERR(FTL, "If not present you need to recompile GENIE");
  NUIS_ABORT("If present you need to contact NUISANCE authors");

#endif
#endif
};

void GENIEWeightEngine::IncludeDial(std::string name, double startval) {
#ifdef __GENIE_ENABLED__
#ifndef __NO_REWEIGHT__

  // Get First enum
  int nuisenum = Reweight::ConvDial(name, kGENIE);

  // Check ZExp sillyness in GENIE
  // If ZExpansion parameters are used we need to set a different mode in GENIE
  // ReWeight... GENIE doesn't have a setter either...
#if (__GENIE_VERSION__ >= 212) and (__GENIE_VERSION__ <= 300)
  std::string ccqetype = FitPar::Config().GetParS("GENIEWeightEngine_CCQEMode");
  if (ccqetype != "kModeZExp" &&
      (name == "ZExpA1CCQE" || name == "ZExpA2CCQE" || name == "ZExpA3CCQE" ||
       name == "ZExpA4CCQE")) {
    NUIS_ERR(FTL, "Found a Z-expansion parameter in GENIE although the GENIE "
                  "ReWeighting engine is set to use Llewelyn-Smith and MaQE!");
    NUIS_ABORT("Change your GENIE UserPhysicsOptions.xml in "
               << std::getenv("GENIE")
               << "/config/UserPhysicsOptions.xml to match requirements");
  }

  if ((ccqetype != "kModeMa" && ccqetype != "kModeMaNormAndMaShape") &&
      (name == "MaCCQE")) {
    NUIS_ERR(FTL, "Found MaCCQE parameter in GENIE although the GENIE "
                  "ReWeighting engine is set to not use this!");
    NUIS_ABORT("Change your GENIE UserPhysicsOptions.xml in "
               << std::getenv("GENIE")
               << "/config/UserPhysicsOptions.xml to match requirements");
  }
#endif

  // Setup Maps
  fEnumIndex[nuisenum]; // = std::vector<size_t>(0);
  fNameIndex[name];     // = std::vector<size_t>(0);

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
#endif
#endif
}

void GENIEWeightEngine::SetDialValue(int nuisenum, double val) {
#ifdef __GENIE_ENABLED__
#ifndef __NO_REWEIGHT__
  std::vector<size_t> indices = fEnumIndex[nuisenum];
  for (uint i = 0; i < indices.size(); i++) {
    fValues[indices[i]] = val;
    fGenieRW->Systematics().Set(fGENIESysts[indices[i]], val);
  }
#endif
#endif
}

void GENIEWeightEngine::SetDialValue(std::string name, double val) {
#ifdef __GENIE_ENABLED__
#ifndef __NO_REWEIGHT__
  std::vector<size_t> indices = fNameIndex[name];
  for (uint i = 0; i < indices.size(); i++) {
    fValues[indices[i]] = val;
    fGenieRW->Systematics().Set(fGENIESysts[indices[i]], val);
  }
#endif
#endif
}

void GENIEWeightEngine::Reconfigure(bool silent) {
#ifdef __GENIE_ENABLED__
#ifndef __NO_REWEIGHT__
  // Hush now...
  if (silent)
    StopTalking();

  // Reconf
  fGenieRW->Reconfigure();
  fGenieRW->Print();

  // Shout again
  if (silent)
    StartTalking();
#endif
#endif
}

double GENIEWeightEngine::CalcWeight(BaseFitEvt *evt) {
  double rw_weight = 1.0;

#ifdef __GENIE_ENABLED__
#ifndef __NO_REWEIGHT__
  // Make nom weight
  if (!evt) {
    NUIS_ABORT("evt not found : " << evt);
  }

  // Skip Non GENIE
  if (evt->fType != kGENIE)
    return 1.0;

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
#endif
#endif

  // Return rw_weight
  return rw_weight;
}
