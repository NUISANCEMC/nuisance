#include "MINERvA_CC1pi0_XSec_1DQ2_antinu.h"

// The constructor
MINERvA_CC1pi0_XSec_1DQ2_antinu::MINERvA_CC1pi0_XSec_1DQ2_antinu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  fName = "MINERvA_CC1pi0_XSec_1DQ2_nubar_2016";
  fPlotTitles = "; Q^{2} (GeV^{2}); d#sigma/dQ^{2} (cm^{2}/(GeV^{2})/nucleon)";
  EnuMin = 1.5;
  EnuMax = 10;
  fIsDiag = false;
  fAllowedTypes += "NEW";
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  //this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pi0/2016/cc1pi0_q2.txt");
  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pi0/2016/anu-cc1pi0-xsec-q2.csv");

  // Error is given as percentage of cross-section
  // Need to scale the bin error properly before we do correlation -> covariance conversion
  for (int i = 0; i < fDataHist->GetNbinsX()+1; i++) {
    fDataHist->SetBinError(i+1, fDataHist->GetBinContent(i+1)*fDataHist->GetBinError(i+1)/100.);
  }

  //this->SetCovarMatrixFromText(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pi0/2016/cc1pi0_q2_corr.txt", fDataHist->GetNbinsX());
  this->SetCovarMatrixFromText(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pi0/2016/anu-cc1pi0-correlation-q2.csv", fDataHist->GetNbinsX());


  this->SetupDefaultHist();

  this->fScaleFactor = this->fEventHist->Integral("width")*double(1E-38)/double(fNEvents)/TotalIntegratedFlux("width");
};

void MINERvA_CC1pi0_XSec_1DQ2_antinu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu = (event->PartInfo(0))->fP;
  TLorentzVector Pmu;
  TLorentzVector Ppi0;

  // Loop over the particle stack
  for (unsigned int j = 2; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fNEUTStatusCode != 0) continue;
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 111) {
      Ppi0 = event->PartInfo(j)->fP;
    } else if (PID == -13) {
      Pmu = event->PartInfo(j)->fP;
    }
  }

  double hadMass = FitUtils::Wrec(Pnu, Pmu);
  double Q2;

  if (hadMass > 100 && hadMass < 1800) {
    Q2 = FitUtils::Q2CC1pi0rec(Pnu, Pmu, Ppi0);
  } else {
    Q2 = -999;
  }

  fXVar = Q2;

  return;
};

//******************************************************************** 
bool MINERvA_CC1pi0_XSec_1DQ2_antinu::isSignal(FitEvent *event) {
//******************************************************************** 
  return SignalDef::isCC1pi(event, -14, 111, EnuMin, EnuMax);
}
