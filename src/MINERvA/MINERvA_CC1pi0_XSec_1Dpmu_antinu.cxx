#include "MINERvA_CC1pi0_XSec_1Dpmu_antinu.h"

// The constructor
MINERvA_CC1pi0_XSec_1Dpmu_antinu::MINERvA_CC1pi0_XSec_1Dpmu_antinu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  measurementName = "MINERvA_CC1pi0_XSec_1Dpmu_nubar_2016";
  plotTitles = "; p_{#mu} (GeV); d#sigma/dp_{#mu} (cm^{2}/GeV/nucleon)";
  EnuMin = 1.5;
  EnuMax = 10;
  isDiag = false;
  allowed_types += "NEW";

  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CC1pi0/2016_upd/cc1pi0_pmu.txt");

  // MINERvA mucked up the scaling in the data-release where everything was bin-width normalised to the first bin, not the nth bin
  double binOneWidth = dataHist->GetBinWidth(1);
  // Scale data to proper cross-section
  for (int i = 0; i < dataHist->GetNbinsX()+1; i++) {
    double binNWidth = dataHist->GetBinWidth(i+1);
    dataHist->SetBinContent(i+1, dataHist->GetBinContent(i+1)*1E-40);
    dataHist->SetBinError(i+1, dataHist->GetBinContent(i+1)*dataHist->GetBinError(i+1)/100.);
    dataHist->SetBinContent(i+1, dataHist->GetBinContent(i+1)*binOneWidth/binNWidth);
    dataHist->SetBinError(i+1, dataHist->GetBinError(i+1)*binOneWidth/binNWidth);
  }

  this->SetCovarMatrixFromText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CC1pi0/2016_upd/cc1pi0_pmu_corr.txt", dataHist->GetNbinsX());

  this->SetupDefaultHist();

  this->scaleFactor = this->eventHist->Integral("width")*double(1E-38)/double(nevents)/TotalIntegratedFlux("width");
};

void MINERvA_CC1pi0_XSec_1Dpmu_antinu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu = (event->PartInfo(0))->fP;
  TLorentzVector Pmu;
  TLorentzVector Ppi0;

  // Loop over the particle stack
  for (unsigned int j = 2; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0) continue;
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 111) {
      Ppi0 = event->PartInfo(j)->fP;
    } else if (PID == -13) {
      Pmu = event->PartInfo(j)->fP;
    }
  }

  double hadMass = FitUtils::Wrec(Pnu, Pmu);
  double pmu;

  if (hadMass > 100 && hadMass < 1800) {
    pmu = FitUtils::p(Pmu);
  } else {
    pmu = -999;
  }

  this->X_VAR = pmu;

  return;
};

//******************************************************************** 
bool MINERvA_CC1pi0_XSec_1Dpmu_antinu::isSignal(FitEvent *event) {
//******************************************************************** 
  return SignalDef::isCC1pi0Bar_MINERvA(event, EnuMin, EnuMax);
}
