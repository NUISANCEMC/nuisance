#include "T2K_CC1pip_H2O_XSec_1Dpmu_nu.h"

// The muon momentum

//********************************************************************
T2K_CC1pip_H2O_XSec_1Dpmu_nu::T2K_CC1pip_H2O_XSec_1Dpmu_nu(nuiskey samplekey) {
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
  fSettings.SetTitle("T2K_CC1pip_H2O_XSec_1Dpmu_nu");
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("p_{#mu} (GeV)");
  fSettings.SetYTitle("d#sigma/dp_{#mu} (cm^{2}/GeV/nucleon)");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/DIAG");
  fSettings.SetEnuRange(0.0, 100.0);
  fSettings.DefineAllowedTargets("C,H");
  fSettings.DefineAllowedSpecies("numu");

  fSettings.SetDataInput(
      GeneralUtils::GetTopLevelDir() +
      "/data/T2K/CC1pip/H2O/nd280data-numu-cc1pi-xs-on-h2o-2015.root;MuMom/"
      "hResultTot");
  fSettings.SetCovarInput(
      GeneralUtils::GetTopLevelDir() +
      "/data/T2K/CC1pip/H2O/nd280data-numu-cc1pi-xs-on-h2o-2015.root;MuMom/"
      "TotalCovariance");

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
// Find the momentum of the muon
void T2K_CC1pip_H2O_XSec_1Dpmu_nu::FillEventVariables(FitEvent *event) {
  //********************************************************************

  // Need to make sure there's a muon
  if (event->NumFSParticle(13) == 0)
    return;

  // Get the muon
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;

  double p_mu = FitUtils::p(Pmu);

  fXVar = p_mu;

  return;
};

//********************************************************************
// Beware: The H2O analysis has different signal definition to the CH analysis!
bool T2K_CC1pip_H2O_XSec_1Dpmu_nu::isSignal(FitEvent *event) {
  //********************************************************************
  return SignalDef::isCC1pip_T2K_PRD97_012001(event, EnuMin, EnuMax);
}
