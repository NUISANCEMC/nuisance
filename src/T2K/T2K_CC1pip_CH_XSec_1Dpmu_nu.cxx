#include "T2K_CC1pip_CH_XSec_1Dpmu_nu.h"
#include <iomanip>

// The constructor
T2K_CC1pip_CH_XSec_1Dpmu_nu::T2K_CC1pip_CH_XSec_1Dpmu_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  fName = "T2K_CC1pip_CH_XSec_1Dpmu_nu";
  fPlotTitles = "; p_{#mu} (GeV/c); d#sigma/dp_{#mu} (cm^{2}/(GeV/c)/nucleon)";
  EnuMin = 0.;
  EnuMax = 10.;
  fIsDiag = false;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/T2K/CC1pip/CH/Pmu.root");
  this->SetCovarMatrix(std::string(std::getenv("EXT_FIT"))+"/data/T2K/CC1pip/CH/Pmu.root");

  this->SetupDefaultHist();

  this->fScaleFactor = (this->fEventHist->Integral("width")*1E-38)/double(fNEvents)/TotalIntegratedFlux("width");
};

// Override this for now
// Should really have Measurement1D do this properly though
void T2K_CC1pip_CH_XSec_1Dpmu_nu::SetDataValues(std::string fileLocation) {
  std::cout << "Reading: " << this->fName << "\nData: " << fileLocation.c_str() << std::endl;
  TFile *dataFile = new TFile(fileLocation.c_str()); //truly great .root file!

  // Don't want the last bin of dataCopy
  TH1D *dataCopy = (TH1D*)(dataFile->Get("hResult_sliced_0_1"))->Clone();
  std::cout << dataCopy->GetNbinsX() << std::endl;

  double *binEdges = new double[dataCopy->GetNbinsX()-1];
  for (int i = 0; i < dataCopy->GetNbinsX()-1; i++) {
    binEdges[i] = dataCopy->GetBinLowEdge(i+1);
  }
  binEdges[dataCopy->GetNbinsX()-1] = dataCopy->GetBinLowEdge(dataCopy->GetNbinsX());

  for (int i = 0; i < dataCopy->GetNbinsX()+5; i++) {
    std::cout << "binEdges[" << i << "] = " << binEdges[i] << std::endl;
  }

  fDataHist = new TH1D((fName+"_data").c_str(), (fName+"_data"+fPlotTitles).c_str(), dataCopy->GetNbinsX()-1, binEdges);

  for (int i = 0; i < fDataHist->GetNbinsX(); i++) {
    fDataHist->SetBinContent(i+1, dataCopy->GetBinContent(i+1)*1E-38);
    fDataHist->SetBinError(i+1, dataCopy->GetBinError(i+1)*1E-38);
    std::cout << fDataHist->GetBinLowEdge(i+1) << " " << fDataHist->GetBinContent(i+1) << std::endl;
  }

  fDataHist->SetDirectory(0); //should disassociate fDataHist with dataFile
  fDataHist->SetNameTitle((fName+"_data").c_str(), (fName+"_MC"+fPlotTitles).c_str());


  dataFile->Close();
};

// Override this for now
// Should really have Measurement1D do this properly though
void T2K_CC1pip_CH_XSec_1Dpmu_nu::SetCovarMatrix(std::string fileLocation) {
  std::cout << "Covariance: " << fileLocation.c_str() << std::endl;
  TFile *dataFile = new TFile(fileLocation.c_str()); //truly great .root file!

  TH2D *covarMatrix = (TH2D*)(dataFile->Get("TMatrixDBase;1"))->Clone();

  int nBinsX = covarMatrix->GetXaxis()->GetNbins();
  int nBinsY = covarMatrix->GetYaxis()->GetNbins();

  if ((nBinsX != nBinsY)) std::cerr << "covariance matrix not square!" << std::endl;

  this->covar = new TMatrixDSym(nBinsX-2);
  this->fullcovar = new TMatrixDSym(nBinsX-2);

  // First two entries are BS
  // Last entry is BS
  for (int i = 1; i < nBinsX-1; i++) {
    for (int j = 1; j < nBinsY-1; j++) {
      (*this->covar)(i-1, j-1) = covarMatrix->GetBinContent(i+1, j+1); //adds syst+stat covariances
      (*this->fullcovar)(i-1, j-1) = covarMatrix->GetBinContent(i+1, j+1); //adds syst+stat covariances
      std::cout << "covar(" << i-1 << ", " << j-1 << ") = " << (*this->covar)(i-1,j-1) << std::endl;
    }
  } //should now have set covariance, I hope

  TDecompChol tempMat = TDecompChol(*this->covar);
  this->covar = new TMatrixDSym(nBinsX, tempMat.Invert().GetMatrixArray(), "");
  *this->covar *= 1E-38*1E-38;

  return;
};


void T2K_CC1pip_CH_XSec_1Dpmu_nu::FillEventVariables(FitEvent *event) {

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
//
// Pretty much a copy/paste of SignalDef::isCC1pip_T2K_CH(event, EnuMin, EnuMax, true), excluding the muon momentum requirement

  if ((event->PartInfo(0))->fPID != 14) return false; 

  if (((event->PartInfo(0))->fP.E() < EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > EnuMax*1000.)) return false; 

  if (((event->PartInfo(2))->fPID != 13) && ((event->PartInfo(3))->fPID != 13)) return false;

  int pipCnt = 0; // counts number of pions
  int lepCnt = 0;

  TLorentzVector Pnu = (event->PartInfo(0))->fP;
  TLorentzVector Ppip;
  TLorentzVector Pmu;

  for (unsigned int j = 2; j < event->Npart(); j++) {

    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; //move on if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;

    // Ignore any non pi+ mesons
    if ((abs(PID) >= 111 && abs(PID) <= 210) || (abs(PID) >= 212 && abs(PID) <= 557) || PID == -211) return false; 
    // Count leptons
    else if (abs(PID) == 13) {
      lepCnt++;
      Pmu = (event->PartInfo(j))->fP;
    }
    else if (PID == 211) {
      pipCnt++;
      Ppip = (event->PartInfo(j))->fP;
    }
  }

  if (pipCnt != 1) return false; 
  if (lepCnt != 1) return false;

  // relatively generic CC1pi+ definition done
  //
  // now measurement specific depending on if we have Michel e- tag or not in measurement

  double cos_th_mu = cos(FitUtils::th(Pnu,Pmu));

  if (cos_th_mu <= 0.2) {
    return false;
  } else {
    return true;
  }

  // Unnecssary default to false
  return false;

}

