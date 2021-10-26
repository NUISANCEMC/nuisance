#include "T2K_CCCOH_C12_XSec_1DEnu_nu.h"

// The constructor
T2K_CCCOH_C12_XSec_1DEnu_nu::T2K_CCCOH_C12_XSec_1DEnu_nu(nuiskey samplekey) {

  // Sample overview ---------------------------------------------------
  std::string descrip = "T2K_CCCOH_C12_XSec_nu sample. \n"
                        "Target: C12 \n"
                        "Flux: T2K FHC numu \n"
                        "Signal: CCCOHpi+, p_mu,pi > 0.18 GeV, theta_mu,pi < 70 deg\n"
                        ", p_pi < 1.6 GeV\n"
                        "https://arxiv.org/abs/1604.04406";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetTitle("T2K_CCCOH_C12_XSec_1DEnu_nu");
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("E_{#nu} (GeV)");
  fSettings.SetYTitle("d#sigma/dE_{#nu} (cm^{2}/^{12}C)");
  fSettings.SetEnuRange(0.0, 100.0);
  fSettings.DefineAllowedTargets("C");
  fSettings.DefineAllowedSpecies("numu");
  FinaliseSampleSettings();

  // Override the default
  fMCFine = new TH1D("mcfine", "mcfine", 50, 0, 5);

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = 12*(GetEventHistogram()->Integral("width") * 1E-38) /
                 double(fNEvents) / TotalIntegratedFlux("width");

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile(GeneralUtils::GetTopLevelDir()+"/data/T2K/CCCOH/C12_Enu_1bin.txt");

  // Slight hack to get single bin comparisons recognised
  this->SetFitOptions("SINGLEBIN");

  FinaliseMeasurement();
};

void T2K_CCCOH_C12_XSec_1DEnu_nu::FillEventVariables(FitEvent *event) {
  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  fXVar = Pnu.E()/1000.;
  return;
};


bool T2K_CCCOH_C12_XSec_1DEnu_nu::isSignal(FitEvent *event) {

  if (!SignalDef::isCCCOH(event, 14, 211))
    return false;

  TLorentzVector Pnu  = event->GetHMISParticle(14)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;

  double p_mu  = FitUtils::p(Pmu);
  double p_pi  = FitUtils::p(Ppip);
  double th_mu = FitUtils::th(Pnu, Pmu)* 180 / TMath::Pi();
  double th_pi = FitUtils::th(Pnu, Ppip)* 180 / TMath::Pi();

  if (p_mu < 0.18 || p_pi < 0.18 || p_pi > 1.6 || th_mu > 70 || th_pi > 70) {
    return false;
  }
  return true;
}
