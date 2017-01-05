#include "T2K_CC1pip_H2O_XSec_1Dcospi_nu.h"

// The cos of the angle between the pion and the neutrino

//******************************************************************** 
T2K_CC1pip_H2O_XSec_1Dcospi_nu::T2K_CC1pip_H2O_XSec_1Dcospi_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){
//******************************************************************** 

  fName = "T2K_CC1pip_H2O_XSec_1Dcospi_nu";
  fPlotTitles = "; cos#theta_{#pi}; d#sigma/dcos#theta_{#pi} (cm^{2}/nucleon)";
  EnuMin = 0.;
  EnuMax = 10.;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Data comes in ROOT file
  // hResultTot is cross-section with all errors
  // hResultStat is cross-section with stats-only errors
  // hTruthNEUT is the NEUT cross-section given by experimenter
  // hTruthGENIE is the GENIE cross-section given by experimenter
  SetDataFromFile(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/H2O/nd280data-numu-cc1pi-xs-on-h2o-2015.root","PosPionCos/hResultTot");
  SetCovarFromDataFile(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/H2O/nd280data-numu-cc1pi-xs-on-h2o-2015.root", "PosPionCos/TotalCovariance", true);

  SetupDefaultHist();

  fScaleFactor = (fEventHist->Integral("width")*1E-38)/double(fNEvents)/TotalIntegratedFlux("width");
};

//******************************************************************** 
// Find the cos theta of the angle between pion and neutrino
void T2K_CC1pip_H2O_XSec_1Dcospi_nu::FillEventVariables(FitEvent *event) {
//******************************************************************** 

  // Need to make sure there's a pion
  if (event->NumFSParticle(211) == 0) return;

  // Get the incoming neutrino
  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  // Get the pion
  TLorentzVector Ppip  = event->GetHMFSParticle(211)->fP;

  double cos_th = cos(FitUtils::th(Pnu, Ppip));

  fXVar = cos_th;

  return;
};

//******************************************************************** 
// Beware: The H2O analysis has different signal definition to the CH analysis!
bool T2K_CC1pip_H2O_XSec_1Dcospi_nu::isSignal(FitEvent *event) {
//******************************************************************** 
  return SignalDef::isCC1pip_T2K_H2O(event, EnuMin, EnuMax);
}
