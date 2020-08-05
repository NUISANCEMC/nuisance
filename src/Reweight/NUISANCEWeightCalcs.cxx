#include "NUISANCEWeightCalcs.h"

#include "FitEvent.h"
#include "GeneralUtils.h"
#include "NUISANCESyst.h"
#include "WeightUtils.h"

using namespace Reweight;

ModeNormCalc::ModeNormCalc() { fNormRES = 1.0; }

double ModeNormCalc::CalcWeight(BaseFitEvt *evt) {
  int mode = abs(evt->Mode);
  double w = 1.0;

  if (mode == 11 or mode == 12 or mode == 13) {
    w *= fNormRES;
  }

  return w;
}

void ModeNormCalc::SetDialValue(std::string name, double val) {
  SetDialValue(Reweight::ConvDial(name, kCUSTOM), val);
}

void ModeNormCalc::SetDialValue(int rwenum, double val) {
  int curenum = rwenum % 1000;

  // Check Handled
  if (!IsHandled(curenum)) 
    return;
  if (curenum == kModeNorm_NormRES) 
    fNormRES = val;
}

bool ModeNormCalc::IsHandled(int rwenum) {
  int curenum = rwenum % 1000;
  switch (curenum) {
  case kModeNorm_NormRES:
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
  if (!fTweaked || !fApply_MINOSRPA) return 1.0;

  double w = 1.0;

  // If GENIE is enabled, use old code
#ifdef __GENIE_ENABLED__
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

  w *= GetRPAWeight(Q2);
#else
  // Get the Q2 from NUISANCE if not GENIE
  FitEvent *fevt = static_cast<FitEvent*>(evt);
  // Check the event is resonant
  if (!fevt->IsResonant()) return 1.0;
  int targeta = fevt->GetTargetA();
  int targetz = fevt->GetTargetZ();
  // Apply only to nuclear targets, ignore free protons
  if (targeta == 1 || targetz == 1) return 1.0;
  // Q2 in GeV2
  double Q2 = fevt->GetQ2();
  w *= GetRPAWeight(Q2);
#endif

  return w;
}

// Do the actual weight calculation
double MINOSRPA::GetRPAWeight(double Q2) {
  if (Q2 > 0.7) return 1.0;
  double w = fCur_MINOSRPA_A / (1.0 + TMath::Exp(1.0 - TMath::Sqrt(Q2) / fCur_MINOSRPA_B));
  return w;
}

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

  if (!fTweaked || !fApplyRPA) return 1.0;
  double w = 1.0;

  // If GENIE is enabled, use old code
#ifdef __GENIE_ENABLED__
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
  w *= GetRPAWeight(Q2);
#else
  // Get the Q2 from NUISANCE if not GENIE
  FitEvent *fevt = static_cast<FitEvent*>(evt);
  // Check the event is resonant
  if (!fevt->IsResonant()) return 1.0;
  int targeta = fevt->GetTargetA();
  int targetz = fevt->GetTargetZ();
  // Apply only to nuclear targets, ignore free protons
  if (targeta == 1 || targetz == 1) return 1.0;
  // Q2 in GeV2
  double Q2 = fevt->GetQ2();
  w *= GetRPAWeight(Q2);
#endif

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


BeRPACalc::BeRPACalc()
  : fBeRPA_A(0.59), fBeRPA_B(1.05), fBeRPA_D(1.13), fBeRPA_E(0.88),
  fBeRPA_U(1.2), nParams(0) {
    // A = 0.59 +/- 20%
    // B = 1.05 +/- 20%
    // D = 1.13 +/- 15%
    // E = 0.88 +/- 40%
    // U = 1.2
}

double BeRPACalc::CalcWeight(BaseFitEvt *evt) {
  int mode = abs(evt->Mode);
  double w = 1.0;
  if (nParams == 0) {
    return w;
  }

  // Get Q2
  // Get final state lepton
  if (mode == 1) {
    FitEvent *fevt = static_cast<FitEvent*>(evt);
    double Q2 = fevt->GetQ2();
    // Only CCQE events
    w *= calcRPA(Q2, fBeRPA_A, fBeRPA_B, fBeRPA_D, fBeRPA_E, fBeRPA_U);
  }

  return w;
}

void BeRPACalc::SetDialValue(std::string name, double val) {
  SetDialValue(Reweight::ConvDial(name, kCUSTOM), val);
}

void BeRPACalc::SetDialValue(int rwenum, double val) {
  int curenum = rwenum % 1000;

  // Check Handled
  if (!IsHandled(curenum))
    return;
  // Need 4 or 5 reconfigures
  if (curenum == kBeRPA_A)
    fBeRPA_A = val;
  else if (curenum == kBeRPA_B)
    fBeRPA_B = val;
  else if (curenum == kBeRPA_D)
    fBeRPA_D = val;
  else if (curenum == kBeRPA_E)
    fBeRPA_E = val;
  else if (curenum == kBeRPA_U)
    fBeRPA_U = val;
  nParams++;
}

bool BeRPACalc::IsHandled(int rwenum) {
  int curenum = rwenum % 1000;
  switch (curenum) {
    case kBeRPA_A:
    case kBeRPA_B:
    case kBeRPA_D:
    case kBeRPA_E:
    case kBeRPA_U:
      return true;
    default:
      return false;
  }
}

SBLOscWeightCalc::SBLOscWeightCalc() {
  fDistance = 0.0;
  fMassSplitting = 0.0;
  fSin2Theta = 0.0;
}

double SBLOscWeightCalc::CalcWeight(BaseFitEvt *evt) {
  FitEvent *fevt = static_cast<FitEvent *>(evt);

  FitParticle *pnu = fevt->PartInfo(0);
  double E = pnu->fP.E() / 1.E3;

  // Extract energy
  return GetSBLOscWeight(E);
}

void SBLOscWeightCalc::SetDialValue(std::string name, double val) {
  SetDialValue(Reweight::ConvDial(name, kCUSTOM), val);
}

void SBLOscWeightCalc::SetDialValue(int rwenum, double val) {
  int curenum = rwenum % 1000;
  if (!IsHandled(curenum))
    return;
  if (curenum == kSBLOsc_Distance)
    fDistance = val;
  if (curenum == kSBLOsc_MassSplitting)
    fMassSplitting = val;
  if (curenum == kSBLOsc_Sin2Theta)
    fSin2Theta = val;
}

bool SBLOscWeightCalc::IsHandled(int rwenum) {
  int curenum = rwenum % 1000;
  switch (curenum) {
    case kSBLOsc_Distance:
      return true;
    case kSBLOsc_MassSplitting:
      return true;
    case kSBLOsc_Sin2Theta:
      return true;
    default:
      return false;
  }
}

double SBLOscWeightCalc::GetSBLOscWeight(double E) {
  if (E <= 0.0)
    return 1.0 - 0.5 * fSin2Theta;
  return 1.0 - fSin2Theta * pow(sin(1.267 * fMassSplitting * fDistance / E), 2);
}

GaussianModeCorr::GaussianModeCorr() {

  // Apply the tilt-shift Gauss by Patrick
  // Alternatively set in config
  fMethod = true;

  // Init
  fApply_CCQE = false;
  fGausVal_CCQE[kPosNorm] = 0.0;
  fGausVal_CCQE[kPosTilt] = 0.0;
  fGausVal_CCQE[kPosPq0] = 1.0;
  fGausVal_CCQE[kPosWq0] = 1.0;
  fGausVal_CCQE[kPosPq3] = 1.0;
  fGausVal_CCQE[kPosWq3] = 1.0;

  fApply_2p2h = false;
  fGausVal_2p2h[kPosNorm] = 0.0;
  fGausVal_2p2h[kPosTilt] = 0.0;
  fGausVal_2p2h[kPosPq0] = 1.0;
  fGausVal_2p2h[kPosWq0] = 1.0;
  fGausVal_2p2h[kPosPq3] = 1.0;
  fGausVal_2p2h[kPosWq3] = 1.0;

  fApply_2p2h_PPandNN = false;
  fGausVal_2p2h_PPandNN[kPosNorm] = 0.0;
  fGausVal_2p2h_PPandNN[kPosTilt] = 0.0;
  fGausVal_2p2h_PPandNN[kPosPq0] = 1.0;
  fGausVal_2p2h_PPandNN[kPosWq0] = 1.0;
  fGausVal_2p2h_PPandNN[kPosPq3] = 1.0;
  fGausVal_2p2h_PPandNN[kPosWq3] = 1.0;

  fApply_2p2h_NP = false;
  fGausVal_2p2h_NP[kPosNorm] = 0.0;
  fGausVal_2p2h_NP[kPosTilt] = 0.0;
  fGausVal_2p2h_NP[kPosPq0] = 1.0;
  fGausVal_2p2h_NP[kPosWq0] = 1.0;
  fGausVal_2p2h_NP[kPosPq3] = 1.0;
  fGausVal_2p2h_NP[kPosWq3] = 1.0;

  fApply_CC1pi = false;
  fGausVal_CC1pi[kPosNorm] = 0.0;
  fGausVal_CC1pi[kPosTilt] = 0.0;
  fGausVal_CC1pi[kPosPq0] = 1.0;
  fGausVal_CC1pi[kPosWq0] = 1.0;
  fGausVal_CC1pi[kPosPq3] = 1.0;
  fGausVal_CC1pi[kPosWq3] = 1.0;

  fAllowSuppression = false;

  fDebugStatements = FitPar::Config().GetParB("GaussianModeCorr_DEBUG");
  // fDebugStatements = true;
}

double GaussianModeCorr::CalcWeight(BaseFitEvt *evt) {
  FitEvent *fevt = static_cast<FitEvent *>(evt);
  double rw_weight = 1.0;

  // Get Neutrino
  if (!fevt->Npart()) {
    NUIS_ABORT("NO particles found in stack!");
  }
  FitParticle *pnu = fevt->GetNeutrinoIn();
  if (!pnu) {
    NUIS_ABORT("NO Starting particle found in stack!");
  }

  FitParticle *plep = fevt->GetLeptonOut();
  if (!plep) return 1.0;

  TLorentzVector q = pnu->fP - plep->fP;

  // Extra q0,q3
  double q0 = fabs(q.E()) / 1.E3;
  double q3 = fabs(q.Vect().Mag()) / 1.E3;

  int initialstate = -1; // Undef
  if (abs(fevt->Mode) == 2) {
    int npr = 0;
    int nne = 0;

    for (UInt_t j = 0; j < fevt->Npart(); j++) {
      if ((fevt->PartInfo(j))->fIsAlive)
        continue;

      if (fevt->PartInfo(j)->fPID == 2212) 
        npr++;
      else if (fevt->PartInfo(j)->fPID == 2112) 
        nne++;
    }

    if (fevt->Mode == 2 && npr == 1 && nne == 1) {
      initialstate = 2;

    } else if (fevt->Mode == 2 && 
        ((npr == 0 && nne == 2) || (npr == 2 && nne == 0))) {
      initialstate = 1;
    }
  }

  // Apply weighting
  if (fApply_CCQE && abs(fevt->Mode) == 1) {
    if (fDebugStatements)
      std::cout << "Getting CCQE Weight" << std::endl;
    double g = GetGausWeight(q0, q3, fGausVal_CCQE);
    if (g < 1.0) 
      g = 1.0;
    rw_weight *= g;
  }

  if (fApply_2p2h && abs(fevt->Mode) == 2) {
    if (fDebugStatements) std::cout << "Getting 2p2h Weight" << std::endl;
    if (fDebugStatements) std::cout << "Got q0 q3 = " << q0 << " " << q3 << " mode = " << fevt->Mode << std::endl;
    rw_weight *= GetGausWeight(q0, q3, fGausVal_2p2h);
    if (fDebugStatements) std::cout << "Returning Weight " << rw_weight << std::endl;
  }

  if (fApply_2p2h_PPandNN && abs(fevt->Mode) == 2 && initialstate == 1) {
    if (fDebugStatements) std::cout << "Getting 2p2h PPandNN Weight" << std::endl;
    rw_weight *= GetGausWeight(q0, q3, fGausVal_2p2h_PPandNN);
  }

  if (fApply_2p2h_NP && abs(fevt->Mode) == 2 && initialstate == 2) {
    if (fDebugStatements) std::cout << "Getting 2p2h NP Weight" << std::endl;
    rw_weight *= GetGausWeight(q0, q3, fGausVal_2p2h_NP);
  }

  if (fApply_CC1pi && abs(fevt->Mode) >= 11 && abs(fevt->Mode) <= 13) {
    if (fDebugStatements)
      std::cout << "Getting CC1pi Weight" << std::endl;
    rw_weight *= GetGausWeight(q0, q3, fGausVal_CC1pi);
  }

  return rw_weight;
}

void GaussianModeCorr::SetMethod(bool method) {
  fMethod = method;
  if (fMethod == true) {
    NUIS_LOG(FIT,
        " Using tilt-shift Gaussian parameters for Gaussian enhancement...");
  } else {
    NUIS_LOG(FIT, " Using Normal Gaussian parameters for Gaussian enhancement...");
  }
};

double GaussianModeCorr::GetGausWeight(double q0, double q3, double vals[]) {
  // The weight
  double w = 1.0;

  // Use tilt-shift method by Patrick
  if (fMethod) {
    if (fDebugStatements) {
      std::cout << "Using Patrick gaussian" << std::endl;
    }
    // // CCQE Without Suppression
    // double Norm = 4.82788679036;
    // double Tilt = 2.3501416116;
    // double Pq0  = 0.363964889702;
    // double Wq0  = 0.133976806938;
    // double Pq3  = 0.431769740224;
    // double Wq3  = 0.207666663434;

    // // Also add for CCQE at the end
    // return (w > 1.0) ? w : 1.0;

    // // 2p2h with suppression
    // double Norm = 15.967;
    // double Tilt = -0.455655;
    // double Pq0  = 0.214598;
    // double Wq0  = 0.0291061;
    // double Pq3  = 0.480194;
    // double Wq3  = 0.134588;

    double Norm = vals[kPosNorm];
    double Tilt = vals[kPosTilt];
    double Pq0 = vals[kPosPq0];
    double Wq0 = vals[kPosWq0];
    double Pq3 = vals[kPosPq3];
    double Wq3 = vals[kPosWq3];

    double a = cos(Tilt) * cos(Tilt) / (2 * Wq0 * Wq0);
    a += sin(Tilt) * sin(Tilt) / (2 * Wq3 * Wq3);

    double b = -sin(2 * Tilt) / (4 * Wq0 * Wq0);
    b += sin(2 * Tilt) / (4 * Wq3 * Wq3);

    double c = sin(Tilt) * sin(Tilt) / (2 * Wq0 * Wq0);
    c += cos(Tilt) * cos(Tilt) / (2 * Wq3 * Wq3);

    w = Norm;
    w *= exp(-a * (q0 - Pq0) * (q0 - Pq0));
    w *= exp(+2.0 * b * (q0 - Pq0) * (q3 - Pq3));
    w *= exp(-c * (q3 - Pq3) * (q3 - Pq3));

    if (fDebugStatements) {
      std::cout << "Applied Tilt " << Tilt << " " << cos(Tilt) << " "
        << sin(Tilt) << std::endl;
      std::cout << "abc = " << a << " " << b << " " << c << std::endl;
      std::cout << "Returning " << Norm << " " << Pq0 << " " << Wq0 << " "
        << Pq3 << " " << Wq3 << " " << w << std::endl;
    }

    if (w != w || std::isnan(w) || w < 0.0) {
      w = 0.0;
    }

    if (w < 1.0 and !fAllowSuppression) {
      w = 1.0;
    }

    return w;

    // Use the MINERvA Gaussian method
  } else {
    /*
     * From MINERvA and Daniel Ruterbories:
     * Old notes here: *
     * http://cdcvs.fnal.gov/cgi-bin/public-cvs/cvsweb-public.cgi/AnalysisFramework/Ana/CCQENu/ana_common/data/?cvsroot=mnvsoft
     * These parameters are slightly altered
     *
     * FRESH:
     * 10.5798
     * 0.254032
     * 0.50834
     * 0.0571035
     * 0.129051
     * 0.875287
     */
    if (fDebugStatements) {
      std::cout << "Using MINERvA Gaussian" << std::endl;
    }

    double norm = vals[kPosNorm];
    double meanq0 = vals[kPosTilt];
    double meanq3 = vals[kPosPq0];
    double sigmaq0 = vals[kPosWq0];
    double sigmaq3 = vals[kPosPq3];
    double corr = vals[kPosWq3];

    double z = (q0 - meanq0) * (q0 - meanq0) / (sigmaq0 * sigmaq0) +
      (q3 - meanq3) * (q3 - meanq3) / (sigmaq3 * sigmaq3) -
      2 * corr * (q0 - meanq0) * (q3 - meanq3) / (sigmaq0 * sigmaq3);
    double ret = 1.0;

    if ( fabs(1 - corr*corr) < 1.E-5 ) {
      return 1.0;
    }

    if ( (-0.5 * z / (1 - corr*corr)) > 200 or (-0.5 * z / (1 - corr*corr)) < -200 ) {
      return 1.0;
    } else {
      ret = norm * exp( -0.5 * z / (1 - corr*corr) );
    }

    if (ret != ret or ret < 0.0 or isnan(ret)) {
      return 1.0;
    }

    if (fAllowSuppression) return ret;
    return ret + 1.0;
    // Need to add 1 to the results
  }
  return w;
}

void GaussianModeCorr::SetDialValue(std::string name, double val) {
  SetDialValue(Reweight::ConvDial(name, kCUSTOM), val);
}

void GaussianModeCorr::SetDialValue(int rwenum, double val) {
  int curenum = rwenum % 1000;

  // Check Handled
  if (!IsHandled(curenum)) return;

  // CCQE Setting
  for (int i = kGaussianCorr_CCQE_norm; i <= kGaussianCorr_CCQE_Wq3; i++) {
    if (i == curenum) {
      int index = i - kGaussianCorr_CCQE_norm;
      fGausVal_CCQE[index] = val;
      fApply_CCQE = true;
    }
  }

  // 2p2h Setting
  for (int i = kGaussianCorr_2p2h_norm; i <= kGaussianCorr_2p2h_Wq3; i++) {
    if (i == curenum) {
      int index = i - kGaussianCorr_2p2h_norm;
      fGausVal_2p2h[index] = val;
      fApply_2p2h = true;
    }
  }

  // 2p2h_PPandNN Setting
  for (int i = kGaussianCorr_2p2h_PPandNN_norm; i <= kGaussianCorr_2p2h_PPandNN_Wq3; i++) {
    if (i == curenum) {
      int index = i - kGaussianCorr_2p2h_PPandNN_norm;
      fGausVal_2p2h_PPandNN[index] = val;
      fApply_2p2h_PPandNN = true;
    }
  }

  // 2p2h_NP Setting
  for (int i = kGaussianCorr_2p2h_NP_norm; i <= kGaussianCorr_2p2h_NP_Wq3; i++) {
    if (i == curenum) {
      int index = i - kGaussianCorr_2p2h_NP_norm;
      fGausVal_2p2h_NP[index] = val;
      fApply_2p2h_NP = true;
    }
  }

  // CC1pi Setting
  for (int i = kGaussianCorr_CC1pi_norm; i <= kGaussianCorr_CC1pi_Wq3; i++) {
    if (i == curenum) {
      int index = i - kGaussianCorr_CC1pi_norm;
      fGausVal_CC1pi[index] = val;
      fApply_CC1pi = true;
    }
  }

  if (curenum == kGaussianCorr_AllowSuppression) {
    fAllowSuppression = (val > 0.5);
  }
}

bool GaussianModeCorr::IsHandled(int rwenum) {
  int curenum = rwenum % 1000;
  switch (curenum) {
    case kGaussianCorr_CCQE_norm:
    case kGaussianCorr_CCQE_tilt:
    case kGaussianCorr_CCQE_Pq0:
    case kGaussianCorr_CCQE_Wq0:
    case kGaussianCorr_CCQE_Pq3:
    case kGaussianCorr_CCQE_Wq3:

    case kGaussianCorr_2p2h_norm:
    case kGaussianCorr_2p2h_tilt:
    case kGaussianCorr_2p2h_Pq0:
    case kGaussianCorr_2p2h_Wq0:
    case kGaussianCorr_2p2h_Pq3:
    case kGaussianCorr_2p2h_Wq3:

    case kGaussianCorr_2p2h_PPandNN_norm:
    case kGaussianCorr_2p2h_PPandNN_tilt:
    case kGaussianCorr_2p2h_PPandNN_Pq0:
    case kGaussianCorr_2p2h_PPandNN_Wq0:
    case kGaussianCorr_2p2h_PPandNN_Pq3:
    case kGaussianCorr_2p2h_PPandNN_Wq3:

    case kGaussianCorr_2p2h_NP_norm:
    case kGaussianCorr_2p2h_NP_tilt:
    case kGaussianCorr_2p2h_NP_Pq0:
    case kGaussianCorr_2p2h_NP_Wq0:
    case kGaussianCorr_2p2h_NP_Pq3:
    case kGaussianCorr_2p2h_NP_Wq3:

    case kGaussianCorr_CC1pi_norm:
    case kGaussianCorr_CC1pi_tilt:
    case kGaussianCorr_CC1pi_Pq0:
    case kGaussianCorr_CC1pi_Wq0:
    case kGaussianCorr_CC1pi_Pq3:
    case kGaussianCorr_CC1pi_Wq3:
    case kGaussianCorr_AllowSuppression:
      return true;
    default:
      return false;
  }
}
