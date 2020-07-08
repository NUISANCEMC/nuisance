#include "T2K_CC1pip_H2O_XSec_1Dcospi_nu.h"

//********************************************************************
T2K_CC1pip_H2O_XSec_1Dcospi_nu::T2K_CC1pip_H2O_XSec_1Dcospi_nu(
    nuiskey samplekey) {
  //********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip =
      "T2K_CC1pip_H2O_XSec_nu sample. \n"
      "Target: H20 \n"
      "Flux: T2K FHC numu \n"
      "Signal: CC1pi+, p_mu > 200 MeV, p_pi > 200 MeV\n"
      ", costheta_mu > 0.3, costheta_pi > 0.3\n"
      "https://journals.aps.org/prd/pdf/10.1103/PhysRevD.95.012010";
  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetTitle("T2K_CC1pip_H2O_XSec_1Dcospi_nu");
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("cos#theta_{#nu,#pi}");
  fSettings.SetYTitle("d#sigma/dcos#theta_{#nu,#pi} (cm^{2}/nucleon)");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/DIAG");
  fSettings.SetEnuRange(0.0, 100.0);
  fSettings.DefineAllowedTargets("C,H");
  fSettings.DefineAllowedSpecies("numu");

  fSettings.SetDataInput(
      GeneralUtils::GetTopLevelDir() +
      "/data/T2K/CC1pip/H2O/"
      "nd280data-numu-cc1pi-xs-on-h2o-2015.root;PosPionCos/hResultTot");
  fSettings.SetCovarInput(
      GeneralUtils::GetTopLevelDir() +
      "/data/T2K/CC1pip/H2O/"
      "nd280data-numu-cc1pi-xs-on-h2o-2015.root;PosPionCos/TotalCovariance");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = (GetEventHistogram()->Integral("width") * 1E-38) /
                 double(fNEvents) / TotalIntegratedFlux("width");

  // Plot Setup -------------------------------------------------------
  SetDataFromRootFile(fSettings.GetDataInput());
  SetCovarFromRootFile(fSettings.GetCovarInput());
  ScaleCovar(1E76);
  SetShapeCovar();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

//********************************************************************
// Find the cos theta of the angle between pion and neutrino
void T2K_CC1pip_H2O_XSec_1Dcospi_nu::FillEventVariables(FitEvent *event) {
  //********************************************************************

  // Need to make sure there's a pion
  if (event->NumFSParticle(211) == 0)
    return;

  // Get the incoming neutrino
  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  // Get the pion
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;

  double cos_th = cos(FitUtils::th(Pnu, Ppip));

  fXVar = cos_th;

  return;
};

//********************************************************************
// Beware: The H2O analysis has different signal definition to the CH analysis!
bool T2K_CC1pip_H2O_XSec_1Dcospi_nu::isSignal(FitEvent *event) {
  //********************************************************************
  return SignalDef::isCC1pip_T2K_PRD97_012001(event, EnuMin, EnuMax);
}
