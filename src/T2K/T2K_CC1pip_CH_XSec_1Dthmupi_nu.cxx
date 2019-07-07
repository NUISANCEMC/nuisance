#include <iomanip>

#include "T2K_SignalDef.h"
#include "T2K_CC1pip_CH_XSec_1Dthmupi_nu.h"

// The constructor
T2K_CC1pip_CH_XSec_1Dthmupi_nu::T2K_CC1pip_CH_XSec_1Dthmupi_nu(nuiskey samplekey) {

  // Sample overview ---------------------------------------------------
  std::string descrip = "T2K_CC1pip_CH_XSec_1Dthmupi_nu sample. \n" \
                        "Target: CH \n" \
                        "Flux: T2K Forward Horn Current numu \n" \
                        "Signal: Any event with 1 muon -, 1 pion +, any nucleons, and no other FS particles \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetTitle("T2K_CC1pip_CH_XSec_1Dthmupi_nu");
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("#theta_{#mu,#pi} (radians)");
  fSettings.SetYTitle("d#sigma/d#theta_{#mu,#pi} (cm^{2}/radians/nucleon)");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/DIAG");
  fSettings.SetEnuRange(0.0, 100.0);
  fSettings.DefineAllowedTargets("C,H");
  fSettings.DefineAllowedSpecies("numu");
  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor =  (GetEventHistogram()->Integral("width") * 1E-38) / double(fNEvents) / TotalIntegratedFlux("width");

  // Plot Setup -------------------------------------------------------
  SetDataFromRootFile(GeneralUtils::GetTopLevelDir() + "/data/T2K/CC1pip/CH/Thetapimu.rootout.root", "Theta(pi,mu)(rads)");
  SetCovarFromRootFile(GeneralUtils::GetTopLevelDir() + "/data/T2K/CC1pip/CH/Thetapimu.rootout.root", "Theta(pi,mu)(rads)Cov");
  
  SetShapeCovar();
  fDataHist->Scale(1E-38);

  FinaliseMeasurement();
};

void T2K_CC1pip_CH_XSec_1Dthmupi_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(13) == 0 ||
      event->NumFSParticle(211) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double thmupi = FitUtils::th(Pmu, Ppip);

  fXVar = thmupi;
  //std::cout << thmupi << std::endl;

  return;
};

//********************************************************************
bool T2K_CC1pip_CH_XSec_1Dthmupi_nu::isSignal(FitEvent *event) {
//********************************************************************
// This distribution uses a somewhat different signal definition so might as well implement it separately here
  return SignalDef::isCC1pip_T2K_CH(event, EnuMin, EnuMax, false);
}

