#include "MINERvA_CC1pi0_XSec_1Dppi0_antinu.h"

// The constructor
MINERvA_CC1pi0_XSec_1Dppi0_antinu::MINERvA_CC1pi0_XSec_1Dppi0_antinu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  EnuMin = 1.5;
  EnuMax = 10;
  fAllowedTypes += "NEW";

  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  if (type.find("NEW") != std::string::npos) {
    // NEW measurement is actually Tpi, not ppi
    fName = "MINERvA_CC1pi0_XSec_1DTpi_nubar_2016";
    fPlotTitles = "; T_{#pi} (GeV); d#sigma/dT_{#pi} (cm^{2}/GeV/nucleon)";
    isNew = true;
    hadMassCut = 1800;
    fIsDiag = false;

    //this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CC1pi0/2016/cc1pi0_tpi.txt");
    this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CC1pi0/2016/anu-cc1pi0-xsec-pion-kinetic-energy.csv");

    // Error is given as percentage of cross-section
    // Need to scale the bin error properly before we do correlation -> covariance conversion
    for (int i = 0; i < fDataHist->GetNbinsX()+1; i++) {
      fDataHist->SetBinError(i+1, fDataHist->GetBinContent(i+1)*fDataHist->GetBinError(i+1)/100.);
    }

    //this->SetCovarMatrixFromText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CC1pi0/2016/cc1pi0_tpi_corr.txt", fDataHist->GetNbinsX());
    this->SetCovarMatrixFromCorrText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CC1pi0/2016/anu-cc1pi0-correlation-pion-kinetic-energy.csv", fDataHist->GetNbinsX());

  } else {
    fName = "MINERvA_CC1pi0_XSec_1Dppi0_nubar";
    fPlotTitles = "; p_{#pi^{0}} (GeV/c); d#sigma/dp_{#pi^{0}} (cm^{2}/(GeV/c)/nucleon)";
    // Although the covariance is given for MINERvA CC1pi0 nubar from 2015, it doesn't Cholesky decompose, hinting at something bad
    // I've tried adding small numbers to the diagonal but it still didn't work and the chi2s are crazy
    fIsDiag = true;
    fNormError = 0.15;
    // No hadronic mass cut on old publication
    hadMassCut = 99999;

    this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CC1pi0/2015/ccpi0_ppi0.csv");

    // Adjust MINERvA data to flux correction; roughly a 11% normalisation increase in data
    // Please change when MINERvA releases new data!
    for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
      fDataHist->SetBinContent(i+1, fDataHist->GetBinContent(i+1)*1.11);
    }

    fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
    covar     = StatUtils::GetInvert(fFullCovar);

  }

  this->SetupDefaultHist();

  this->fScaleFactor = this->fEventHist->Integral("width")*double(1E-38)/double(fNEvents)/TotalIntegratedFlux("width");
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

  fXVar = ppi0;

  return;
};

//******************************************************************** 
bool MINERvA_CC1pi0_XSec_1Dppi0_antinu::isSignal(FitEvent *event) {
//******************************************************************** 
  return SignalDef::isCC1pi0Bar_MINERvA(event, EnuMin, EnuMax);
}
