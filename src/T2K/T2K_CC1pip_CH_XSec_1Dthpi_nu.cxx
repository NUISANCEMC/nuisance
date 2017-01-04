#include <iomanip>

#include "T2K_SignalDef.h"

#include "T2K_CC1pip_CH_XSec_1Dthpi_nu.h"

// The constructor
T2K_CC1pip_CH_XSec_1Dthpi_nu::T2K_CC1pip_CH_XSec_1Dthpi_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  fName = "T2K_CC1pip_CH_XSec_1Dthpi_nu";
  fPlotTitles = "; #theta_{#pi} (radians); d#sigma/d#theta_{#pi} (cm^{2}/nucleon)";
  EnuMin = 0.;
  EnuMax = 10.;
  fIsDiag = false;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/CH/Thetapi.root");
  this->SetCovarMatrix(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/CH/Thetapi.root");

  this->SetupDefaultHist();

  this->fScaleFactor = (this->fEventHist->Integral("width")*1E-38)/double(fNEvents)/TotalIntegratedFlux("width");
};

// Override this for now
// Should really have Measurement1D do this properly though
void T2K_CC1pip_CH_XSec_1Dthpi_nu::SetDataValues(std::string fileLocation) {
  LOG(DEB) << "Reading: " << this->fName << "\nData: " << fileLocation.c_str() << std::endl;
  TFile *dataFile = new TFile(fileLocation.c_str()); //truly great .root file!

  // Don't want the first and last bin of dataCopy
  TH1D *dataCopy = (TH1D*)(dataFile->Get("hResult_sliced_0_1"))->Clone();

  LOG(DEB) << "dataCopy->GetNbinsX() = " << dataCopy->GetNbinsX() << std::endl;
  double *binEdges = new double[dataCopy->GetNbinsX()-4];
  for (int i = 0; i < dataCopy->GetNbinsX()-4; i++) {
    binEdges[i] = dataCopy->GetBinLowEdge(i+1);
  }
  binEdges[dataCopy->GetNbinsX()-4] = dataCopy->GetBinLowEdge(dataCopy->GetNbinsX()-3);

  for (int i = 0; i < dataCopy->GetNbinsX(); i++) {
    LOG(DEB) << "binEdges[" << i << "] = " << binEdges[i] << std::endl;
  }

  fDataHist = new TH1D((fName+"_data").c_str(), (fName+"_data"+fPlotTitles).c_str(), dataCopy->GetNbinsX()-4, binEdges);

  for (int i = 0; i < fDataHist->GetNbinsX(); i++) {
    fDataHist->SetBinContent(i+1, dataCopy->GetBinContent(i+1)*1E-38);
    fDataHist->SetBinError(i+1, dataCopy->GetBinError(i+1)*1E-38);
    LOG(DEB) << fDataHist->GetBinLowEdge(i+1) << " " << fDataHist->GetBinContent(i+1) << " " << fDataHist->GetBinError(i+1) << std::endl;
  }

  fDataHist->SetDirectory(0); //should disassociate fDataHist with dataFile

  dataFile->Close();
};

// Override this for now
// Should really have Measurement1D do this properly though
void T2K_CC1pip_CH_XSec_1Dthpi_nu::SetCovarMatrix(std::string fileLocation) {
  LOG(DEB) << "Covariance: " << fileLocation.c_str() << std::endl;
  TFile *dataFile = new TFile(fileLocation.c_str()); //truly great .root file!

  TH2D *covarMatrix = (TH2D*)(dataFile->Get("TMatrixDBase;1"))->Clone();

  int nBinsX = covarMatrix->GetXaxis()->GetNbins();
  int nBinsY = covarMatrix->GetYaxis()->GetNbins();

  if ((nBinsX != nBinsY)) ERR(WRN) << "covariance matrix not square!" << std::endl;

  this->covar = new TMatrixDSym(nBinsX-5);
  this->fFullCovar = new TMatrixDSym(nBinsX-5);

  for (int i = 2; i < nBinsX-3; i++) {
    for (int j = 2; j < nBinsY-3; j++) {
      (*this->covar)(i-2, j-2) = covarMatrix->GetBinContent(i, j); //adds syst+stat covariances
      (*this->fFullCovar)(i-2, j-2) = covarMatrix->GetBinContent(i, j); //adds syst+stat covariances
      LOG(DEB) << "covar(" << i-2 << ", " << j-2 << ") = " << (*this->covar)(i-2,j-2) << std::endl;
    }
  } //should now have set covariance, I hope

  TDecompChol tempMat = TDecompChol(*this->covar);
  this->covar = new TMatrixDSym(nBinsX, tempMat.Invert().GetMatrixArray(), "");
  //  *this->covar *= 1E-38*1E-38;

  dataFile->Close();
};


void T2K_CC1pip_CH_XSec_1Dthpi_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(13) == 0 ||
      event->NumFSParticle(211) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double thpi = FitUtils::th(Pnu, Ppip);

  fXVar = thpi;

  return;
};

//********************************************************************
bool T2K_CC1pip_CH_XSec_1Dthpi_nu::isSignal(FitEvent *event) {
//********************************************************************
// This sample uses directional info on the pion so Michel e tag sample can not be included
// i.e. we need reduce the pion variable phase space
  return SignalDef::isCC1pip_T2K_CH(event, EnuMin, EnuMax, false);
}

