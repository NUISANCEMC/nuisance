#include "T2K_CC1pip_H2O_XSec_1DEnuMB_nu.h"

// The derived neutrino energy using the "MiniBooNE formula" (see paper)
// Essentially this is a proxy for the neutrino energy, using the outgoing pion
// and muon to get the reconstructed neutrino energy, assuming the struck
// nucleon was at rest Again, THIS IS NOT A "TRUE" NEUTRINO ENERGY!

//********************************************************************
T2K_CC1pip_H2O_XSec_1DEnuMB_nu::T2K_CC1pip_H2O_XSec_1DEnuMB_nu(
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
  fSettings.SetTitle("T2K_CC1pip_H2O_XSec_1DEnuMB_nu");
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("E_{#nu}^{MiniBooNE} (GeV)");
  fSettings.SetYTitle("#sigma(E_{#nu}^{MiniBooNE}) (cm^{2}/nucleon)");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/DIAG");
  fSettings.SetEnuRange(0.0, 100.0);
  fSettings.DefineAllowedTargets("C,H");
  fSettings.DefineAllowedSpecies("numu");

  fSettings.SetDataInput(
      GeneralUtils::GetTopLevelDir() +
      "/data/T2K/CC1pip/H2O/nd280data-numu-cc1pi-xs-on-h2o-2015.root;EnuRec_MB/"
      "hResultTot");
  fSettings.SetCovarInput(
      GeneralUtils::GetTopLevelDir() +
      "/data/T2K/CC1pip/H2O/nd280data-numu-cc1pi-xs-on-h2o-2015.root;EnuRec_MB/"
      "TotalCovariance");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor =
      (GetEventHistogram()->Integral("width") * 1E-38) / double(fNEvents);

  // Plot Setup -------------------------------------------------------
  SetDataFromRootFile(fSettings.GetDataInput());
  SetCovarFromRootFile(fSettings.GetCovarInput());
  ScaleCovar(1E76);
  SetShapeCovar();
  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

//********************************************************************
// Find the derived neutrino energy using the "MiniBooNE formula" (see paper)
// Essentially uses the pion and muon kinematics to derive a pseudo-neutrino
// energy, assuming the struck nucleon is at rest We also need the incoming
// neutrino to get the muon/neutrino and pion/neutrino angles
void T2K_CC1pip_H2O_XSec_1DEnuMB_nu::FillEventVariables(FitEvent *event) {
  //********************************************************************

  // Need to make sure there's a muon
  if (event->NumFSParticle(13) == 0)
    return;
  // Need to make sure there's a pion
  if (event->NumFSParticle(211) == 0)
    return;

  // Get the incoming neutrino
  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  // Get the muon
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;
  // Get the pion
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;

  double Enu = FitUtils::EnuCC1piprec(Pnu, Pmu, Ppip);

  fXVar = Enu;

  return;
};

//********************************************************************
// Beware: The H2O analysis has different signal definition to the CH analysis!
bool T2K_CC1pip_H2O_XSec_1DEnuMB_nu::isSignal(FitEvent *event) {
  //********************************************************************
  return SignalDef::isCC1pip_T2K_PRD97_012001(event, EnuMin, EnuMax);
}
