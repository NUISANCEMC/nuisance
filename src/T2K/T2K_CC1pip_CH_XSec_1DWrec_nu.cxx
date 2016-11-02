#include <iomanip>

#include "T2K_SignalDef.h"

#include "T2K_CC1pip_CH_XSec_1DWrec_nu.h"

// The constructor
T2K_CC1pip_CH_XSec_1DWrec_nu::T2K_CC1pip_CH_XSec_1DWrec_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  fName = "T2K_CC1pip_CH_XSec_1DWrec_nu";
  fPlotTitles = "; W_{rec} (GeV/c); d#sigma/dW_{rec} (cm^{2}/(GeV/c^{2})/nucleon)";
  EnuMin = 0.;
  EnuMax = 10.;
  fIsDiag = false;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/CH/W.root");
  this->SetCovarMatrix(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/CH/W.root");

  this->SetupDefaultHist();

  this->fScaleFactor = (this->fEventHist->Integral("width")*1E-38)/double(fNEvents)/TotalIntegratedFlux("width");
};

// Override this for now
// Should really have Measurement1D do this properly though
void T2K_CC1pip_CH_XSec_1DWrec_nu::SetDataValues(std::string fileLocation) {
  LOG(DEB) << "Reading: " << this->fName << "\nData: " << fileLocation.c_str() << std::endl;
  TFile *dataFile = new TFile(fileLocation.c_str()); //truly great .root file!

  // Don't want the first and last bin of dataCopy
  TH1D *dataCopy = (TH1D*)(dataFile->Get("hResult_sliced_0_1"))->Clone();

  LOG(DEB) << dataCopy->GetNbinsX() << std::endl;
  const int dataPoints = dataCopy->GetNbinsX()-2;
  double *binEdges = new double[dataPoints+1];
  // Want to skip the first bin here
  for (int i = 0; i < dataPoints+1; i++) {
    binEdges[i] = dataCopy->GetBinLowEdge(i+2);
  }

  for (int i = 0; i < dataPoints+1; i++) {
    LOG(DEB) << "binEdges[" << i << "] = " << binEdges[i] << std::endl;
  }

  fDataHist = new TH1D((fName+"_data").c_str(), (fName+"_data"+fPlotTitles).c_str(), dataPoints, binEdges);

  for (int i = 0; i < fDataHist->GetNbinsX(); i++) {
    fDataHist->SetBinContent(i+1, dataCopy->GetBinContent(i+2)*1E-38);
    fDataHist->SetBinError(i+1, dataCopy->GetBinError(i+2)*1E-38);
    LOG(DEB) << fDataHist->GetBinLowEdge(i+1) << " " << fDataHist->GetBinContent(i+1) << " " << fDataHist->GetBinError(i+1) << std::endl;
  }


  fDataHist->SetDirectory(0); //should disassociate fDataHist with dataFile
  //fDataHist->SetNameTitle((fName+"_data").c_str(), (fName+"_MC"+fPlotTitles).c_str());


  dataFile->Close();
};

// Override this for now
// Should really have Measurement1D do this properly though
void T2K_CC1pip_CH_XSec_1DWrec_nu::SetCovarMatrix(std::string fileLocation) {
  LOG(DEB) << "Covariance: " << fileLocation.c_str() << std::endl;
  TFile *dataFile = new TFile(fileLocation.c_str()); //truly great .root file!

  TH2D *covarMatrix = (TH2D*)(dataFile->Get("TMatrixDBase;1"))->Clone();

  const int nBinsX = covarMatrix->GetXaxis()->GetNbins();
  const int nBinsY = covarMatrix->GetYaxis()->GetNbins();

  if ((nBinsX != nBinsY)) ERR(WRN) << "covariance matrix not square!" << std::endl;

  LOG(DEB) << nBinsX << std::endl;
  LOG(DEB) << fDataHist->GetNbinsX() << std::endl;
  this->covar = new TMatrixDSym(nBinsX-3);
  this->fFullCovar = new TMatrixDSym(nBinsX-3);

  for (int i = 0; i < nBinsX-3; i++) {
    for (int j = 0; j < nBinsY-3; j++) {
      (*this->covar)(i, j) = covarMatrix->GetBinContent(i+4, j+4); //adds syst+stat covariances
      LOG(DEB) << "covar(" << i << ", " << j << ") = " << (*this->covar)(i,j) << std::endl;
    }
  } //should now have set covariance, I hope

  TDecompChol tempMat = TDecompChol(*this->covar);
  this->covar = new TMatrixDSym(nBinsX, tempMat.Invert().GetMatrixArray(), "");
  *this->covar *= 1E-38*1E-38;

  return;
};


void T2K_CC1pip_CH_XSec_1DWrec_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double Wrec = FitUtils::WrecCC1pip_T2K_MB(Pnu, Pmu, Ppip);

  fXVar = Wrec;

  return;
};

//********************************************************************
bool T2K_CC1pip_CH_XSec_1DWrec_nu::isSignal(FitEvent *event) {
//********************************************************************
// This sample includes the Michel e tag so do not have to cut into the pion phase space
  return SignalDef::isCC1pip_T2K_CH(event, EnuMin, EnuMax, true);
}

