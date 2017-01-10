#include "T2K_CC1pip_H2O_XSec_1DEnuMB_nu.h"

// The derived neutrino energy using the "MiniBooNE formula" (see paper)
// Essentially this is a proxy for the neutrino energy, using the outgoing pion and muon to get the reconstructed neutrino energy, assuming the struck nucleon was at rest
// Again, THIS IS NOT A "TRUE" NEUTRINO ENERGY!

//********************************************************************
T2K_CC1pip_H2O_XSec_1DEnuMB_nu::T2K_CC1pip_H2O_XSec_1DEnuMB_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){
//********************************************************************

  fName = "T2K_CC1pip_H2O_XSec_1DEnuMB_nu";
  fPlotTitles = "; E_{#nu} (GeV); #sigma(E_{#nu}) (cm^{2}/nucleon)";
  EnuMin = 0.;
  EnuMax = 100.;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Data comes in ROOT file
  // hResultTot is cross-section with all errors
  // hResultStat is cross-section with stats-only errors
  // hTruthNEUT is the NEUT cross-section given by experimenter
  // hTruthGENIE is the GENIE cross-section given by experimenter
  SetDataFromFile(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/H2O/nd280data-numu-cc1pi-xs-on-h2o-2015.root","EnuRec_MB/hResultTot");
  SetCovarFromDataFile(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/H2O/nd280data-numu-cc1pi-xs-on-h2o-2015.root", "EnuRec_MB/TotalCovariance", true);

  SetupDefaultHist();

  fScaleFactor = (GetEventHistogram()->Integral("width")*1E-38)/double(fNEvents);
};

//********************************************************************
// Find the derived neutrino energy using the "MiniBooNE formula" (see paper)
// Essentially uses the pion and muon kinematics to derive a pseudo-neutrino energy, assuming the struck nucleon is at rest
// We also need the incoming neutrino to get the muon/neutrino and pion/neutrino angles
void T2K_CC1pip_H2O_XSec_1DEnuMB_nu::FillEventVariables(FitEvent *event) {
//********************************************************************

  // Need to make sure there's a muon
  if (event->NumFSParticle(13) == 0) return;
  // Need to make sure there's a pion
  if (event->NumFSParticle(211) == 0) return;

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
  return SignalDef::isCC1pip_T2K_H2O(event, EnuMin, EnuMax);
}
