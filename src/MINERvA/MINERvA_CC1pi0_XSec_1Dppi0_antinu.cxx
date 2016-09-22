#include "MINERvA_CC1pi0_XSec_1Dppi0_antinu.h"

// The constructor
MINERvA_CC1pi0_XSec_1Dppi0_antinu::MINERvA_CC1pi0_XSec_1Dppi0_antinu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  EnuMin = 1.5;
  EnuMax = 10;
  allowed_types += "NEW";

  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  if (type.find("NEW") != std::string::npos) {
    // NEW measurement is actually Tpi, not ppi
    measurementName = "MINERvA_CC1pi0_XSec_1DTpi_nubar_2016";
    plotTitles = "; T_{#pi} (GeV); d#sigma/dT_{#pi} (cm^{2}/GeV/nucleon)";
    isNew = true;
    hadMassCut = 1800;
    isDiag = false;

    this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CC1pi0/2016_upd/cc1pi0_tpi.txt");

    // MINERvA mucked up the scaling in the data-release where everything was bin-width normalised to the first bin, not the nth bin
    double binOneWidth = dataHist->GetBinWidth(1);
    for (int i = 0; i < dataHist->GetNbinsX()+1; i++) {
      double binNWidth = dataHist->GetBinWidth(i+1);
      dataHist->SetBinContent(i+1, dataHist->GetBinContent(i+1)*1E-40);
      dataHist->SetBinError(i+1, dataHist->GetBinContent(i+1)*dataHist->GetBinError(i+1)/100.);
      dataHist->SetBinContent(i+1, dataHist->GetBinContent(i+1)*binOneWidth/binNWidth);
      dataHist->SetBinError(i+1, dataHist->GetBinError(i+1)*binOneWidth/binNWidth);
    }

    this->SetCovarMatrixFromText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CC1pi0/2016_upd/cc1pi0_tpi_corr.txt", dataHist->GetNbinsX());

  } else {
    measurementName = "MINERvA_CC1pi0_XSec_1Dppi0_nubar";
    plotTitles = "; p_{#pi^{0}} (GeV/c); d#sigma/dp_{#pi^{0}} (cm^{2}/(GeV/c)/nucleon)";
    // Although the covariance is given for MINERvA CC1pi0 nubar from 2015, it doesn't Cholesky decompose, hinting at something bad
    // I've tried adding small numbers to the diagonal but it still didn't work and the chi2s are crazy
    isDiag = true;
    normError = 0.15;
    // No hadronic mass cut on old publication
    hadMassCut = 99999;

    this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CC1pi0/ccpi0_ppi0.csv");

    // Adjust MINERvA data to flux correction; roughly a 11% normalisation increase in data
    // Please change when MINERvA releases new data!
    for (int i = 0; i < dataHist->GetNbinsX() + 1; i++) {
      dataHist->SetBinContent(i+1, dataHist->GetBinContent(i+1)*1.11);
    }

    fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
    covar     = StatUtils::GetInvert(fullcovar);

  }

  this->SetupDefaultHist();

  this->scaleFactor = this->eventHist->Integral("width")*double(1E-38)/double(nevents)/TotalIntegratedFlux("width");
};

void MINERvA_CC1pi0_XSec_1Dppi0_antinu::FillEventVariables(FitEvent *event) {

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
  double ppi0 = -999;

  if (hadMass > 100 && hadMass < hadMassCut) {
    // 2016 does pion kinetic energy in GeV
    // 2015 does pion momentum in GeV
    if (isNew) {
      ppi0 = FitUtils::T(Ppi0);
    } else {
      ppi0 = FitUtils::p(Ppi0);
    }
  }

  this->X_VAR = ppi0;

  return;
};

//******************************************************************** 
bool MINERvA_CC1pi0_XSec_1Dppi0_antinu::isSignal(FitEvent *event) {
//******************************************************************** 
  return SignalDef::isCC1pi0Bar_MINERvA(event, EnuMin, EnuMax);
}
