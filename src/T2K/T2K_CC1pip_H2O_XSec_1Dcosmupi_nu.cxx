#include "T2K_CC1pip_H2O_XSec_1Dcosmupi_nu.h"


//********************************************************************
T2K_CC1pip_H2O_XSec_1Dcosmupi_nu::T2K_CC1pip_H2O_XSec_1Dcosmupi_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "T2K_CC1pip_H2O_XSec_1Dcosmupi_nu sample. \n" \
                        "Target: CH \n" \
                        "Flux: T2k Forward Horn Current nue + nuebar \n" \
                        "Signal: Any event with 1 electron, any nucleons, and no other FS particles \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetTitle("T2K_CC1pip_H2O_XSec_1Dcosmupi_nu");
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("cos#theta_{#pi,#mu}");
  fSettings.SetYTitle("d#sigma/dcos#theta_{#pi#mu} (cm^{2}/nucleon)");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/DIAG");
  fSettings.SetEnuRange(0.0, 100.0);
  fSettings.DefineAllowedTargets("C,H");
  fSettings.DefineAllowedSpecies("numu");

  fSettings.SetDataInput(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/H2O/nd280data-numu-cc1pi-xs-on-h2o-2015.root;MuPiCos/hResultTot");
  fSettings.SetCovarInput(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/H2O/nd280data-numu-cc1pi-xs-on-h2o-2015.root;MuPiCos/TotalCovariance");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor =  GetEventHistogram()->Integral("width")*1E-38/double(fNEvents)/TotalIntegratedFlux("width");

  // Plot Setup -------------------------------------------------------
  SetDataFromRootFile(  fSettings.GetDataInput() );
  SetCovarFromRootFile( fSettings.GetCovarInput() );
  ScaleCovar(1E76);
  SetShapeCovar();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};

//********************************************************************
// Find the cos theta of the angle between muon and pion
void T2K_CC1pip_H2O_XSec_1Dcosmupi_nu::FillEventVariables(FitEvent *event) {
//********************************************************************

  // Need to make sure there's a muon
  if (event->NumFSParticle(13) == 0) return;
  // Need to make sure there's a pion
  if (event->NumFSParticle(211) == 0) return;

  // Get the muon
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;
  // Get the pion
  TLorentzVector Ppip  = event->GetHMFSParticle(211)->fP;

  double cos_th = cos(FitUtils::th(Pmu, Ppip));

  fXVar = cos_th;

  return;
};

//********************************************************************
// Beware: The H2O analysis has different signal definition to the CH analysis!
bool T2K_CC1pip_H2O_XSec_1Dcosmupi_nu::isSignal(FitEvent *event) {
//********************************************************************
  return SignalDef::isCC1pip_T2K_H2O(event, EnuMin, EnuMax);
}
