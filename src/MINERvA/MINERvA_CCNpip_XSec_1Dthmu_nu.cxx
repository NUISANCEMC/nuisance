#include "MINERvA_CCNpip_XSec_1Dthmu_nu.h"

// The constructor
MINERvA_CCNpip_XSec_1Dthmu_nu::MINERvA_CCNpip_XSec_1Dthmu_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){

  fName = "MINERvA_CCNpip_XSec_1Dthmu_nu_2016";
  fPlotTitles = "; #theta_{#mu} (degrees); d#sigma/d#theta_{#mu} (cm^{2}/degrees/nucleon)";
  EnuMin = 1.5;
  EnuMax = 10;
  isDiag = false;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2016_upd/ccnpip_thmu.txt");

  // MINERvA mucked up the scaling in the data-release where everything was bin-width normalised to the first bin, not the nth bin
  double binOneWidth = fDataHist->GetBinWidth(1);
  for (int i = 0; i < fDataHist->GetNbinsX()+1; i++) {
    double binNWidth = fDataHist->GetBinWidth(i+1);
    fDataHist->SetBinContent(i+1, fDataHist->GetBinContent(i+1)*1E-40);
    fDataHist->SetBinError(i+1, fDataHist->GetBinContent(i+1)*fDataHist->GetBinError(i+1)/100.);
    fDataHist->SetBinContent(i+1, fDataHist->GetBinContent(i+1)*binOneWidth/binNWidth);
    fDataHist->SetBinError(i+1, fDataHist->GetBinError(i+1)*binOneWidth/binNWidth);
  }

  // This is a correlation matrix
  this->SetCovarMatrixFromText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2016_upd/ccnpip_thmu_corr.txt", fDataHist->GetNbinsX());

  this->SetupDefaultHist();

  scaleFactor = this->eventHist->Integral("width")*double(1E-38)/double(nevents)/TotalIntegratedFlux("width");
};

void MINERvA_CCNpip_XSec_1Dthmu_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu = (event->PartInfo(0))->fP;
  TLorentzVector Ppip;
  TLorentzVector Pmu;

  // Loop over the particle stack
  for (unsigned int j = 2; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0) continue;
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 211 && event->PartInfo(j)->fP.E() > Ppip.E()) {
      Ppip = event->PartInfo(j)->fP;
    } else if (PID == 13) {
      Pmu = (event->PartInfo(j))->fP;  
    }
  }

  double hadMass = FitUtils::Wrec(Pnu, Pmu);
  double thmu;

  // Include up to some given hadronic mass cut
  if (hadMass > 100 && hadMass < 1800) {
    thmu = (180./M_PI)*FitUtils::th(Pnu, Pmu);
  } else {
    thmu = -999;
  }

  fXVar = thmu;

  return;
};

//******************************************************************** 
bool MINERvA_CCNpip_XSec_1Dthmu_nu::isSignal(FitEvent *event) {
//******************************************************************** 
  // Last false refers to that this is NOT the restricted MINERvA phase space, in which only forward-going muons are accepted
  int dummy;
  return SignalDef::isCCNpip_MINERvA(event, dummy, EnuMin, EnuMax, false);
}
