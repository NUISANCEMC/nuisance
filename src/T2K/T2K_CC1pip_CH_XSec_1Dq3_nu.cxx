#include <iomanip>

#include "T2K_SignalDef.h"

#include "T2K_CC1pip_CH_XSec_1Dq3_nu.h"

// The constructor
T2K_CC1pip_CH_XSec_1Dq3_nu::T2K_CC1pip_CH_XSec_1Dq3_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  fName = "T2K_CC1pip_CH_XSec_1Dq3_nu";
  fPlotTitles = "; q_{3} (GeV/c); d#sigma/dq_{3} (cm^{2}/(GeV/c)/nucleon)";
  EnuMin = 0.;
  EnuMax = 100.;
  fIsDiag = false;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/CH/Q3.root");
  this->SetCovarMatrix(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/CH/Q3.root");
  SetShapeCovar();
  this->SetupDefaultHist();

  this->fScaleFactor = (GetEventHistogram()->Integral("width")*1E-38)/double(fNEvents)/TotalIntegratedFlux("width");
};

// Override this for now
// Should really have Measurement1D do this properly though
void T2K_CC1pip_CH_XSec_1Dq3_nu::SetDataValues(std::string fileLocation) {
  LOG(DEB) << "Reading: " << this->fName << "\nData: " << fileLocation.c_str() << std::endl;
  TFile *dataFile = new TFile(fileLocation.c_str()); //truly great .root file!

  // Don't want the last bin of dataCopy
  TH1D *dataCopy = (TH1D*)(dataFile->Get("hResult_sliced_0_1"))->Clone();

  double *binEdges = new double[dataCopy->GetNbinsX()-2];
  LOG(DEB) << dataCopy->GetNbinsX() << std::endl;
  for (int i = 1; i < dataCopy->GetNbinsX()-1; i++) {
    binEdges[i-1] = dataCopy->GetBinLowEdge(i+1);
    LOG(DEB) << i-1 << " " << binEdges[i-1] << " from binLowEdge " << i+1 << std::endl;
  }

  fDataHist = new TH1D((fName+"_data").c_str(), (fName+"_data"+fPlotTitles).c_str(), dataCopy->GetNbinsX()-2, binEdges);

  for (int i = 0; i < fDataHist->GetNbinsX(); i++) {
    fDataHist->SetBinContent(i+1, dataCopy->GetBinContent(i+2)*1E-38);
    fDataHist->SetBinError(i+1, dataCopy->GetBinError(i+2)*1E-38);
    LOG(DEB) << fDataHist->GetBinLowEdge(i+1) << " " << fDataHist->GetBinContent(i+1) << " " << fDataHist->GetBinError(i+1) << std::endl;
  }

  fDataHist->SetDirectory(0); //should disassociate fDataHist with dataFile
  fDataHist->SetNameTitle((fName+"_data").c_str(), (fName+"_MC"+fPlotTitles).c_str());


  dataFile->Close();
};

// Override this for now
// Should really have Measurement1D do this properly though
void T2K_CC1pip_CH_XSec_1Dq3_nu::SetCovarMatrix(std::string fileLocation) {
  LOG(DEB) << "Covariance: " << fileLocation.c_str() << std::endl;
  TFile *dataFile = new TFile(fileLocation.c_str()); //truly great .root file!

  TH2D *covarMatrix = (TH2D*)(dataFile->Get("TMatrixDBase;1"))->Clone();

  int nBinsX = covarMatrix->GetXaxis()->GetNbins();
  int nBinsY = covarMatrix->GetYaxis()->GetNbins();

  if ((nBinsX != nBinsY)) ERR(WRN) << "covariance matrix not square!" << std::endl;

  this->fFullCovar = new TMatrixDSym(nBinsX-3);

  // First two entries are BS
  // Last entry is BS
  for (int i = 2; i < nBinsX-1; i++) {
    for (int j = 2; j < nBinsY-1; j++) {
      (*this->fFullCovar)(i-2, j-2) = covarMatrix->GetBinContent(i+1, j+1); //adds syst+stat covariances
      LOG(DEB) << "fFullCovar(" << i-2 << ", " << j-2 << ") = " << (*this->fFullCovar)(i-2,j-2) << std::endl;
    }
  } //should now have set covariance, I hope

  this->fDecomp = StatUtils::GetDecomp(this->fFullCovar);
  this->covar   = StatUtils::GetInvert(this->fFullCovar);
  return;
};


void T2K_CC1pip_CH_XSec_1Dq3_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(13) == 0 ||
      event->NumFSParticle(211) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double q3 = FitUtils::q3_CC1pip_T2K(Pnu, Pmu, Ppip);

  fXVar = q3;

  return;
};

//********************************************************************
bool T2K_CC1pip_CH_XSec_1Dq3_nu::isSignal(FitEvent *event) {
//********************************************************************
// Has a Michel e sample in so no phase space cut on pion required
  return SignalDef::isCC1pip_T2K_CH(event, EnuMin, EnuMax, true);
}

