#include <iomanip>

#include "T2K_SignalDef.h"
#include "T2K_CC1pip_CH_XSec_1Dthpi_nu.h"

// The constructor
T2K_CC1pip_CH_XSec_1Dthpi_nu::T2K_CC1pip_CH_XSec_1Dthpi_nu(nuiskey samplekey) {

  // Sample overview ---------------------------------------------------
  std::string descrip = "T2K_CC1pip_CH_XSec_1Dthpi_nu sample. \n" \
                        "Target: CH \n" \
                        "Flux: T2K Forward Horn Current numu \n" \
                        "Signal: Any event with 1 muon -, 1 pion +, any nucleons, and no other FS particles \n";

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
  fScaleFactor =  (GetEventHistogram()->Integral("width") * 1E-38) / double(fNEvents) / TotalIntegratedFlux("width");

  // Plot Setup -------------------------------------------------------
  SetDataFromRootFile(GeneralUtils::GetTopLevelDir() + "/data/T2K/CC1pip/CH/Thetapion.rootout.root", "Theta_pion");
  SetCovarFromRootFile(GeneralUtils::GetTopLevelDir() + "/data/T2K/CC1pip/CH/Thetapion.rootout.root", "Theta_pionCov");
  
  SetShapeCovar();
  fDataHist->Scale(1E-38);

  FinaliseMeasurement();
};


void T2K_CC1pip_CH_XSec_1Dthpi_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(211) == 0) return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;

  double thpi = FitUtils::th(Pnu, Ppip);

  fXVar = thpi;
};

//********************************************************************
bool T2K_CC1pip_CH_XSec_1Dthpi_nu::isSignal(FitEvent *event) {
//********************************************************************
// This distribution uses a somewhat different signal definition so might as well implement it separately here

  if (!SignalDef::isCC1pi(event, 14, 211, EnuMin, EnuMax)) return false;

  TLorentzVector Pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;

  // If this event passes the criteria on particle counting, enforce the T2K
  // ND280 phase space constraints
  // Will be different if Michel tag sample is included or not
  // Essentially, if there's a Michel tag we don't cut on the pion variables

  double p_mu = FitUtils::p(Pmu) * 1000;
  double p_pi = FitUtils::p(Ppip) * 1000;
  double cos_th_mu = cos(FitUtils::th(Pnu, Pmu));
  double cos_th_pi = cos(FitUtils::th(Pnu, Ppip));

  if (p_mu <= 200 || cos_th_mu <= 0.2 || cos_th_pi <= 0.0 || p_pi <= 200) {
    return false;
  } else {
    return true;
  }

  return false;
}

