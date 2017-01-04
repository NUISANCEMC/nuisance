#include "T2K_CC1pip_H2O_XSec_1Dppi_nu.h"

// The momentum of the (positive) pion

//******************************************************************** 
T2K_CC1pip_H2O_XSec_1Dppi_nu::T2K_CC1pip_H2O_XSec_1Dppi_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){
//******************************************************************** 

  measurementName = "T2K_CC1pip_H2O_XSec_1Dppi_nu";
  plotTitles = "; p_{#pi^{+}} (GeV/c); d#sigma/dp_{#pi^{+}} (cm^{2}/(GeV/c)/nucleon)";
  EnuMin = 0.;
  EnuMax = 10.;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Data comes in ROOT file
  // hResultTot is cross-section with all errors
  // hResultStat is cross-section with stats-only errors
  // hTruthNEUT is the NEUT cross-section given by experimenter
  // hTruthGENIE is the GENIE cross-section given by experimenter
  SetDataFromFile(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/H2O/nd280data-numu-cc1pi-xs-on-h2o-2015.root","PosPionMom/hResultTot");
  SetCovarFromDataFile(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/H2O/nd280data-numu-cc1pi-xs-on-h2o-2015.root", "PosPionMom/TotalCovariance");

  SetupDefaultHist();

  fScaleFactor = fEventHist->Integral("width")*1E-38/double(fNEvents)/TotalIntegratedFlux("width");
};

//******************************************************************** 
// Find the momentum of the (positively charged) pion
void T2K_CC1pip_H2O_XSec_1Dppi_nu::FillEventVariables(FitEvent *event) {
//******************************************************************** 

  // Need to make sure there's a muon
  if (event->NumFSParticle(211) == 0) return;

  // Get the pion
  TLorentzVector Ppip  = event->GetHMFSParticle(211)->fP;

  double p_pi = FitUtils::p(Ppip);

  fXVar = p_pi;

  return;
};

//******************************************************************** 
// Beware: The H2O analysis has different signal definition to the CH analysis!
bool T2K_CC1pip_H2O_XSec_1Dppi_nu::isSignal(FitEvent *event) {
//******************************************************************** 
  return SignalDef::isCC1pip_T2K_H2O(event, EnuMin, EnuMax);
}
