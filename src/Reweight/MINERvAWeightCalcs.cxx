#ifdef __MINERVA_RW_ENABLED__
#ifdef __GENIE_ENABLED__

#include "MINERvAWeightCalcs.h"
#include "BaseFitEvt.h"

namespace nuisance {
namespace reweight {

//*******************************************************
MINERvAReWeight_MEC::MINERvAReWeight_MEC() {
  //*******************************************************
  fTwk_NormCCMEC = 0.0;
  fDef_NormCCMEC = 1.0;
  fCur_NormCCMEC = fDef_NormCCMEC;
}

MINERvAReWeight_MEC::~MINERvAReWeight_MEC(){};

double MINERvAReWeight_MEC::CalcWeight(BaseFitEvt* evt) {
  // Check GENIE
  if (evt->fType != kGENIE) return 1.0;

  // Extract the GENIE Record
  GHepRecord* ghep = static_cast<GHepRecord*>(evt->genie_event->event);
  const Interaction* interaction = ghep->Summary();
  const InitialState& init_state = interaction->InitState();
  const ProcessInfo& proc_info = interaction->ProcInfo();
  const Target& tgt = init_state.Tgt();

  // If the event is not MEC this Calc doesn't handle it
  if (!proc_info.IsMEC()) return 1.0;

  // WEIGHT CALCULATIONS -------------
  double w = 1.0;

  // CCMEC Dial
  if (!proc_info.IsWeakCC()) w *= fCur_NormCCMEC;

  // Return Combined Weight
  return w;
}

void MINERvAReWeight_MEC::SetDialValue(std::string name, double val) {
  SetDialValue(Reweight::ConvDial(name, kCUSTOM), val);
}

void MINERvAReWeight_MEC::SetDialValue(int rwenum, double val) {
  // Check Handled
  int curenum = rwenum % 1000;
  if (!IsHandled(curenum)) return;

  // Set Values
  if (curenum == kMINERvARW_NormCCMEC) {
    fTwk_NormCCMEC = val;
    fCur_NormCCMEC = fDef_NormCCMEC + fTwk_NormCCMEC;
  }

  // Define Tweaked
  fTweaked = ((fTwk_NormCCMEC != 0.0));
}

bool MINERvAReWeight_MEC::IsHandled(int rwenum) {
  int curenum = rwenum % 1000;

  switch (curenum) {
    case kMINERvARW_NormCCMEC:
      return true;
    default:
      return false;
  }
}

//*******************************************************
MINERvAReWeight_RES::MINERvAReWeight_RES() {
  //*******************************************************
  fTwk_NormCCRES = 0.0;
  fDef_NormCCRES = 1.0;
  fCur_NormCCRES = fDef_NormCCRES;
}

MINERvAReWeight_RES::~MINERvAReWeight_RES(){};

double MINERvAReWeight_RES::CalcWeight(BaseFitEvt* evt) {

  // std::cout << "Caculating RES" << std::endl;
  // Check GENIE
  if (evt->fType != kGENIE) return 1.0;

  // Extract the GENIE Record
  GHepRecord* ghep = static_cast<GHepRecord*>(evt->genie_event->event);
  const Interaction* interaction = ghep->Summary();
  const InitialState& init_state = interaction->InitState();
  const ProcessInfo& proc_info = interaction->ProcInfo();
  const Target& tgt = init_state.Tgt();

  // If the event is not RES this Calc doesn't handle it
  if (!proc_info.IsResonant()) return 1.0;

  // WEIGHT CALCULATIONS -------------
  double w = 1.0;

  // CCRES Dial
  if (proc_info.IsWeakCC()) w *= fCur_NormCCRES;

  // Return Combined Weight
  return w;
}

void MINERvAReWeight_RES::SetDialValue(std::string name, double val) {
  SetDialValue(Reweight::ConvDial(name, kCUSTOM), val);
}

void MINERvAReWeight_RES::SetDialValue(int rwenum, double val) {
  // Check Handled
  int curenum = rwenum % 1000;
  if (!IsHandled(curenum)) return;

  // Set Values
  if (curenum == kMINERvARW_NormCCRES) {
    fTwk_NormCCRES = val;
    fCur_NormCCRES = fDef_NormCCRES + fTwk_NormCCRES;
  }

  // Define Tweaked
  fTweaked = ((fTwk_NormCCRES != 0.0));
}

bool MINERvAReWeight_RES::IsHandled(int rwenum) {
  int curenum = rwenum % 1000;

  switch (curenum) {
    case kMINERvARW_NormCCRES:
      return true;
    default:
      return false;
  }
}

//*******************************************************
RikRPA::RikRPA() {
  //*******************************************************

  // - Syst : kMINERvA_RikRPA_ApplyRPA
  // - Type : Binary
  // - Limits : 0.0 (false) -> 1.0 (true)
  // - Default : 0.0
  fApplyDial_RPACorrection = false;

  // - Syst : kMINERvA_RikRPA_LowQ2
  // - Type : Absolute
  // - Limits : 1.0 -> 1.0
  // - Default : 0.0
  // - Frac Error : 100%
  fDefDial_RPALowQ2 = 0.0;
  fCurDial_RPALowQ2 = fDefDial_RPALowQ2;
  fErrDial_RPALowQ2 = 0.0;

  // - Syst : kMINERvA_RikRPA_HighQ2
  // - Type : Absolute
  // - Limits : 1.0 -> 1.0
  // - Default : 0.0
  // - Frac Error : 100%
  fDefDial_RPAHighQ2 = 0.0;
  fCurDial_RPAHighQ2 = fDefDial_RPAHighQ2;
  fErrDial_RPAHighQ2 = 1.0;

  fEventWeights = new double[5];

  for (size_t i = 0; i < kMaxCalculators; i++) {
    // fRPACalculators[i] = NULL;
  }

  fTweaked = false;
}

RikRPA::~RikRPA() {
  delete fEventWeights;

  for (size_t i = 0; i < kMaxCalculators; i++) {
    // if (fRPACalculators[i]) delete fRPACalculators[i];
    // fRPACalculators[i] = NULL;
  }
}

double RikRPA::CalcWeight(BaseFitEvt* evt) {
  // LOG(FIT) << "Calculating RikRPA" << std::endl;
  // Return 1.0 if not tweaked
  if (!fTweaked) return 1.0;

  double w = 1.0;

  // Extract the GENIE Record
  GHepRecord* ghep = static_cast<GHepRecord*>(evt->genie_event->event);
  const Interaction* interaction = ghep->Summary();
  const InitialState& init_state = interaction->InitState();
  const ProcessInfo& proc_info = interaction->ProcInfo();
  // const Kinematics &   kine       = interaction->Kine();
  // const XclsTag &      xcls       = interaction->ExclTag();
  const Target& tgt = init_state.Tgt();

  // If not QE return 1.0
  // LOG(FIT) << "RikRPA : Event QE = " << proc_info.IsQuasiElastic() <<
  // std::endl;
  if (!tgt.IsNucleus()) return 1.0;
  if (!proc_info.IsQuasiElastic()) return 1.0;

  // Extract Beam and Target PDG
  GHepParticle* neutrino = ghep->Probe();
  int bpdg = neutrino->Pdg();

  GHepParticle* target = ghep->Particle(1);
  assert(target);
  int tpdg = target->Pdg();

  // Find the enum we need
  int calcenum = GetRPACalcEnum(bpdg, tpdg);
  if (calcenum == -1) return 1.0;

  // Check we have the RPA Calc setup for this enum
  // if not, set it up at that point
  // if (!fRPACalculators[calcenum]) SetupRPACalculator(calcenum);
  // weightRPA* rpacalc = fRPACalculators[calcenum];
  // if (!rpacalc) {
  // THROW("Failed to grab the RPA Calculator : " << calcenum);
  // }

  // Extract Q0-Q3
  GHepParticle* fsl = ghep->FinalStatePrimaryLepton();
  const TLorentzVector& k1 = *(neutrino->P4());
  const TLorentzVector& k2 = *(fsl->P4());
  double q0 = fabs((k1 - k2).E());
  double q3 = fabs((k1 - k2).Vect().Mag());

  // Now use q0-q3 and RPA Calculator to fill fWeights
  // LOG(FIT) << "Getting Weights = " << q0 << " " << q3 << std::endl;
  // rpacalc->getWeight(q0, q3, fEventWeights);

  // Apply Interpolation (for the time being simple linear)

  // Syst Application : kMINERvA_RikRPA_ApplyRPA
  if (fApplyDial_RPACorrection) {
    w *= fEventWeights[0];  // CV
  }

  LOG(FIT) << " fCurDial_RPALowQ2  = " << fCurDial_RPALowQ2
           << " fCurDial_RPAHighQ2 = " << fCurDial_RPAHighQ2 << " Weights "
           << fEventWeights[0] << " " << fEventWeights[1] << " "
           << fEventWeights[2] << " " << fEventWeights[3] << " "
           << fEventWeights[4] << std::endl;

  // Syst Application : kMINERvA_RikRPA_LowQ2
  if (fabs(fCurDial_RPALowQ2) > 0.0) {
    double interpw = fEventWeights[0];

    if (fCurDial_RPALowQ2 > 0.0) {
      interpw = fEventWeights[0] - (fEventWeights[0] - fEventWeights[1]) *
                                       fCurDial_RPALowQ2;  // WLow+    } else if
    } else if (fCurDial_RPALowQ2 < 0.0) {
      interpw = fEventWeights[0] - (fEventWeights[2] - fEventWeights[0]) *
                                       fCurDial_RPALowQ2;  // WLow-
    }
    w *= interpw / fEventWeights[0];  // Div by CV again
  }

  // Syst Application : kMINERvA_RikRPA_HighQ2
  if (fabs(fCurDial_RPAHighQ2) > 0.0) {
    double interpw = fEventWeights[0];
    if (fCurDial_RPAHighQ2 > 0.0) {
      interpw = fEventWeights[0] - (fEventWeights[0] - fEventWeights[3]) *
                                       fCurDial_RPAHighQ2;  // WHigh+    } else
      if (fCurDial_RPAHighQ2 < 0.0) {
        interpw = fEventWeights[0] - (fEventWeights[4] - fEventWeights[0]) *
                                         fCurDial_RPAHighQ2;  // WHigh-
      }
      w *= interpw / fEventWeights[0];  // Div by CV again
    }
  }

  // LOG(FIT) << "RPA Weight = " << w << std::endl;
  return w;
}  // namespace reweight

void RikRPA::SetDialValue(std::string name, double val) {
  SetDialValue(Reweight::ConvDial(name, kCUSTOM), val);
}

void RikRPA::SetDialValue(int rwenum, double val) {
  int curenum = rwenum % 1000;

  // Check Handled
  if (!IsHandled(curenum)) return;
  if (curenum == kMINERvARW_RikRPA_ApplyRPA)
    fApplyDial_RPACorrection = (val > 0.5);
  if (curenum == kMINERvARW_RikRPA_LowQ2) fCurDial_RPALowQ2 = val;
  if (curenum == kMINERvARW_RikRPA_HighQ2) fCurDial_RPAHighQ2 = val;

  // Assign flag to say stuff has changed
  fTweaked = (fApplyDial_RPACorrection ||
              fabs(fCurDial_RPAHighQ2 - fDefDial_RPAHighQ2) > 0.0 ||
              fabs(fCurDial_RPALowQ2 - fDefDial_RPALowQ2) > 0.0);
}

bool RikRPA::IsHandled(int rwenum) {
  int curenum = rwenum % 1000;
  switch (curenum) {
    case kMINERvARW_RikRPA_ApplyRPA:
      return true;
    case kMINERvARW_RikRPA_LowQ2:
      return true;
    case kMINERvARW_RikRPA_HighQ2:
      return true;
    default:
      return false;
  }
}

void RikRPA::SetupRPACalculator(int calcenum) {
  std::string rwdir = FitPar::GetDataBase() + "/reweight/MINERvA/RikRPA/";
  std::string fidir = "";
  switch (calcenum) {
    case kNuMuC12:
      fidir = "outNievesRPAratio-nu12C-20GeV-20170202.root";
      break;

    case kNuMuO16:
      fidir = "outNievesRPAratio-nu16O-20GeV-20170202.root";
      break;

    case kNuMuAr40:
      fidir = "outNievesRPAratio-nu40Ar-20GeV-20170202.root";
      break;

    case kNuMuCa40:
      fidir = "outNievesRPAratio-nu40Ca-20GeV-20170202.root";
      break;

    case kNuMuFe56:
      fidir = "outNievesRPAratio-nu56Fe-20GeV-20170202.root";
      break;

    case kNuMuBarC12:
      fidir = "outNievesRPAratio-anu12C-20GeV-20170202.root";
      break;

    case kNuMuBarO16:
      fidir = "outNievesRPAratio-anu16O-20GeV-20170202.root";
      break;

    case kNuMuBarAr40:
      fidir = "outNievesRPAratio-anu40Ar-20GeV-20170202.root";
      break;

    case kNuMuBarCa40:
      fidir = "outNievesRPAratio-anu40Ca-20GeV-20170202.root";
      break;

    case kNuMuBarFe56:
      fidir = "outNievesRPAratio-anu56Fe-20GeV-20170202.root";
      break;
  }

  LOG(FIT) << "Loading RPA CALC : " << fidir << std::endl;
  TDirectory* olddir = gDirectory;

  // fRPACalculators[calcenum] = new weightRPA(rwdir + "/" + fidir);
  olddir->cd();
  return;
}

int RikRPA::GetRPACalcEnum(int bpdg, int tpdg) {
  if (bpdg == 14 && tpdg == 1000060120)
    return kNuMuC12;
  else if (bpdg == 14 && tpdg == 1000080160)
    return kNuMuO16;
  else if (bpdg == 14 && tpdg == 1000180400)
    return kNuMuAr40;
  else if (bpdg == 14 && tpdg == 1000200400)
    return kNuMuCa40;
  else if (bpdg == 14 && tpdg == 1000280560)
    return kNuMuFe56;
  else if (bpdg == -14 && tpdg == 1000060120)
    return kNuMuBarC12;
  else if (bpdg == -14 && tpdg == 1000080160)
    return kNuMuBarO16;
  else if (bpdg == -14 && tpdg == 1000180400)
    return kNuMuBarAr40;
  else if (bpdg == -14 && tpdg == 1000200400)
    return kNuMuBarCa40;
  else if (bpdg == -14 && tpdg == 1000280560)
    return kNuMuBarFe56;
  else {
    ERROR(WRN, "Unknown beam and target combination for RPA Calcs! "
                   << bpdg << " " << tpdg);
  }

  return -1;
}

}  // namespace reweight
}  // namespace nuisance
#endif
#endif
