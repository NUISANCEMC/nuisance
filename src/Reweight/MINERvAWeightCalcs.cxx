#ifdef __MINERVA_RW_ENABLED__
#ifdef __GENIE_ENABLED__

#include "MINERvAWeightCalcs.h"
#include "BaseFitEvt.h"

namespace nuisance {
namespace reweight {

//*******************************************************
MINERvAReWeight_QE::MINERvAReWeight_QE() {
  //*******************************************************
  fTwk_NormCCQE = 0.0;
  fDef_NormCCQE = 1.0;
  fCur_NormCCQE = fDef_NormCCQE;
}

//*******************************************************
MINERvAReWeight_QE::~MINERvAReWeight_QE(){};
//*******************************************************

//*******************************************************
double MINERvAReWeight_QE::CalcWeight(BaseFitEvt *evt) {
  //*******************************************************
  // Check GENIE
  if (evt->fType != kGENIE) return 1.0;

  // Extract the GENIE Record
  GHepRecord *ghep = static_cast<GHepRecord *>(evt->genie_event->event);
  const Interaction *interaction = ghep->Summary();
  // const InitialState& init_state = interaction->InitState();
  const ProcessInfo &proc_info = interaction->ProcInfo();
  // const Target& tgt = init_state.Tgt();

  // If the event is not QE this Calc doesn't handle it
  if (!proc_info.IsQuasiElastic()) return 1.0;

  // WEIGHT CALCULATIONS -------------
  double w = 1.0;

  // CCQE Dial
  if (!proc_info.IsWeakCC()) w *= fCur_NormCCQE;

  // Return Combined Weight
  return w;
}

//*******************************************************
void MINERvAReWeight_QE::SetDialValue(std::string name, double val) {
  //*******************************************************
  SetDialValue(Reweight::ConvDial(name, kCUSTOM), val);
}

//*******************************************************
void MINERvAReWeight_QE::SetDialValue(int rwenum, double val) {
  //*******************************************************
  // Check Handled
  int curenum = rwenum % 1000;
  if (!IsHandled(curenum)) return;

  // Set Values
  if (curenum == Reweight::kMINERvARW_NormCCQE) {
    fTwk_NormCCQE = val;
    fCur_NormCCQE = fDef_NormCCQE + fTwk_NormCCQE;
  }

  // Define Tweaked
  fTweaked = ((fTwk_NormCCQE != 0.0));
}

//*******************************************************
bool MINERvAReWeight_QE::IsHandled(int rwenum) {
  //*******************************************************
  int curenum = rwenum % 1000;

  switch (curenum) {
  case Reweight::kMINERvARW_NormCCQE:
    return true;
  default:
    return false;
  }
}

//*******************************************************
MINERvAReWeight_MEC::MINERvAReWeight_MEC() {
  //*******************************************************
  fTwk_NormCCMEC = 0.0;
  fDef_NormCCMEC = 1.0;
  fCur_NormCCMEC = fDef_NormCCMEC;
}

//*******************************************************
MINERvAReWeight_MEC::~MINERvAReWeight_MEC(){};
//*******************************************************

//*******************************************************
double MINERvAReWeight_MEC::CalcWeight(BaseFitEvt *evt) {
  //*******************************************************
  // Check GENIE
  if (evt->fType != kGENIE)
    return 1.0;

  // Extract the GENIE Record
  GHepRecord *ghep = static_cast<GHepRecord *>(evt->genie_event->event);
  const Interaction *interaction = ghep->Summary();
  // const InitialState& init_state = interaction->InitState();
  const ProcessInfo &proc_info = interaction->ProcInfo();
  // const Target& tgt = init_state.Tgt();

  // If the event is not MEC this Calc doesn't handle it
  if (!proc_info.IsMEC()) return 1.0;

  // WEIGHT CALCULATIONS -------------
  double w = 1.0;

  // CCMEC Dial
  if (!proc_info.IsWeakCC()) w *= fCur_NormCCMEC;

  // Return Combined Weight
  return w;
}

//*******************************************************
void MINERvAReWeight_MEC::SetDialValue(std::string name, double val) {
  //*******************************************************
  SetDialValue(Reweight::ConvDial(name, kCUSTOM), val);
}

//*******************************************************
void MINERvAReWeight_MEC::SetDialValue(int rwenum, double val) {
  //*******************************************************
  // Check Handled
  int curenum = rwenum % 1000;
  if (!IsHandled(curenum))
    return;

  // Set Values
  if (curenum == Reweight::kMINERvARW_NormCCMEC) {
    fTwk_NormCCMEC = val;
    fCur_NormCCMEC = fDef_NormCCMEC + fTwk_NormCCMEC;
  }

  // Define Tweaked
  fTweaked = ((fTwk_NormCCMEC != 0.0));
}

//*******************************************************
bool MINERvAReWeight_MEC::IsHandled(int rwenum) {
  //*******************************************************
  int curenum = rwenum % 1000;

  switch (curenum) {
  case Reweight::kMINERvARW_NormCCMEC:
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

//*******************************************************
MINERvAReWeight_RES::~MINERvAReWeight_RES(){};
//*******************************************************

//*******************************************************
double MINERvAReWeight_RES::CalcWeight(BaseFitEvt *evt) {
  //*******************************************************

  // std::cout << "Caculating RES" << std::endl;
  // Check GENIE
  if (evt->fType != kGENIE) return 1.0;

  // Extract the GENIE Record
  GHepRecord *ghep = static_cast<GHepRecord *>(evt->genie_event->event);
  const Interaction *interaction = ghep->Summary();
  // const InitialState& init_state = interaction->InitState();
  const ProcessInfo &proc_info = interaction->ProcInfo();
  // const Target& tgt = init_state.Tgt();

  // If the event is not RES this Calc doesn't handle it
  if (!proc_info.IsResonant()) return 1.0;

  // WEIGHT CALCULATIONS -------------
  double w = 1.0;

  // CCRES Dial
  if (proc_info.IsWeakCC()) w *= fCur_NormCCRES;

  // Return Combined Weight
  return w;
}

//*******************************************************
void MINERvAReWeight_RES::SetDialValue(std::string name, double val) {
  //*******************************************************
  SetDialValue(Reweight::ConvDial(name, kCUSTOM), val);
}

//*******************************************************
void MINERvAReWeight_RES::SetDialValue(int rwenum, double val) {
  //*******************************************************
  // Check Handled
  int curenum = rwenum % 1000;
  if (!IsHandled(curenum)) return;

  // Set Values
  if (curenum == Reweight::kMINERvARW_NormCCRES) {
    fTwk_NormCCRES = val;
    fCur_NormCCRES = fDef_NormCCRES + fTwk_NormCCRES;
  }

  // Define Tweaked
  fTweaked = ((fTwk_NormCCRES != 0.0));
}

//*******************************************************
bool MINERvAReWeight_RES::IsHandled(int rwenum) {
  //*******************************************************
  int curenum = rwenum % 1000;

  switch (curenum) {
    case Reweight::kMINERvARW_NormCCRES:
      return true;
    default:
      return false;
  }
}

//*****************************************************************************
MINOSRPA::MINOSRPA() {

  fApply_MINOSRPA = false;

  fDef_MINOSRPA_A = 1.010;
  fTwk_MINOSRPA_A = 0.000;
  fDef_MINOSRPA_B = 0.156;
  fTwk_MINOSRPA_B = 0.000;

}
//
double MINOSRPA::CalcWeight(BaseFitEvt* evt) {
  if (!fTweaked) return 1.0;
  double w = 1.0;

  // Extract the GENIE Record
  GHepRecord* ghep = static_cast<GHepRecord*>(evt->genie_event->event);
  const Interaction* interaction = ghep->Summary();
  const InitialState& init_state = interaction->InitState();
  const ProcessInfo& proc_info = interaction->ProcInfo();
  const Target& tgt = init_state.Tgt();

  // If not QE return 1.0
  if (!tgt.IsNucleus()) return 1.0;
  if (!proc_info.IsQuasiElastic() && !proc_info.IsResonant()) return 1.0;

  // Extract Beam and Target PDG
  GHepParticle* neutrino = ghep->Probe();
  //   int bpdg = neutrino->Pdg();

  GHepParticle* target = ghep->Particle(1);
  assert(target);
  //    int tpdg = target->Pdg();

  // Extract Q0-Q3
  GHepParticle* fsl = ghep->FinalStatePrimaryLepton();
  const TLorentzVector& k1 = *(neutrino->P4());
  const TLorentzVector& k2 = *(fsl->P4());
  //double q0 = fabs((k1 - k2).E());
  //double q3 = fabs((k1 - k2).Vect().Mag());
  double Q2 = fabs((k1 - k2).Mag2());

  // Resonant Events
  if (proc_info.IsResonant() && fApply_MINOSRPA) {
    w *= GetRPAWeight(
        Q2,
        fCur_MINOSRPA_A,
        fCur_MINOSRPA_B
        );
  }

  return w;
}
//
double MINOSRPA::GetRPAWeight(double Q2, double A, double B) {
  if (Q2 > 0.7) return 1.0;
  double w = A / (1.0 + TMath::Exp(1.0 - TMath::Sqrt(Q2) / B));
  return w;
}
//
bool MINOSRPA::IsHandled(int rwenum) {
  int curenum = rwenum % 1000;
  switch (curenum) {
    case Reweight::kMINERvARW_MINOSRPA_Apply:
    case Reweight::kMINERvARW_MINOSRPA_A:
    case Reweight::kMINERvARW_MINOSRPA_B:
      return true;
    default:
      return false;
  }
}
//
void MINOSRPA::SetDialValue(std::string name, double val) {
  SetDialValue(Reweight::ConvDial(name, kCUSTOM), val);
}
//
void MINOSRPA::SetDialValue(int rwenum, double val) {
  int curenum = rwenum % 1000;

  // Check Handled
  if (!IsHandled(curenum)) return;
  if (curenum == Reweight::kMINERvARW_MINOSRPA_Apply) fApply_MINOSRPA = (val > 0.5);
  if (curenum == Reweight::kMINERvARW_MINOSRPA_A) fTwk_MINOSRPA_A = val;
  if (curenum == Reweight::kMINERvARW_MINOSRPA_B) fTwk_MINOSRPA_B = val;

  // Check for changes
  fTweaked = (fApply_MINOSRPA ||
      fabs(fTwk_MINOSRPA_A) > 0.0 ||
      fabs(fTwk_MINOSRPA_B) > 0.0);

  // Update Values
  fCur_MINOSRPA_A = fDef_MINOSRPA_A * (1.0 + 0.1 * fTwk_MINOSRPA_A);
  fCur_MINOSRPA_B = fDef_MINOSRPA_B * (1.0 + 0.1 * fTwk_MINOSRPA_B);

}
//
//
//*****************************************************************************
LagrangeRPA::LagrangeRPA() {

  fApplyRPA = false;

  /*
  fI1_Def = 4.18962e-01;
  fI1     = fI1_Def;

  fI2_Def = 7.39927e-01;
  fI2     = fI2_Def;
  */

  // Table VIII https://arxiv.org/pdf/1903.01558.pdf
  fR1_Def = 0.37;
  fR1     = fR1_Def;

  fR2_Def = 0.60;
  fR2     = fR2_Def;

}
//
double LagrangeRPA::CalcWeight(BaseFitEvt* evt) {

  if (!fTweaked) return 1.0;
  double w = 1.0;

  // Extract the GENIE Record
  GHepRecord* ghep = static_cast<GHepRecord*>(evt->genie_event->event);
  const Interaction* interaction = ghep->Summary();
  const InitialState& init_state = interaction->InitState();
  const ProcessInfo& proc_info = interaction->ProcInfo();
  const Target& tgt = init_state.Tgt();

  // If not QE return 1.0
  if (!tgt.IsNucleus()) return 1.0;
  if (!proc_info.IsQuasiElastic() && !proc_info.IsResonant()) return 1.0;

  // Extract Beam and Target PDG
  GHepParticle* neutrino = ghep->Probe();
  //   int bpdg = neutrino->Pdg();

  GHepParticle* target = ghep->Particle(1);
  assert(target);
  //    int tpdg = target->Pdg();

  // Extract Q0-Q3
  GHepParticle* fsl = ghep->FinalStatePrimaryLepton();
  const TLorentzVector& k1 = *(neutrino->P4());
  const TLorentzVector& k2 = *(fsl->P4());
  //double q0 = fabs((k1 - k2).E());
  //double q3 = fabs((k1 - k2).Vect().Mag());
  double Q2 = fabs((k1 - k2).Mag2());

  if (proc_info.IsResonant() && fApplyRPA) {
    w *= GetRPAWeight(Q2);
  }

  return w;
}
//
//
double LagrangeRPA::GetRPAWeight(double Q2) {
  //std::cout << "Getting RPA Weight : " << Q2 << std::endl;
  if (Q2 > 0.7) return 1.0;

  // Keep original Lagrange RPA for documentation
  /*
  double x1 = 0.00;
  double x2 = 0.30;
  double x3 = 0.70;

  double y1 = 0.00;
  double y2 = fI2;
  double y3 = 1.00;

  double xv = Q2;

  // Automatically 0 because y1 choice
  double W1 = y1 * (xv-x2)*(xv-x3)/((x1-x2)*(x1-x3));
  double W2 = y2 * (xv-x1)*(xv-x3)/((x2-x1)*(x2-x3));
  double W3 = y3 * (xv-x1)*(xv-x2)/((x3-x1)*(x3-x2));

  double P = W1 + W2 + W3;
  double A1 = (1.0 - sqrt(1.0 - fI1));
  double R = P * (1.0 - A1) + A1;

  return 1.0 - (1.0-R)*(1.0-R);
  */

  // Equation 7 https://arxiv.org/pdf/1903.01558.pdf
  const double x1 = 0.00;
  const double x2 = 0.35;
  const double x3 = 0.70;

  // Equation 6 https://arxiv.org/pdf/1903.01558.pdf
  double RQ2 = fR2 *( (Q2-x1)*(Q2-x3)/((x2-x1)*(x2-x3)) )
              + (Q2-x1)*(Q2-x2)/((x3-x1)*(x3-x2));
  double weight = 1-(1-fR1)*(1-RQ2)*(1-RQ2);

  // Check range of R1 and R2
  // Commented out because this is implementation dependent: user may want strange double peaks
  /*
  if (fR1 > 1) return 1;
  if (fR2 > 1 || fR2 < 0.5) return 1;
  */

  return weight;
}
//
bool LagrangeRPA::IsHandled(int rwenum) {
  int curenum = rwenum % 1000;
  switch (curenum) {
    case Reweight::kMINERvARW_LagrangeRPA_Apply:
    case Reweight::kMINERvARW_LagrangeRPA_R1:
    case Reweight::kMINERvARW_LagrangeRPA_R2:
      return true;
    default:
      return false;
  }
}
//
void LagrangeRPA::SetDialValue(std::string name, double val) {
  SetDialValue(Reweight::ConvDial(name, kCUSTOM), val);
}
//
void LagrangeRPA::SetDialValue(int rwenum, double val) {
  int curenum = rwenum % 1000;

  // Check Handled
  if (!IsHandled(curenum)) return;
  if (curenum == Reweight::kMINERvARW_LagrangeRPA_Apply) fApplyRPA = (val > 0.5);
  if (curenum == Reweight::kMINERvARW_LagrangeRPA_R1) fR1 = val;
  if (curenum == Reweight::kMINERvARW_LagrangeRPA_R2) fR2 = val;

  // Check for changes
  fTweaked = (fApplyRPA);
}
//

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

  // - Syst : kMINERvA_RikRESRPA_ApplyRPA
  // - Type : Binary
  // - Limits : 0.0 (false) -> 1.0 (true)
  // - Default : 0.0
  fApplyDial_RESRPACorrection = false;

  // - Syst : kMINERvA_RikRESRPA_LowQ2
  // - Type : Absolute
  // - Limits : 1.0 -> 1.0
  // - Default : 0.0
  // - Frac Error : 100%
  fDefDial_RESRPALowQ2 = 0.0;
  fCurDial_RESRPALowQ2 = fDefDial_RESRPALowQ2;
  fErrDial_RESRPALowQ2 = 0.0;

  // - Syst : kMINERvA_RikRESRPA_HighQ2
  // - Type : Absolute
  // - Limits : 1.0 -> 1.0
  // - Default : 0.0
  // - Frac Error : 100%
  fDefDial_RESRPAHighQ2 = 0.0;
  fCurDial_RESRPAHighQ2 = fDefDial_RESRPAHighQ2;
  fErrDial_RESRPAHighQ2 = 1.0;

  // Setup Calculators
  fEventWeights = new double[5];
  for (int i = 0; i < kMaxCalculators; i++) {
    fRPACalculators[i] = NULL;
  }

  fTweaked = false;
}

//*******************************************************
RikRPA::~RikRPA() {
  //*******************************************************
  //  delete fEventWeights;

  //  for (size_t i = 0; i < kMaxCalculators; i++) {
  //    if (fRPACalculators[i]) delete fRPACalculators[i];
  //    fRPACalculators[i] = NULL;
  //  }
}

//*******************************************************
double RikRPA::CalcWeight(BaseFitEvt *evt) {
  //*******************************************************
  // LOG(FIT) << "Calculating RikRPA" << std::endl;
  // Return 1.0 if not tweaked
  if (!fTweaked)
    return 1.0;

  double w = 1.0;

  // Extract the GENIE Record
  GHepRecord *ghep = static_cast<GHepRecord *>(evt->genie_event->event);
  const Interaction *interaction = ghep->Summary();
  const InitialState &init_state = interaction->InitState();
  const ProcessInfo &proc_info = interaction->ProcInfo();
  // const Kinematics &   kine       = interaction->Kine();
  // const XclsTag &      xcls       = interaction->ExclTag();
  const Target &tgt = init_state.Tgt();

  // If not QE return 1.0
  // LOG(FIT) << "RikRPA : Event QE = " << proc_info.IsQuasiElastic() <<
  // std::endl;
  if (!tgt.IsNucleus()) {
    return 1.0;
  }
  if (!proc_info.IsQuasiElastic() && !proc_info.IsResonant())
    return 1.0;

  // Extract Beam and Target PDG
  GHepParticle *neutrino = ghep->Probe();
  int bpdg = neutrino->Pdg();

  GHepParticle *target = ghep->TargetNucleus();
  assert(target);
  int tpdg = target->Pdg();

  // Find the enum we need
  int calcenum = GetRPACalcEnum(bpdg, tpdg);
  if (calcenum == -1) 
    return 1.0;

  // Check we have the RPA Calc setup for this enum
  // if not, set it up at that point
  if (!fRPACalculators[calcenum]) 
    SetupRPACalculator(calcenum);
  weightRPA *rpacalc = fRPACalculators[calcenum];
  if (!rpacalc) {
    NUIS_ABORT("Failed to grab the RPA Calculator : " << calcenum);
  }

  // Extract Q0-Q3
  GHepParticle *fsl = ghep->FinalStatePrimaryLepton();
  const TLorentzVector &k1 = *(neutrino->P4());
  const TLorentzVector &k2 = *(fsl->P4());
  double q0 = fabs((k1 - k2).E());
  double q3 = fabs((k1 - k2).Vect().Mag());
  double Q2 = fabs((k1 - k2).Mag2());

  // Quasielastic
  if (proc_info.IsQuasiElastic()) {

    // Now use q0-q3 and RPA Calculator to fill fWeights
    rpacalc->getWeight(q0, q3, fEventWeights);

    if (fApplyDial_RPACorrection) {
      w *= fEventWeights[0]; // CV
    }

    // Syst Application : kMINERvA_RikRPA_LowQ2
    if (fabs(fCurDial_RPALowQ2) > 0.0) {
      double interpw = fEventWeights[0];

      if (fCurDial_RPALowQ2 > 0.0 && Q2 < 2.0) {
        interpw =
            fEventWeights[0] - (fEventWeights[0] - fEventWeights[1]) *
                                   fCurDial_RPALowQ2; // WLow+
      } else if (fCurDial_RPALowQ2 < 0.0 && Q2 < 2.0) {
        interpw = fEventWeights[0] - (fEventWeights[2] - fEventWeights[0]) *
                                         fCurDial_RPALowQ2; // WLow-
      }
      w *= interpw / fEventWeights[0]; // Div by CV again
    }

    // Syst Application : kMINERvA_RikRPA_HighQ2
    if (fabs(fCurDial_RPAHighQ2) > 0.0) {
      double interpw = fEventWeights[0];

      if (fCurDial_RPAHighQ2 > 0.0) {
        interpw = fEventWeights[0] - (fEventWeights[0] - fEventWeights[3]) *
          fCurDial_RPAHighQ2; // WHigh+

      } else if (fCurDial_RPAHighQ2 < 0.0) {
        interpw = fEventWeights[0] - (fEventWeights[4] - fEventWeights[0]) *
          fCurDial_RPAHighQ2; // WHigh-
      }
      w *= interpw / fEventWeights[0]; // Div by CV again
    }
  }

  // Resonant Events
  if (proc_info.IsResonant()) {

    // Now use Q2 and RESRPA Calculator to fill fWeights
    double CV = rpacalc->getWeight(Q2);

    if (fApplyDial_RESRPACorrection) {
      w *= CV; // fEventWeights[0];  // CVa
    }

    /*
    // Syst Application : kMINERvA_RikRESRPA_LowQ2
    if (fabs(fCurDial_RESRPAHighQ2) > 0.0) {
    double interpw = fEventWeights[0];

    if (fCurDial_RESRPAHighQ2 > 0.0) {
    interpw = fEventWeights[0] - (fEventWeights[0] - fEventWeights[3]) *
    fCurDial_RESRPAHighQ2;  // WHigh+

    } else if (fCurDial_RESRPAHighQ2 < 0.0) {
    interpw = fEventWeights[0] - (fEventWeights[4] - fEventWeights[0]) *
    fCurDial_RESRPAHighQ2;  // WHigh-
    }
    w *= interpw / fEventWeights[0];  // Div by CV again
    }

    // Syst Application : kMINERvA_RikRESRPA_HighQ2
    if (fabs(fCurDial_RESRPAHighQ2) > 0.0) {
    double interpw = fEventWeights[0];

    if (fCurDial_RESRPAHighQ2 > 0.0) {
    interpw = fEventWeights[0] - (fEventWeights[0] - fEventWeights[3]) *
    fCurDial_RESRPAHighQ2;  // WHigh+

    } else if (fCurDial_RESRPAHighQ2 < 0.0) {
    interpw = fEventWeights[0] - (fEventWeights[4] - fEventWeights[0]) *
    fCurDial_RESRPAHighQ2;  // WHigh-
    }
    w *= interpw / fEventWeights[0];  // Div by CV again
    }
    */
  }

  // LOG(FIT) << "RPA Weight = " << w << std::endl;
  return w;
} // namespace reweight

//*******************************************************
void RikRPA::SetDialValue(std::string name, double val) {
  //*******************************************************
  SetDialValue(Reweight::ConvDial(name, kCUSTOM), val);
}

//*******************************************************
void RikRPA::SetDialValue(int rwenum, double val) {
  //*******************************************************
  int curenum = rwenum % 1000;

  // Check Handled
  if (!IsHandled(curenum))
    return;
  if (curenum == Reweight::kMINERvARW_RikRPA_ApplyRPA)
    fApplyDial_RPACorrection = (val > 0.5);
  if (curenum == Reweight::kMINERvARW_RikRPA_LowQ2)
    fCurDial_RPALowQ2 = val;
  if (curenum == Reweight::kMINERvARW_RikRPA_HighQ2)
    fCurDial_RPAHighQ2 = val;
  if (curenum == Reweight::kMINERvARW_RikRESRPA_ApplyRPA)
    fApplyDial_RESRPACorrection = (val > 0.5);
  if (curenum == Reweight::kMINERvARW_RikRESRPA_LowQ2)
    fCurDial_RESRPALowQ2 = val;
  if (curenum == Reweight::kMINERvARW_RikRESRPA_HighQ2)
    fCurDial_RESRPAHighQ2 = val;

  // Assign flag to say stuff has changed
  fTweaked = (fApplyDial_RPACorrection ||
      fabs(fCurDial_RPAHighQ2 - fDefDial_RPAHighQ2) > 0.0 ||
      fabs(fCurDial_RPALowQ2 - fDefDial_RPALowQ2) > 0.0 ||
      fApplyDial_RESRPACorrection ||
      fabs(fCurDial_RESRPAHighQ2 - fDefDial_RESRPAHighQ2) > 0.0 ||
      fabs(fCurDial_RESRPALowQ2 - fDefDial_RESRPALowQ2) > 0.0);
}

//*******************************************************
bool RikRPA::IsHandled(int rwenum) {
  //*******************************************************
  int curenum = rwenum % 1000;
  switch (curenum) {
    case Reweight::kMINERvARW_RikRESRPA_ApplyRPA:
      return true;
    case Reweight::kMINERvARW_RikRESRPA_LowQ2:
      return true;
    case Reweight::kMINERvARW_RikRESRPA_HighQ2:
      return true;
    case Reweight::kMINERvARW_RikRPA_ApplyRPA:
      return true;
    case Reweight::kMINERvARW_RikRPA_LowQ2:
      return true;
    case Reweight::kMINERvARW_RikRPA_HighQ2:
      return true;
    default:
      return false;
  }
}

//*******************************************************
void RikRPA::SetupRPACalculator(int calcenum) {
  //*******************************************************
  std::string rwdir = FitPar::GetDataBase() + "reweight/MINERvA/RikRPA/";
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

  NUIS_LOG(FIT, "Loading RPA CALC : " << fidir);
  TDirectory *olddir = gDirectory;

  NUIS_LOG(FIT, "***********************************************");
  NUIS_LOG(FIT, "Loading a new weightRPA calculator");
  NUIS_LOG(FIT, "Authors:  Rik Gran, Heidi Schellman");
  NUIS_LOG(FIT, "Citation: arXiv:1705.02932 [hep-ex]");
  NUIS_LOG(FIT, "***********************************************");

  // Test the file exists
  std::ifstream infile((rwdir + fidir).c_str());
  if (!infile.good()) {
    NUIS_ERR(FTL, "*** ERROR ***");
    NUIS_ERR(FTL, "RikRPA file " << rwdir + fidir << " does not exist!");
    NUIS_ERR(FTL,
        "These can be found at https://nuisance.hepforge.org/files/RikRPA/");
    NUIS_ERR(FTL,
        "Please run: wget -r -nH --cut-dirs=2 -np -e robots=off -R "
        "\"index.html*\" https://nuisance.hepforge.org/files/RikRPA/ -P "
        << rwdir);
    NUIS_ERR(FTL, "And try again");
    NUIS_ABORT("*************");
  }

  fRPACalculators[calcenum] = new weightRPA(rwdir + fidir);
  olddir->cd();
  return;
}

//*******************************************************
int RikRPA::GetRPACalcEnum(int bpdg, int tpdg) {
  //*******************************************************
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
    // NUIS_ERR(WRN, "Unknown beam and target combination for RPA Calcs! "
    //<< bpdg << " " << tpdg);
  }

  return -1;
}


//*****************************************************************************
COHBrandon::COHBrandon() {

  fApply_COHNorm = false;

  fDef_COHNorm = 0.5;
  fCur_COHNorm = fDef_COHNorm;
  fTwk_COHNorm = 0.0;

  fDef_COHCut = 0.450;
  fCur_COHCut = fDef_COHCut;
  fTwk_COHNorm = 0.0;

}

COHBrandon::~COHBrandon() {};

double COHBrandon::CalcWeight(BaseFitEvt* evt) {

  // Check GENIE
  if (evt->fType != kGENIE) return 1.0;

  // Extract the GENIE Record
  GHepRecord* ghep = static_cast<GHepRecord*>(evt->genie_event->event);
  const Interaction* interaction = ghep->Summary();
  //const InitialState& init_state = interaction->InitState();
  const ProcessInfo& proc_info = interaction->ProcInfo();
  //const Target& tgt = init_state.Tgt();

  // If the event is not QE this Calc doesn't handle it
  if (!proc_info.IsCoherent()) return 1.0;

  // WEIGHT CALCULATIONS -------------
  double w = 1.0;
  double pionE = -999.9;

  TObjArrayIter piter(ghep);
  GHepParticle * p = 0;
  //int ip = -1;
  while ( (p = (GHepParticle *) piter.Next()) ) {
    int pdgc = p->Pdg();
    int ist  = p->Status();
    //      if (ghep->Particle(ip)->FirstMother()==0) continue;
    if (pdg::IsPseudoParticle(p->Pdg())) continue;
    if (ist == kIStStableFinalState) {
      if (pdgc == 211 || pdgc == -211 || pdgc == 111) {
        pionE = p->Energy();
        break;
      }
    }
  }
  //    std::cout << "Coherent Pion Energy : " << pionE << std::endl;

  // Apply Weight
  if (fApply_COHNorm && pionE > 0.0 && pionE < fCur_COHCut) {
    w *= fCur_COHNorm;
  }

  // Return Combined Weight
  return w;
}

void COHBrandon::SetDialValue(std::string name, double val) {
  SetDialValue(Reweight::ConvDial(name, kCUSTOM), val);
}

void COHBrandon::SetDialValue(int rwenum, double val) {
  // Check Handled
  int curenum = rwenum % 1000;
  if (!IsHandled(curenum)) return;

  // Set Values
  if (curenum == Reweight::kMINERvARW_NormCOH) {
    fTwk_COHNorm = val;
    fCur_COHNorm = fDef_COHNorm * (1.0 + 0.1 * fTwk_COHNorm);
  }

  if (curenum == Reweight::kMINERvARW_CutCOH) {
    fTwk_COHCut = val;
    fCur_COHCut = fDef_COHCut * (1.0 + 0.1 * fTwk_COHCut);
  }

  if (curenum == Reweight::kMINERvARW_ApplyCOH) {
    fApply_COHNorm = (val > 0.5);
  }

  // Define Tweaked
  fTweaked = (fApply_COHNorm);
}

bool COHBrandon::IsHandled(int rwenum) {
  int curenum = rwenum % 1000;

  switch (curenum) {
    case Reweight::kMINERvARW_NormCOH:
    case Reweight::kMINERvARW_CutCOH:
    case Reweight::kMINERvARW_ApplyCOH:
      return true;
    default:
      return false;
  }
}

//*****************************************************************************
WEnhancement::WEnhancement() {

  fApply_Enhancement = false;

  fDef_WNorm = 1.0;
  fCur_WNorm = fDef_WNorm;
  fTwk_WNorm = 0.0;

  fDef_WMean = 0.95;
  fCur_WMean = fDef_WMean;
  fTwk_WMean = 0.0;

  fDef_WSigma = 0.225;
  fCur_WSigma = fDef_WSigma;
  fTwk_WSigma = 0.0;

}

WEnhancement::~WEnhancement() {};

double WEnhancement::CalcWeight(BaseFitEvt* evt) {

  // Check GENIE
  if (evt->fType != kGENIE) return 1.0;

  // Extract the GENIE Record
  GHepRecord* ghep = static_cast<GHepRecord*>(evt->genie_event->event);
  const Interaction* interaction = ghep->Summary();
  //const Kinematics &   kine       = interaction->Kine();
  //const InitialState& init_state = interaction->InitState();
  const ProcessInfo& proc_info = interaction->ProcInfo();
  //const Target& tgt = init_state.Tgt();

  // If the event is not QE this Calc doesn't handle it
  if (!proc_info.IsWeakCC()) return 1.0;
  if (!proc_info.IsResonant()) return 1.0;
  if (!fApply_Enhancement) return 1.0;

  // WEIGHT CALCULATIONS -------------
  double w = 1.0;
  bool isCC1pi0AtVertex = false;

  // Get W
  GHepParticle* neutrino = ghep->Probe();
  GHepParticle* fsl = ghep->FinalStatePrimaryLepton();
  const TLorentzVector& k1 = *(neutrino->P4());
  const TLorentzVector& k2 = *(fsl->P4());

  double E_mu = k2.E();
  double p_mu = k2.Vect().Mag();
  double m_mu = sqrt(E_mu * E_mu - p_mu * p_mu);
  double th_nu_mu = k1.Vect().Angle(k2.Vect());

  // The factor of 1000 is necessary for downstream functions
  const double m_p = PhysConst::mass_proton;
  double E_nu = k1.E();
  //    double hadMass  = kine.W (true);
  double hadMass = sqrt(m_p * m_p + m_mu * m_mu - 2 * m_p * E_mu + \
      2 * E_nu * (m_p - E_mu + p_mu * cos(th_nu_mu)));


  // Determine if event is CC1pi0 at vertex
  TObjArrayIter piter(ghep);
  GHepParticle * p = 0;
  int pi0 = 0;
  int piother = 0;
  while ( (p = (GHepParticle *) piter.Next()) ) {
    int pdgc = p->Pdg();
    int ist  = p->Status();
    if (pdg::IsPseudoParticle(p->Pdg())) continue;
    if (ist == genie::kIStStableFinalState) {
      if (pdgc == 111) {
        pi0++;
      } else if (pdgc == 211 || pdgc == -211) {
        piother++;
      }
    }
  }

  //    std::cout << "Npi0 = " << pi0 << std::endl;
  isCC1pi0AtVertex = (pi0 == 1 && piother == 0);

  // Apply Weight
  if (fApply_Enhancement && isCC1pi0AtVertex) {

    double enhancement = 1.0 + fCur_WNorm * (exp( -0.5 * pow((fCur_WMean - hadMass) / (fCur_WSigma), 2) ));
    w *= enhancement;
  }

  // Return Combined Weight
  if (isnan(w) || w < 0.0 || w > 400.0) {
    w = 1.0;
  }

  return w;
}

void WEnhancement::SetDialValue(std::string name, double val) {
  SetDialValue(Reweight::ConvDial(name, kCUSTOM), val);
}

void WEnhancement::SetDialValue(int rwenum, double val) {
  // Check Handled
  int curenum = rwenum % 1000;
  if (!IsHandled(curenum)) return;

  // Set Values
  if (curenum == Reweight::kMINERvARW_ApplyWTune) {
    fApply_Enhancement = (val > 0.5);
  }

  if (curenum == Reweight::kMINERvARW_NormWTune) {
    fTwk_WNorm = val;
    fCur_WNorm = fDef_WNorm * (1.0 + 0.1 * fTwk_WNorm);
  }

  if (curenum == Reweight::kMINERvARW_MeanWTune) {
    fTwk_WMean = val;
    fCur_WMean = fDef_WMean * (1.0 + 0.1 * fTwk_WMean);
  }

  if (curenum == Reweight::kMINERvARW_SigmaWTune) {
    fTwk_WSigma = val;
    fCur_WSigma = fDef_WSigma * (1.0 + 0.1 * fTwk_WSigma);
  }

  // Define Tweaked
  fTweaked = (fApply_Enhancement);
}

bool WEnhancement::IsHandled(int rwenum) {
  int curenum = rwenum % 1000;

  switch (curenum) {
    case Reweight::kMINERvARW_ApplyWTune:
    case Reweight::kMINERvARW_NormWTune:
    case Reweight::kMINERvARW_MeanWTune:
    case Reweight::kMINERvARW_SigmaWTune:
      return true;
    default:
      return false;
  }
}

} // namespace reweight
} // namespace nuisance
#endif
#endif
