#include "T2K_CC1pip_CH_XSec_1Dpmu_nu.h"
#include <iomanip>

// The constructor
T2K_CC1pip_CH_XSec_1Dpmu_nu::T2K_CC1pip_CH_XSec_1Dpmu_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  fName = "T2K_CC1pip_CH_XSec_1Dpmu_nu";
  fPlotTitles = "; p_{#mu} (GeV/c); d#sigma/dp_{#mu} (cm^{2}/(GeV/c)/nucleon)";
  EnuMin = 0.;
  EnuMax = 100.;
  fIsDiag = false;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Currently the data is given in individual root files
  // This will likely change when results become official
  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/CH/Pmu.root");
  this->SetCovarMatrix(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/CH/Pmu.root");

  this->SetupDefaultHist();

  this->fScaleFactor = (GetEventHistogram()->Integral("width")*1E-38)/double(fNEvents)/TotalIntegratedFlux("width");
};

// Override this for now
// Should really have Measurement1D do this properly though
void T2K_CC1pip_CH_XSec_1Dpmu_nu::SetDataValues(std::string fileLocation) {
  LOG(DEB) << "Reading: " << this->fName << "\nData: " << fileLocation.c_str() << std::endl;
  TFile *dataFile = new TFile(fileLocation.c_str()); //truly great .root file!

  // Don't want the last bin of dataCopy
  TH1D *dataCopy = (TH1D*)(dataFile->Get("hResult_sliced_0_1"))->Clone();
  LOG(DEB) << "nbins = " << dataCopy->GetNbinsX() << std::endl;

  double *binEdges = new double[dataCopy->GetNbinsX()-1];
  for (int i = 0; i < dataCopy->GetNbinsX()-1; i++) {
    binEdges[i] = dataCopy->GetBinLowEdge(i+1);
  }
  binEdges[dataCopy->GetNbinsX()-1] = dataCopy->GetBinLowEdge(dataCopy->GetNbinsX());

  for (int i = 0; i < dataCopy->GetNbinsX()+5; i++) {
    LOG(DEB) << "binEdges[" << i << "] = " << binEdges[i] << std::endl;
  }

  fDataHist = new TH1D((fName+"_data").c_str(), (fName+"_data"+fPlotTitles).c_str(), dataCopy->GetNbinsX()-1, binEdges);

  for (int i = 0; i < fDataHist->GetNbinsX(); i++) {
    fDataHist->SetBinContent(i+1, dataCopy->GetBinContent(i+1)*1E-38);
    fDataHist->SetBinError(i+1, dataCopy->GetBinError(i+1)*1E-38);
    LOG(DEB) << fDataHist->GetBinLowEdge(i+1) << " " << fDataHist->GetBinContent(i+1) << std::endl;
  }

  fDataHist->SetDirectory(0); //should disassociate fDataHist with dataFile
  fDataHist->SetNameTitle((fName+"_data").c_str(), (fName+"_MC"+fPlotTitles).c_str());


  dataFile->Close();
};

// Override this for now
// Should really have Measurement1D do this properly though
void T2K_CC1pip_CH_XSec_1Dpmu_nu::SetCovarMatrix(std::string fileLocation) {
  LOG(DEB) << "Covariance: " << fileLocation.c_str() << std::endl;
  TFile *dataFile = new TFile(fileLocation.c_str()); //truly great .root file!

  TH2D *covarMatrix = (TH2D*)(dataFile->Get("TMatrixDBase;1"))->Clone();

  int nBinsX = covarMatrix->GetXaxis()->GetNbins();
  int nBinsY = covarMatrix->GetYaxis()->GetNbins();

  if ((nBinsX != nBinsY)) ERR(WRN) << "covariance matrix not square!" << std::endl;

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
  //  *this->covar *= 1E-38*1E-38;

  return;
};


void T2K_CC1pip_CH_XSec_1Dpmu_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(13) == 0 ||
      event->NumFSParticle(211) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double pmu = FitUtils::p(Pmu);

  fXVar = pmu;

  return;
};

//********************************************************************
bool T2K_CC1pip_CH_XSec_1Dpmu_nu::isSignal(FitEvent *event) {
//********************************************************************
// Warning: The CH analysis has different signal definition to the H2O analysis!
//
// If Michel e- is used for pion PID we don't have directional info on pion; set the bool to true
// The bool is if we use Michel e- or not
// Also, for events binned in muon momentum/angle there's no cut on the pion kinematics
//
// Additionally, the 2D distribution uses 0.8 > cos th mu > 0 and no pion phase space reduction applied
// Also no muon momentum reduction applied
//
// This uses a slightly custom signal definition where a cut is only placed on the muon angle, not the momentum

  if (!SignalDef::isCC1pi(event, 14, 211, EnuMin, EnuMax)) return false;

  TLorentzVector Pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;

  double cos_th_mu = cos(FitUtils::th(Pnu,Pmu));

  if (cos_th_mu >= 0.2) return true;
  return false;
};

