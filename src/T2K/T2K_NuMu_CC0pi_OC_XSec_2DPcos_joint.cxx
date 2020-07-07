#include "T2K_NuMu_CC0pi_OC_XSec_2DPcos_joint.h"

//********************************************************************
T2K_NuMu_CC0pi_OC_XSec_2DPcos_joint::T2K_NuMu_CC0pi_OC_XSec_2DPcos_joint(nuiskey samplekey){
//********************************************************************

  fSettings = LoadSampleSettings(samplekey);
  std::string descrip = "T2K_NuMu_CC0pi_OC_XSec_2DPcos_joint. \n"
                        "Target: O-C \n"
                        "Flux: T2K 2.5 degree off-axis (ND280)  \n"
                        "Signal: CC0pi\n"
                        "arXiv:2004.05434";
  fSettings.SetTitle("T2K_NuMu_CC0pi_OC_XSec_2DPcos_joint");
  fSettings.DefineAllowedSpecies("numu");
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("p_{#mu}-cos#theta_{#mu}");
  fSettings.SetYTitle("d^{2}#sigma/dP_{#mu}dcos#theta_{#mu} (cm^{2}/GeV)");
  fSettings.SetAllowedTypes("DIAG,FULL/FREE,SHAPE,FIX/SYSTCOV/STATCOV","FIX");
  fSettings.SetEnuRange(0.0, 30.0);
  fSettings.DefineAllowedTargets("O,C");
  FinaliseSampleSettings();


  if (fSubInFiles.size() != 2) {
    NUIS_ABORT("T2K NuMuCC0pi O-C joint requires input files in format: NuMuCC0pi on O and NuMuCC0pi on C");
  }

  std::string inFileNuMuO = fSubInFiles.at(0);
  std::string inFileNuMuC = fSubInFiles.at(1);

  // Create some config keys
  nuiskey NuMuCC0piOKey = Config::CreateKey("sample");
  NuMuCC0piOKey.SetS("input", inFileNuMuO);
  NuMuCC0piOKey.SetS("type",  fSettings.GetS("type"));
  NuMuCC0piOKey.SetS("name", "T2K_NuMu_CC0pi_O_XSec_2DPcos");
  NuMuCC0piO = new T2K_NuMu_CC0pi_OC_XSec_2DPcos(NuMuCC0piOKey);

  nuiskey NuMuCC0piCKey = Config::CreateKey("sample");
  NuMuCC0piCKey.SetS("input", inFileNuMuC);
  NuMuCC0piCKey.SetS("type",  fSettings.GetS("type"));
  NuMuCC0piCKey.SetS("name", "T2K_NuMu_CC0pi_C_XSec_2DPcos");
  NuMuCC0piC = new T2K_NuMu_CC0pi_OC_XSec_2DPcos(NuMuCC0piCKey);

  // Sort out the data hist
  this->CombineDataHists();

  // Set the covariance
  SetCovariance();
  
  // Add to chain for processing
  fSubChain.clear();
  fSubChain.push_back(NuMuCC0piO);
  fSubChain.push_back(NuMuCC0piC);

  // This saves information from the sub-measurements
  fSaveSubMeas = true;
  FinaliseMeasurement();
};

//********************************************************************
void T2K_NuMu_CC0pi_OC_XSec_2DPcos_joint::SetCovariance(){
//********************************************************************

  // Read covariance matrix
  fInputFileCov = new TFile( (FitPar::GetDataBase() + "/T2K/CC0pi/JointO-C/covmatrix_noreg.root").c_str(),"READ");

  TMatrixDSym* tempcov = (TMatrixDSym*) fInputFileCov->Get("covmatrixOeCbin");
  
  fFullCovar = new TMatrixDSym(tempcov->GetNrows());

  for(int ibin=0; ibin<tempcov->GetNrows(); ibin++) {  
    for(int jbin=0; jbin<tempcov->GetNrows(); jbin++) {
      // The factor 1E-2 needed since the covariance matrix in the 
      // data release is divided by 1E-78
      (*fFullCovar)(ibin,jbin) = (*tempcov)(ibin,jbin)*1E-2;
    }
  }
  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);

  return;
}


//********************************************************************
void T2K_NuMu_CC0pi_OC_XSec_2DPcos_joint::CombineDataHists(){
//********************************************************************

  TH1D *hNuMuOData = (TH1D*)NuMuCC0piO->GetDataHistogram();
  TH1D *hNuMuCData = (TH1D*)NuMuCC0piC->GetDataHistogram();

  int nbins = hNuMuOData->GetNbinsX() + hNuMuCData->GetNbinsX();

  fDataHist = new TH1D((fSettings.GetName() + "_data").c_str(),
                       (fSettings.GetFullTitles()).c_str(), nbins, 0, nbins);
  fDataHist->SetDirectory(0);
  
  int count = 0;
  for (int x=0; x<hNuMuOData->GetNbinsX(); ++x){
    fDataHist->SetBinContent(count+1, hNuMuOData->GetBinContent(x+1));
    fDataHist->SetBinError(count+1,   hNuMuOData->GetBinError(x+1));
    fDataHist->GetXaxis()->SetBinLabel(count+1, Form("NuMu CC0pi %.1f-%.1f", hNuMuOData->GetXaxis()->GetBinLowEdge(x+1), hNuMuOData->GetXaxis()->GetBinUpEdge(x+1)));
    count++;
  }
  for (int x=0; x<hNuMuCData->GetNbinsX(); ++x){
    fDataHist->SetBinContent(count+1, hNuMuCData->GetBinContent(x+1));
    fDataHist->SetBinError(count+1,   hNuMuCData->GetBinError(x+1));
    fDataHist->GetXaxis()->SetBinLabel(count+1, Form("AntiNuMu CC0pi %.1f-%.1f", hNuMuCData->GetXaxis()->GetBinLowEdge(x+1), hNuMuCData->GetXaxis()->GetBinUpEdge(x+1)));
    count++;
  }
}

//********************************************************************
void T2K_NuMu_CC0pi_OC_XSec_2DPcos_joint::SetHistograms() {
//********************************************************************

  NuMuCC0piO->SetHistograms();
  NuMuCC0piC->SetHistograms();

  return;
}

//********************************************************************
void T2K_NuMu_CC0pi_OC_XSec_2DPcos_joint::FillHistograms() {
//********************************************************************

  NuMuCC0piO->FillHistograms();
  NuMuCC0piC->FillHistograms();

  return;
}

//********************************************************************
void T2K_NuMu_CC0pi_OC_XSec_2DPcos_joint::ConvertEventRates() {
//********************************************************************
  
  NuMuCC0piO->ConvertEventRates();
  NuMuCC0piC->ConvertEventRates();

  TH1D* hNuMuCC0piO = (TH1D*)NuMuCC0piO->GetMCHistogram();
  TH1D* hNuMuCC0piC = (TH1D*)NuMuCC0piC->GetMCHistogram();
  
  int count = 0;
  for (int i = 0; i < hNuMuCC0piO->GetNbinsX(); ++i) {
    fMCHist->SetBinContent(count + 1, hNuMuCC0piO->GetBinContent(i + 1));
    fMCHist->SetBinError(count + 1, hNuMuCC0piO->GetBinError(i + 1));
    count++;
  }
  for (int i = 0; i < hNuMuCC0piC->GetNbinsX(); ++i) {
    fMCHist->SetBinContent(count + 1, hNuMuCC0piC->GetBinContent(i + 1));
    fMCHist->SetBinError(count + 1, hNuMuCC0piC->GetBinError(i + 1));
    count++;
  }

  return;  
}





