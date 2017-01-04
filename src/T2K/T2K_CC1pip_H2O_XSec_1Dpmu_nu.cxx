#include "T2K_CC1pip_H2O_XSec_1Dpmu_nu.h"

// The muon momentum

//******************************************************************** 
T2K_CC1pip_H2O_XSec_1Dpmu_nu::T2K_CC1pip_H2O_XSec_1Dpmu_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){
//******************************************************************** 

  measurementName = "T2K_CC1pip_H2O_XSec_1Dpmu_nu";
  plotTitles = "; p_{#mu} (GeV/c); d#sigma/dp_{#mu} (cm^{2}/(GeV/c)/nucleon)";
  EnuMin = 0.;
  EnuMax = 10.;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Data comes in ROOT file
  // hResultTot is cross-section with all errors
  // hResultStat is cross-section with stats-only errors
  // hTruthNEUT is the NEUT cross-section given by experimenter
  // hTruthGENIE is the GENIE cross-section given by experimenter
  SetDataFromFile(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/H2O/nd280data-numu-cc1pi-xs-on-h2o-2015.root","MuMom/hResultTot");
  SetCovarFromDataFile(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/H2O/nd280data-numu-cc1pi-xs-on-h2o-2015.root", "MuMom/TotalCovariance");

  SetupDefaultHist();

  fScaleFactor = (fEventHist->Integral("width")*1E-38)/double(fNEvents)/TotalIntegratedFlux("width");
};

//******************************************************************** 
// Find the momentum of the muon
void T2K_CC1pip_H2O_XSec_1Dpmu_nu::FillEventVariables(FitEvent *event) {
//******************************************************************** 

  // Need to make sure there's a muon
  if (event->NumFSParticle(13) == 0) return;

  // Get the muon
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double p_mu = FitUtils::p(Pmu);

  this->X_VAR = p_mu;

  return;
};

//******************************************************************** 
// Beware: The H2O analysis has different signal definition to the CH analysis!
bool T2K_CC1pip_H2O_XSec_1Dpmu_nu::isSignal(FitEvent *event) {
//******************************************************************** 
  return SignalDef::isCC1pip_T2K_H2O(event, EnuMin, EnuMax);
}
