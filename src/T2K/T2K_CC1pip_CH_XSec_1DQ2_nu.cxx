#include <iomanip>

#include "T2K_SignalDef.h"

#include "T2K_CC1pip_CH_XSec_1DQ2_nu.h"

// The constructor
T2K_CC1pip_CH_XSec_1DQ2_nu::T2K_CC1pip_CH_XSec_1DQ2_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  EnuMin = 0.;
  EnuMax = 10.;
  fIsDiag = false;

  // Here we can give either MB (kMB), extended MB (keMB) or Delta (kDelta)
  if (type.find("eMB") != std::string::npos) {
    fT2KSampleType = keMB;
    fName = "T2K_CC1pip_CH_XSec_1DQ2eMB_nu";
    fPlotTitles = "; Q^{2}_{eMB} (GeV^{2}); d#sigma/dQ^{2}_{eMB} (cm^{2}/GeV^{2}/nucleon)";
  } else if (type.find("MB") != std::string::npos) {
    fT2KSampleType = kMB;
    fName = "T2K_CC1pip_CH_XSec_1DQ2MB_nu";
    fPlotTitles = "; Q^{2}_{MB} (GeV^{2}); d#sigma/dQ^{2}_{MB} (cm^{2}/GeV^{2}/nucleon)";
  } else if (type.find("Delta") != std::string::npos) {
    fT2KSampleType = kDelta;
    fName = "T2K_CC1pip_CH_XSec_1DQ2delta_nu";
    fPlotTitles = "; Q^{2}_{#Delta} (GeV^{2}); d#sigma/dQ^{2}_{#Delta} (cm^{2}/GeV^{2}/nucleon)";
  } else {
    LOG(SAM) << "Found no specified type, using MiniBooNE E_nu/Q2 definition" << std::endl;
    fT2KSampleType = kMB;
    fName = "T2K_CC1pip_CH_XSec_1DQ2MB_nu";
    fPlotTitles = "; Q^{2}_{MB} (GeV^{2}); d#sigma/dQ^{2}_{MB} (cm^{2}/GeV^{2}/nucleon)";
  }

  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  //type = keMB;
  //type = kDelta;

  if (fT2KSampleType == kMB) {
    this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/CH/Q2_MB.root");
    this->SetCovarMatrix(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/CH/Q2_MB.root");
  } else if (fT2KSampleType == keMB) {
    this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/CH/Q2_extendedMB.root");
    this->SetCovarMatrix(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/CH/Q2_extendedMB.root");
  } else if (fT2KSampleType == kDelta) {
    this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/CH/Q2_Delta.root");
    this->SetCovarMatrix(GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pip/CH/Q2_Delta.root");
  } else {
    ERR(FTL) << "No data type set for " << fName << std::endl;
    ERR(FTL) << __FILE__ << ":" << __LINE__ << std::endl;
    exit(-1);
  }

  this->SetupDefaultHist();

  this->fScaleFactor = (this->fEventHist->Integral("width")*1E-38)/double(fNEvents)/TotalIntegratedFlux("width");
};

// Override this for now
// Should really have Measurement1D do this properly though
void T2K_CC1pip_CH_XSec_1DQ2_nu::SetDataValues(std::string fileLocation) {
  LOG(DEB) << "Reading: " << this->fName << "\nData: " << fileLocation.c_str() << std::endl;
  TFile *dataFile = new TFile(fileLocation.c_str()); //truly great .root file!

  // Don't want the last bin of dataCopy
  TH1D *dataCopy = (TH1D*)(dataFile->Get("hResult_sliced_0_1"))->Clone();

  const int nPoints = dataCopy->GetNbinsX()-6;
  LOG(DEB) << nPoints << std::endl;
  double *binEdges = new double[nPoints+1];
  for (int i = 0; i < nPoints+1; i++) {
    binEdges[i] = dataCopy->GetBinLowEdge(i+1);
  }

  for (int i = 0; i < nPoints+1; i++) {
    LOG(DEB) << "binEdges[" << i << "] = " << binEdges[i] << std::endl;
  }

  fDataHist = new TH1D((fName+"_data").c_str(), (fName+"_data"+fPlotTitles).c_str(), nPoints, binEdges);

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
void T2K_CC1pip_CH_XSec_1DQ2_nu::SetCovarMatrix(std::string fileLocation) {
  LOG(DEB) << "Covariance: " << fileLocation.c_str() << std::endl;
  TFile *dataFile = new TFile(fileLocation.c_str()); //truly great .root file!

  TH2D *covarMatrix = (TH2D*)(dataFile->Get("TMatrixDBase;1"))->Clone();

  int nBinsX = covarMatrix->GetXaxis()->GetNbins();
  int nBinsY = covarMatrix->GetYaxis()->GetNbins();

  LOG(DEB) << nBinsX << std::endl;
  LOG(DEB) << fDataHist->GetNbinsX() << std::endl;

  if ((nBinsX != nBinsY)) ERR(WRN) << "covariance matrix not square!" << std::endl;

  this->covar = new TMatrixDSym(nBinsX-7);
  this->fFullCovar = new TMatrixDSym(nBinsX-7);

  // First two entries are BS
  // Last entry is BS
  for (int i = 0; i < nBinsX-7; i++) {
    for (int j = 0; j < nBinsY-7; j++) {
      (*this->covar)(i, j) = covarMatrix->GetBinContent(i+3, j+3); //adds syst+stat covariances
      (*this->fFullCovar)(i, j) = covarMatrix->GetBinContent(i+3, j+3); //adds syst+stat covariances
      LOG(DEB) << "covar(" << i << ", " << j << ") = " << (*this->covar)(i,j) << std::endl;
    }
  } //should now have set covariance, I hope

  TDecompChol tempMat = TDecompChol(*this->covar);
  this->covar = new TMatrixDSym(nBinsX, tempMat.Invert().GetMatrixArray(), "");
  *this->covar *= 1E-38*1E-38;

  return;
};


void T2K_CC1pip_CH_XSec_1DQ2_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(13) == 0 ||
      event->NumFSParticle(211) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double q2 = -999;

  switch(fT2KSampleType) {

    // First int refers to how we reconstruct Enu
    // 0 uses true neutrino energy (not used here but common for other analyses when they unfold to true Enu from reconstructed Enu)
    // 1 uses "extended MiniBooNE" method
    // 2 uses "MiniBooNE reconstructed" method
    // 3 uses Delta resonance mass for reconstruction
    //
    // The last bool refers to if pion directional information was used
    //
    // Use MiniBooNE reconstructed Enu; uses Michel tag so no pion direction information
    case kMB:
      q2 = FitUtils::Q2CC1piprec(Pnu, Pmu, Ppip, 2, false);
      break;

    // Use Extended MiniBooNE reconstructed Enu
    // Needs pion information to reconstruct so bool is true (did not include Michel e tag)
    case keMB:
      q2 = FitUtils::Q2CC1piprec(Pnu, Pmu, Ppip, 1, true);
      break;

    // Use Delta resonance reconstructed Enu
    // Uses Michel electron so don't have pion directional information
    case kDelta:
      q2 = FitUtils::Q2CC1piprec(Pnu, Pmu, Ppip, 3, false);
      break;
  }


  fXVar = q2;

  return;
};

//********************************************************************
bool T2K_CC1pip_CH_XSec_1DQ2_nu::isSignal(FitEvent *event) {
//********************************************************************
// Warning: The CH analysis has different signal definition to the H2O analysis!
//          Often to do with the Michel tag

  switch(fT2KSampleType) {
    // Using MiniBooNE formula for Enu reconstruction on the Q2 variable
    // Does have Michel e tag, set bool to true!
    case kMB:
      return SignalDef::isCC1pip_T2K_CH(event, EnuMin, EnuMax, true);
      break;
    // Using extended MiniBooNE formula for Enu reconstruction on the Q2 variable
    // Does not have Michel e tag because we need directional information to reconstruct Q2
    case keMB:
      return SignalDef::isCC1pip_T2K_CH(event, EnuMin, EnuMax, false);
      break;
    // Using Delta resonance for Enu reconstruction on the Q2 variable
    // Does have Michel e tag, bool to true
    case kDelta:
      return SignalDef::isCC1pip_T2K_CH(event, EnuMin, EnuMax, true);
      break;
  }

  // Default to return false
  return false;
}

