#include <iomanip>

#include "T2K_SignalDef.h"

#include "T2K_CC1pip_CH_XSec_1DQ2_nu.h"

// The constructor
T2K_CC1pip_CH_XSec_1DQ2_nu::T2K_CC1pip_CH_XSec_1DQ2_nu(nuiskey samplekey) {

  // Sample overview ---------------------------------------------------
  std::string descrip = "T2K_CC1pip_CH_XSec_1DQ2_nu sample. \n" \
                        "Target: CH \n" \
                        "Flux: T2K Forward Horn Current numu \n" \
                        "Signal: Any event with 1 muon -, 1 pion +, any nucleons, and no other FS particles \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetTitle("T2K_CC1pip_CH_XSec_1DQ2_nu");
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("Q^{2} (GeV/c)^{2}");
  fSettings.SetYTitle("d#sigma/dQ^{2} (cm^{2}/GeV^{2}/nucleon)");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/DIAG");
  fSettings.SetEnuRange(0.0, 100.0);
  fSettings.DefineAllowedTargets("C,H");
  fSettings.DefineAllowedSpecies("numu");
  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor =  (GetEventHistogram()->Integral("width") * 1E-38) / double(fNEvents) / TotalIntegratedFlux("width");

  // Plot Setup -------------------------------------------------------
  SetDataFromRootFile(GeneralUtils::GetTopLevelDir() + "/data/T2K/CC1pip/CH/Q2.rootout.root", "Q2");
  SetCovarFromRootFile(GeneralUtils::GetTopLevelDir() + "/data/T2K/CC1pip/CH/Q2.rootout.root", "Q2Cov");
  
  SetShapeCovar();
  fDataHist->Scale(1E-38);

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};


void T2K_CC1pip_CH_XSec_1DQ2_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(13) == 0 ||
      event->NumFSParticle(211) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double q2 = -999;

  //switch(fT2KSampleType) {

    // First int refers to how we reconstruct Enu
    // 0 uses true neutrino energy (not used here but common for other analyses when they unfold to true Enu from reconstructed Enu)
    // 1 uses "extended MiniBooNE" method
    // 2 uses "MiniBooNE reconstructed" method
    // 3 uses Delta resonance mass for reconstruction
    //
    // The last bool refers to if pion directional information was used
    //
    // Use MiniBooNE reconstructed Enu; uses Michel tag so no pion direction information
    //case kMB:
      //q2 = FitUtils::Q2CC1piprec(Pnu, Pmu, Ppip, 2, false);
      //break;

    // Use Extended MiniBooNE reconstructed Enu
    // Needs pion information to reconstruct so bool is true (did not include Michel e tag)
    //case keMB:
      q2 = FitUtils::Q2CC1piprec(Pnu, Pmu, Ppip, 1, true);
      //break;

    // Use Delta resonance reconstructed Enu
    // Uses Michel electron so don't have pion directional information
    //case kDelta:
      //q2 = FitUtils::Q2CC1piprec(Pnu, Pmu, Ppip, 3, false);
      //break;
  //}


  fXVar = q2;

  return;
};

//********************************************************************
bool T2K_CC1pip_CH_XSec_1DQ2_nu::isSignal(FitEvent *event) {
//********************************************************************
// Warning: The CH analysis has different signal definition to the H2O analysis!
//          Often to do with the Michel tag

  //switch(fT2KSampleType) {
    // Using MiniBooNE formula for Enu reconstruction on the Q2 variable
    // Does have Michel e tag, set bool to true!
    //case kMB:
      //return SignalDef::isCC1pip_T2K_CH(event, EnuMin, EnuMax, true);
      //break;
    // Using extended MiniBooNE formula for Enu reconstruction on the Q2 variable
    // Does not have Michel e tag because we need directional information to reconstruct Q2
    //case keMB:
      return SignalDef::isCC1pip_T2K_CH(event, EnuMin, EnuMax, false);
      //break;
    // Using Delta resonance for Enu reconstruction on the Q2 variable
    // Does have Michel e tag, bool to true
    //case kDelta:
      //return SignalDef::isCC1pip_T2K_CH(event, EnuMin, EnuMax, true);
      //break;
  //}

  // Default to return false
  //return false;
}

