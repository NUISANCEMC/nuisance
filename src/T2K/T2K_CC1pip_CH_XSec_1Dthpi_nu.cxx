#include <iomanip>

#include "T2K_CC1pip_CH_XSec_1Dthpi_nu.h"
#include "T2K_SignalDef.h"

// The constructor
T2K_CC1pip_CH_XSec_1Dthpi_nu::T2K_CC1pip_CH_XSec_1Dthpi_nu(nuiskey samplekey) {

  // Sample overview ---------------------------------------------------
  std::string descrip = "T2K_CC1pip_CH_XSec_nu sample. \n"
                        "Target: CH \n"
                        "Flux: T2K FHC numu \n"
                        "Signal: CC1pi+, p_mu > 200 MeV, p_pi > 200 MeV\n"
                        ", costheta_mu > 0.2, costheta_pi > 0\n"
                        "https://arxiv.org/abs/1909.03936";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetTitle("T2K_CC1pip_CH_XSec_1Dthpi_nu");
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("#theta_{#pi} (radians)");
  fSettings.SetYTitle("d#sigma/d#theta_{#pi} (cm^{2}/radians/nucleon)");
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
                          "/data/T2K/CC1pip/CH/Thetapion.rootout.root",
                      "Theta_pion");
  SetCovarFromRootFile(GeneralUtils::GetTopLevelDir() +
                           "/data/T2K/CC1pip/CH/Thetapion.rootout.root",
                       "Theta_pionCov");

  SetShapeCovar();
  fDataHist->Scale(1E-38);

  FinaliseMeasurement();
};

void T2K_CC1pip_CH_XSec_1Dthpi_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(211) == 0)
    return;

  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;

  double thpi = FitUtils::th(Pnu, Ppip);

  fXVar = thpi;
};

//********************************************************************
bool T2K_CC1pip_CH_XSec_1Dthpi_nu::isSignal(FitEvent *event) {
  //********************************************************************
  return SignalDef::isCC1pip_T2K_arxiv1909_03936(
      event, EnuMin, EnuMax,
      SignalDef::kMuonHighEff | SignalDef::kPionFwd | SignalDef::kPionHighMom);
}
