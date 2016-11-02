#include <iomanip>

#include "T2K_SignalDef.h"

#include "T2K_CC1pip_CH_XSec_1Dthmupi_nu.h"

// The constructor
T2K_CC1pip_CH_XSec_1Dthmupi_nu::T2K_CC1pip_CH_XSec_1Dthmupi_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  fName = "T2K_CC1pip_CH_XSec_1Dthmupi_nu";
  fPlotTitles = "; #theta_{#pi,#mu} (radians); d#sigma/d#theta_{#pi} (cm^{2}/nucleon)";
  EnuMin = 0.;
  EnuMax = 10.;
  fIsDiag = false;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/CH/Thetapimu.root");
  this->SetCovarMatrix(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/CH/Thetapimu.root");

  this->SetupDefaultHist();

  this->fScaleFactor = (this->fEventHist->Integral("width")*1E-38)/double(fNEvents)/TotalIntegratedFlux("width");
};

// Override this for now
// Should really have Measurement1D do this properly though
void T2K_CC1pip_CH_XSec_1Dthmupi_nu::SetDataValues(std::string fileLocation) {
  LOG(DEB) << "Reading: " << this->fName << "\nData: " << fileLocation.c_str() << std::endl;
  TFile *dataFile = new TFile(fileLocation.c_str()); //truly great .root file!

  // Don't want the first and last bin of dataCopy
  TH1D *dataCopy = (TH1D*)(dataFile->Get("hResult_sliced_0_1"))->Clone();

  LOG(DEB) << "dataCopy->GetNbinsX() = " << dataCopy->GetNbinsX() << std::endl;
  double *binEdges = new double[dataCopy->GetNbinsX()];
  for (int i = 0; i < dataCopy->GetNbinsX(); i++) {
    binEdges[i] = dataCopy->GetBinLowEdge(i+1);
  }
  binEdges[dataCopy->GetNbinsX()] = dataCopy->GetBinLowEdge(dataCopy->GetNbinsX()+1);

  for (int i = 0; i < dataCopy->GetNbinsX()+5; i++) {
    LOG(DEB) << "binEdges[" << i << "] = " << binEdges[i] << std::endl;
  }

  fDataHist = new TH1D((fName+"_data").c_str(), (fName+"_data"+fPlotTitles).c_str(), dataCopy->GetNbinsX(), binEdges);

  for (int i = 0; i < fDataHist->GetNbinsX()+1; i++) {
    fDataHist->SetBinContent(i+1, dataCopy->GetBinContent(i+1)*1E-38);
    fDataHist->SetBinError(i+1, dataCopy->GetBinError(i+1)*1E-38);
    LOG(DEB) << fDataHist->GetBinLowEdge(i+1) << " " << fDataHist->GetBinContent(i+1) << " " << fDataHist->GetBinError(i+1) << std::endl;
  }

  fDataHist->SetDirectory(0); //should disassociate fDataHist with dataFile

  dataFile->Close();
};

// Override this for now
// Should really have Measurement1D do this properly though
void T2K_CC1pip_CH_XSec_1Dthmupi_nu::SetCovarMatrix(std::string fileLocation) {
  LOG(DEB) << "Covariance: " << fileLocation.c_str() << std::endl;
  TFile *dataFile = new TFile(fileLocation.c_str()); //truly great .root file!

  TH2D *covarMatrix = (TH2D*)(dataFile->Get("TMatrixDBase;1"))->Clone();

  int nBinsX = covarMatrix->GetXaxis()->GetNbins();
  int nBinsY = covarMatrix->GetYaxis()->GetNbins();

  if ((nBinsX != nBinsY)) ERR(WRN) << "covariance matrix not square!" << std::endl;

  this->covar = new TMatrixDSym(nBinsX-1);
  this->fFullCovar = new TMatrixDSym(nBinsX-1);

  for (int i = 1; i < nBinsX; i++) {
    for (int j = 1; j < nBinsY; j++) {
      (*this->covar)(i-1, j-1) = covarMatrix->GetBinContent(i, j); //adds syst+stat covariances
      (*this->fFullCovar)(i-1, j-1) = covarMatrix->GetBinContent(i, j); //adds syst+stat covariances
      LOG(DEB) << "covar(" << i-1 << ", " << j-1 << ") = " << (*this->covar)(i-1,j-1) << std::endl;
    }
  } //should now have set covariance, I hope

  TDecompChol tempMat = TDecompChol(*this->covar);
  this->covar = new TMatrixDSym(nBinsX, tempMat.Invert().GetMatrixArray(), "");
  *this->covar *= 1E-38*1E-38;

  dataFile->Close();
};


void T2K_CC1pip_CH_XSec_1Dthmupi_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double thmupi = FitUtils::th(Pmu, Ppip);

  fXVar = thmupi;

  return;
};

//********************************************************************
bool T2K_CC1pip_CH_XSec_1Dthmupi_nu::isSignal(FitEvent *event) {
//********************************************************************
// This sample requires directional information on the pion so can't use Michel tag sample
  return SignalDef::isCC1pip_T2K_CH(event, EnuMin, EnuMax, false);
}

