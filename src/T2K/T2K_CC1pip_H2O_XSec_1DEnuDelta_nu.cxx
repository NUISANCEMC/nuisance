#include "T2K_CC1pip_H2O_XSec_1DEnuDelta_nu.h"

// The derived neutrino energy assuming a Delta resonance and a nucleon at rest; so only requires the outgoing muon to derive (and information on the angle between the muon and the neutrino)
// Please beware that this is NOT THE "TRUE" NEUTRINO ENERGY; IT'S A PROXY FOR THE TRUE NEUTRINO ENERGY
// Also, this is flux-integrated cross-section, not flux averaged

//********************************************************************
T2K_CC1pip_H2O_XSec_1DEnuDelta_nu::T2K_CC1pip_H2O_XSec_1DEnuDelta_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){
//********************************************************************
  fName = "T2K_CC1pip_H2O_XSec_1DEnuDelta_nu";
  fPlotTitles = "; E^{#Delta}_{#nu} (GeV); #sigma(E^{#Delta}_{#nu}) (cm^{2}/nucleon)";
  EnuMin = 0.;
  EnuMax = 100.;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Data comes in ROOT file
  // hResultTot is cross-section with all errors
  // hResultStat is cross-section with stats-only errors
  // hTruthNEUT is the NEUT cross-section given by experimenter
  // hTruthGENIE is the GENIE cross-section given by experimenter
  SetDataFromFile(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/H2O/nd280data-numu-cc1pi-xs-on-h2o-2015.root","EnuRec_Delta/hResultTot");
  SetCovarFromDataFile(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/H2O/nd280data-numu-cc1pi-xs-on-h2o-2015.root", "EnuRec_Delta/TotalCovariance", true);

  SetupDefaultHist();

  fScaleFactor = GetEventHistogram()->Integral("width")*1E-38/double(fNEvents);
};


//********************************************************************
// Find the muon whows kinematics we use to derive the "neutrino energy"
void T2K_CC1pip_H2O_XSec_1DEnuDelta_nu::FillEventVariables(FitEvent *event) {
//********************************************************************

  // Need to make sure there's a muon
  if (event->NumFSParticle(13) == 0) return;

  // Get the incoming neutrino
  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  // Get the muon
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double Enu = FitUtils::EnuCC1piprecDelta(Pnu, Pmu);

  fXVar = Enu;

  return;
};

//********************************************************************
// Beware: The H2O analysis has different signal definition to the CH analysis!
bool T2K_CC1pip_H2O_XSec_1DEnuDelta_nu::isSignal(FitEvent *event) {
//********************************************************************
  return SignalDef::isCC1pip_T2K_H2O(event, EnuMin, EnuMax);
}
