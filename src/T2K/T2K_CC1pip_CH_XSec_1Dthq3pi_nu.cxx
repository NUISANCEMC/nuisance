#include <iomanip>

#include "T2K_SignalDef.h"

#include "T2K_CC1pip_CH_XSec_1Dthq3pi_nu.h"

// The constructor
T2K_CC1pip_CH_XSec_1Dthq3pi_nu::T2K_CC1pip_CH_XSec_1Dthq3pi_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  fName = "T2K_CC1pip_CH_XSec_1Dthq3pi_nu";
  fPlotTitles = "; #theta_{q_{3},#pi} (radians); d#sigma/d#theta_{q_{3},#pi} (cm^{2}/(radian)/nucleon)";
  EnuMin = 0.;
  EnuMax = 10.;
  fIsDiag = false;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/CH/ThetaQ3Pi.root");
  this->SetCovarMatrix(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/CH/ThetaQ3Pi.root");

  this->SetupDefaultHist();

  this->fScaleFactor = (this->fEventHist->Integral("width")*1E-38)/double(fNEvents)/TotalIntegratedFlux("width");
};

// Override this for now
// Should really have Measurement1D do this properly though
void T2K_CC1pip_CH_XSec_1Dthq3pi_nu::SetDataValues(std::string fileLocation) {
  LOG(DEB) << "Reading: " << this->fName << "\nData: " << fileLocation.c_str() << std::endl;
  TFile *dataFile = new TFile(fileLocation.c_str()); //truly great .root file!

  // Don't want the last bin of dataCopy
  TH1D *dataCopy = (TH1D*)(dataFile->Get("hResult_sliced_0_1"))->Clone();

  double *binEdges = new double[dataCopy->GetNbinsX()-1];
  LOG(DEB) << dataCopy->GetNbinsX() << std::endl;
  for (int i = 1; i < dataCopy->GetNbinsX()+1; i++) {
    binEdges[i-1] = dataCopy->GetBinLowEdge(i);
    LOG(DEB) << i-1 << " " << binEdges[i-1] << std::endl;
  }

  for (int i = 0; i < dataCopy->GetNbinsX()+5; i++) {
    LOG(DEB) << "binEdges[" << i << "] = " << binEdges[i] << std::endl;
  }

  fDataHist = new TH1D((fName+"_data").c_str(), (fName+"_data"+fPlotTitles).c_str(), dataCopy->GetNbinsX()-1, binEdges);

  for (int i = 0; i < fDataHist->GetNbinsX(); i++) {
    fDataHist->SetBinContent(i+1, dataCopy->GetBinContent(i+1)*1E-38);
    fDataHist->SetBinError(i+1, dataCopy->GetBinError(i+1)*1E-38);
    LOG(DEB) << fDataHist->GetBinLowEdge(i+1) << " " << fDataHist->GetBinContent(i+1) << " " << fDataHist->GetBinError(i+1) << std::endl;
  }

  fDataHist->SetDirectory(0); //should disassociate fDataHist with dataFile
  fDataHist->SetNameTitle((fName+"_data").c_str(), (fName+"_MC"+fPlotTitles).c_str());


  dataFile->Close();
};

// Override this for now
// Should really have Measurement1D do this properly though
void T2K_CC1pip_CH_XSec_1Dthq3pi_nu::SetCovarMatrix(std::string fileLocation) {
  LOG(DEB) << "Covariance: " << fileLocation.c_str() << std::endl;
  TFile *dataFile = new TFile(fileLocation.c_str()); //truly great .root file!

  TH2D *covarMatrix = (TH2D*)(dataFile->Get("TMatrixDBase;1"))->Clone();

  int nBinsX = covarMatrix->GetXaxis()->GetNbins();
  int nBinsY = covarMatrix->GetYaxis()->GetNbins();

  if ((nBinsX != nBinsY)) ERR(WRN) << "covariance matrix not square!" << std::endl;

  // First bin is underflow, last bin is overflow
  this->covar = new TMatrixDSym(nBinsX-2);
  this->fFullCovar = new TMatrixDSym(nBinsX-2);

  // First two entries are BS
  // Last entry is BS
  for (int i = 1; i < nBinsX-1; i++) {
    for (int j = 1; j < nBinsY-1; j++) {
      (*this->covar)(i-1, j-1) = covarMatrix->GetBinContent(i+1, j+1); //adds syst+stat covariances
      (*this->fFullCovar)(i-1, j-1) = covarMatrix->GetBinContent(i+1, j+1); //adds syst+stat covariances
      LOG(DEB) << "covar(" << i-1 << ", " << j-1 << ") = " << (*this->covar)(i-1,j-1) << std::endl;
    }
  } //should now have set covariance, I hope

  TDecompChol tempMat = TDecompChol(*this->covar);
  this->covar = new TMatrixDSym(nBinsX, tempMat.Invert().GetMatrixArray(), "");
  *this->covar *= 1E-38*1E-38;

  return;
};


void T2K_CC1pip_CH_XSec_1Dthq3pi_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double th_q3_pi = FitUtils::thq3pi_CC1pip_T2K(Pnu, Pmu, Ppip);

  fXVar = th_q3_pi;

  return;
};

//********************************************************************
bool T2K_CC1pip_CH_XSec_1Dthq3pi_nu::isSignal(FitEvent *event) {
//********************************************************************
// This sample requires pion directional information so can not include Michel tag sample
// i.e. will need to cut the pion phase space
  return SignalDef::isCC1pip_T2K_CH(event, EnuMin, EnuMax, false);
}

