#include <iomanip>

#include "T2K_SignalDef.h"

#include "T2K_CC1pip_CH_XSec_1DCosThAdler_nu.h"

// The constructor
T2K_CC1pip_CH_XSec_1DCosThAdler_nu::T2K_CC1pip_CH_XSec_1DCosThAdler_nu(
    nuiskey samplekey) {

  // Sample overview ---------------------------------------------------
  std::string descrip = "T2K_CC1pip_CH_XSec_nu sample. \n"
                        "Target: CH \n"
                        "Flux: T2K FHC numu \n"
                        "Signal: CC1pi+, p_mu > 200 MeV, p_pi > 200 MeV\n"
                        ", costheta_mu > 0.2, costheta_pi > 0.2\n"
                        "https://arxiv.org/abs/1909.03936";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetTitle("T2K_CC1pip_CH_XSec_1DCosThAdler_nu");
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("cos#theta_{Adler} (radians)");
  fSettings.SetYTitle("d#sigma/dcos#theta_{Adler} (cm^{2}/1/nucleon)");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/DIAG");
  fSettings.SetEnuRange(0.0, 100.0);
  fSettings.DefineAllowedTargets("C,H");
  fSettings.DefineAllowedSpecies("numu");
  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = (GetEventHistogram()->Integral("width") * 1E-38) /
                 double(fNEvents) / TotalIntegratedFlux("width");

  // Plot Setup -------------------------------------------------------
  SetDataFromRootFile(GeneralUtils::GetTopLevelDir() +
                          "/data/T2K/CC1pip/CH/theta_adler.rootout.root",
                      "Theta_Adler");
  SetCovarFromRootFile(GeneralUtils::GetTopLevelDir() +
                           "/data/T2K/CC1pip/CH/theta_adler.rootout.root",
                       "Theta_AdlerCov");

  SetShapeCovar();
  fDataHist->Scale(1E-38);

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

void T2K_CC1pip_CH_XSec_1DCosThAdler_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(13) == 0 || event->NumFSParticle(211) == 0)
    return;

  // Essentially the same as FitUtils::CosThAdler but uses the reconsturcted
  // neutrino instead of the true neutrino Reconstruct the neutrino
  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  double rEnu = FitUtils::EnuCC1piprec_T2K_eMB(Pnu, Pmu, Ppip) * 1000.;
  // Now make the reconstructed neutrino
  // Has the same direction as the predicted neutrino
  TLorentzVector PnuReco(Pnu.Vect().Unit().X() * rEnu,
                         Pnu.Vect().Unit().Y() * rEnu,
                         Pnu.Vect().Unit().Z() * rEnu, rEnu);

  // Reconstruct the initial state assuming still nucleon
  TLorentzVector Pinit(0, 0, 0, PhysConst::mass_proton * 1000.);
  // Pretty much a copy of FitUtils::PhiAdler but using the reconstructed
  // neutrino instead of true neutrino{

  // Get the "resonance" lorentz vector (pion proton system) reconstructed from
  // the variables
  TLorentzVector Pres = PnuReco + Pinit - Pmu;
  // Boost the particles into the resonance rest frame so we can define the
  // x,y,z axis
  PnuReco.Boost(-Pres.BoostVector());
  Pmu.Boost(-Pres.BoostVector());
  Ppip.Boost(-Pres.BoostVector());

  // The z-axis is defined as the axis of three-momentum transfer, \vec{k}
  // Also unit normalise the axis
  TVector3 zAxis = (PnuReco.Vect() - Pmu.Vect()) *
                   (1.0 / ((PnuReco.Vect() - Pmu.Vect()).Mag()));

  // Then the angle between the pion in the "resonance" rest-frame and the
  // z-axis is the theta Adler angle
  double costhAdler = cos(Ppip.Vect().Angle(zAxis));

  fXVar = costhAdler;

  return;
};

//********************************************************************
bool T2K_CC1pip_CH_XSec_1DCosThAdler_nu::isSignal(FitEvent *event) {
  //********************************************************************
  return SignalDef::isCC1pip_T2K_arxiv1909_03936(
      event, EnuMin, EnuMax,
      SignalDef::kMuonHighEff | SignalDef::kPionVFwd | SignalDef::kPionHighMom);
}
