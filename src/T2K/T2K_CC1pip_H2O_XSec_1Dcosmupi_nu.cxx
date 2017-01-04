#include "T2K_CC1pip_H2O_XSec_1Dcosmupi_nu.h"

// The cosine of the angle between the muon and pion

//******************************************************************** 
T2K_CC1pip_H2O_XSec_1Dcosmupi_nu::T2K_CC1pip_H2O_XSec_1Dcosmupi_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){
//******************************************************************** 

  measurementName = "T2K_CC1pip_H2O_XSec_1Dcosmupi_nu";
  plotTitles = "; cos#theta_{#pi,#mu}; d#sigma/dcos#theta_{#pi#mu} (cm^{2}/nucleon)";
  EnuMin = 0;
  EnuMax = 10;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Data comes in ROOT file
  // hResultTot is cross-section with all errors
  // hResultStat is cross-section with stats-only errors
  // hTruthNEUT is the NEUT cross-section given by experimenter
  // hTruthGENIE is the GENIE cross-section given by experimenter
  this->SetDataFromFile(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/H2O/nd280data-numu-cc1pi-xs-on-h2o-2015.root","MuPiCos/hResultTot");
  this->SetCovarFromDataFile(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/H2O/nd280data-numu-cc1pi-xs-on-h2o-2015.root", "MuPiCos/TotalCovariance");

  this->SetupDefaultHist();

  this->scaleFactor = fEventHist->Integral("width")*1E-38/double(fNEvents)/TotalIntegratedFlux("width");
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
