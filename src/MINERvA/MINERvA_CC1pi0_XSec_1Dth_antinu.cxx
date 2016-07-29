#include "MINERvA_CC1pi0_XSec_1Dth_antinu.h"

// The constructor
MINERvA_CC1pi0_XSec_1Dth_antinu::MINERvA_CC1pi0_XSec_1Dth_antinu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile) {

  measurementName = "MINERvA_CC1pi0_XSec_1Dth_nubar";
  plotTitles = "; #theta_{#pi} (degrees); d#sigma/d#theta_{#pi} (cm^{2}/degrees/nucleon)";
  EnuMin = 1.5;
  EnuMax = 10;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  if (type.find("NEW") != std::string::npos) {
    measurementName += "_2016";
    isNew = true;
    hadMassCut = 1800;
    isDiag = false;

    this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CC1pi0/2016_upd/cc1pi0_thpi.txt");

    // MINERvA mucked up the scaling in the data-release where everything was bin-width normalised to the first bin, not the nth bin
    double binOneWidth = dataHist->GetBinWidth(1);
    for (int i = 0; i < dataHist->GetNbinsX()+1; i++) {
      double binNWidth = dataHist->GetBinWidth(i+1);
      dataHist->SetBinContent(i+1, dataHist->GetBinContent(i+1)*1E-40);
      dataHist->SetBinError(i+1, dataHist->GetBinContent(i+1)*dataHist->GetBinError(i+1)/100.);
      dataHist->SetBinContent(i+1, dataHist->GetBinContent(i+1)*binOneWidth/binNWidth);
      dataHist->SetBinError(i+1, dataHist->GetBinError(i+1)*binOneWidth/binNWidth);
    }

    this->SetCovarMatrixFromText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CC1pi0/2016_upd/cc1pi0_thpi_corr.txt", dataHist->GetNbinsX());

  } else {

    // Although the covariance is given for MINERvA CC1pi0 nubar from 2015, it doesn't Cholesky decompose, hinting at something bad
    // I've tried adding small numbers to the diagonal but it still didn't work and the chi2s are crazy
    isDiag = true;
    normError = 0.15;
    // No hadronic mass cut on old publication
    hadMassCut = 99999;

    this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CC1pi0/ccpi0_th.csv");
    this->SetupDefaultHist();
 
    // Adjust MINERvA data to flux correction; roughly a 11% normalisation increase in data
    // Please change when MINERvA releases new data!
    for (int i = 0; i < dataHist->GetNbinsX() + 1; i++) {
      dataHist->SetBinContent(i+1, dataHist->GetBinContent(i+1)*1.11);
    }

    fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
    covar     = StatUtils::GetInvert(fullcovar);
  } // end special treatment depending on release year

  this->SetupDefaultHist();


  scaleFactor = this->eventHist->Integral("width")*double(1E-38)/double(nevents)/TotalIntegratedFlux("width");
};

void MINERvA_CC1pi0_XSec_1Dth_antinu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu = (event->PartInfo(0))->fP;
  TLorentzVector Pmu;
  TLorentzVector Ppi0;

  // Loop over the particle stack
  for (int j = 2; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0) continue;
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 111) {
      Ppi0 = event->PartInfo(j)->fP;
    } else if (PID == -13) {
      Pmu = event->PartInfo(j)->fP;
    }
  }

  double hadMass = FitUtils::Wrec(Pnu, Pmu);
  double th;

  if (hadMass > 100 && hadMass < hadMassCut) {
    th = (180./M_PI)*FitUtils::th(Pnu, Ppi0);
  } else {
    th = -999;
  }

  this->X_VAR = th;

  return;
};

//******************************************************************** 
bool MINERvA_CC1pi0_XSec_1Dth_antinu::isSignal(FitEvent *event) {
//******************************************************************** 
  return SignalDef::isCC1pi0Bar_MINERvA(event, EnuMin, EnuMax);
}
