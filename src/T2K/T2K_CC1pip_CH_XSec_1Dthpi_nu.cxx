#include "T2K_CC1pip_CH_XSec_1Dthpi_nu.h"
#include <iomanip>

// The constructor
T2K_CC1pip_CH_XSec_1Dthpi_nu::T2K_CC1pip_CH_XSec_1Dthpi_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  measurementName = "T2K_CC1pip_CH_XSec_1Dthpi_nu";
  plotTitles = "; #theta_{#pi} (radians); d#sigma/d#theta_{#pi} (cm^{2}/nucleon)";
  EnuMin = 0.;
  EnuMax = 10.;
  isDiag = false;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/T2K/CC1pip/CH/Thetapi.root");
  this->SetCovarMatrix(std::string(std::getenv("EXT_FIT"))+"/data/T2K/CC1pip/CH/Thetapi.root");

  this->SetupDefaultHist();

  this->scaleFactor = (this->eventHist->Integral("width")*1E-38)/double(nevents)/TotalIntegratedFlux("width");
};

// Override this for now
// Should really have Measurement1D do this properly though
void T2K_CC1pip_CH_XSec_1Dthpi_nu::SetDataValues(std::string fileLocation) {
  std::cout << "Reading: " << this->measurementName << "\nData: " << fileLocation.c_str() << std::endl;
  TFile *dataFile = new TFile(fileLocation.c_str()); //truly great .root file!

  // Don't want the first and last bin of dataCopy
  TH1D *dataCopy = (TH1D*)(dataFile->Get("hResult_sliced_0_1"))->Clone();

  //std::cout << "dataCopy->GetNbinsX() = " << dataCopy->GetNbinsX() << std::endl;
  double *binEdges = new double[dataCopy->GetNbinsX()-4];
  for (int i = 0; i < dataCopy->GetNbinsX()-4; i++) {
    binEdges[i] = dataCopy->GetBinLowEdge(i+1);
  }
  binEdges[dataCopy->GetNbinsX()-4] = dataCopy->GetBinLowEdge(dataCopy->GetNbinsX()-3);

  for (int i = 0; i < dataCopy->GetNbinsX(); i++) {
    //std::cout << "binEdges[" << i << "] = " << binEdges[i] << std::endl;
  }

  dataHist = new TH1D((measurementName+"_data").c_str(), (measurementName+"_data"+plotTitles).c_str(), dataCopy->GetNbinsX()-4, binEdges);

  for (int i = 0; i < dataHist->GetNbinsX(); i++) {
    dataHist->SetBinContent(i+1, dataCopy->GetBinContent(i+1)*1E-38);
    dataHist->SetBinError(i+1, dataCopy->GetBinError(i+1)*1E-38);
    //std::cout << dataHist->GetBinLowEdge(i+1) << " " << dataHist->GetBinContent(i+1) << " " << dataHist->GetBinError(i+1) << std::endl;
  }

  dataHist->SetDirectory(0); //should disassociate dataHist with dataFile

  dataFile->Close();
};

// Override this for now
// Should really have Measurement1D do this properly though
void T2K_CC1pip_CH_XSec_1Dthpi_nu::SetCovarMatrix(std::string fileLocation) {
  std::cout << "Covariance: " << fileLocation.c_str() << std::endl;
  TFile *dataFile = new TFile(fileLocation.c_str()); //truly great .root file!

  TH2D *covarMatrix = (TH2D*)(dataFile->Get("TMatrixDBase;1"))->Clone();

  int nBinsX = covarMatrix->GetXaxis()->GetNbins();
  int nBinsY = covarMatrix->GetYaxis()->GetNbins();

  if ((nBinsX != nBinsY)) std::cerr << "covariance matrix not square!" << std::endl;

  this->covar = new TMatrixDSym(nBinsX-5);
  this->fullcovar = new TMatrixDSym(nBinsX-5);

  for (int i = 2; i < nBinsX-3; i++) {
    for (int j = 2; j < nBinsY-3; j++) {
      //std::cout << "(" << i << ", " << j << ") = " << covarMatrix->GetBinContent(i+1,j+1) << std::endl;
      (*this->covar)(i-2, j-2) = covarMatrix->GetBinContent(i, j); //adds syst+stat covariances
      (*this->fullcovar)(i-2, j-2) = covarMatrix->GetBinContent(i, j); //adds syst+stat covariances
      //std::cout << "covar(" << i-2 << ", " << j-2 << ") = " << (*this->covar)(i-2,j-2) << std::endl;
    }
  } //should now have set covariance, I hope

  TDecompChol tempMat = TDecompChol(*this->covar);
  this->covar = new TMatrixDSym(nBinsX, tempMat.Invert().GetMatrixArray(), "");
  *this->covar *= 1E-38*1E-38;

  dataFile->Close();
};


void T2K_CC1pip_CH_XSec_1Dthpi_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu = (event->PartInfo(0))->fP;
  TLorentzVector Ppip;
  TLorentzVector Pmu;

  // Loop over the particle stack
  for (unsigned int j = 2; j < event->Npart(); ++j) {
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0) continue;
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 211) {
      Ppip = event->PartInfo(j)->fP;
    } else if (PID == 13) {
      Pmu = (event->PartInfo(j))->fP;  
    }
  }

  double thpi = FitUtils::th(Pnu, Ppip);

  this->X_VAR = thpi;

  return;
};

//******************************************************************** 
bool T2K_CC1pip_CH_XSec_1Dthpi_nu::isSignal(FitEvent *event) {
//******************************************************************** 
// This sample uses directional info on the pion so Michel e tag sample can not be included
// i.e. we need reduce the pion variable phase space
  return SignalDef::isCC1pip_T2K_CH(event, EnuMin, EnuMax, false);
}

