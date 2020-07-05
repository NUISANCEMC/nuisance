#include "T2K_NuMuAntiNuMu_CC0pi_XSec_joint.h"

//********************************************************************
T2K_NuMuAntiNuMu_CC0pi_XSec_joint::T2K_NuMuAntiNuMu_CC0pi_XSec_joint(nuiskey samplekey){
//********************************************************************

  fSettings = LoadSampleSettings(samplekey);
  std::string descrip = "T2K_NuMuAntiNuMu_CC0pi_XSec_joint. \n"
                        "Target: CH \n"
                        "Flux: T2K 2.5 degree off-axis (ND280)  \n"
                        "Signal: CC0pi\n"
                        "arXiv:2002.09323";
  fSettings.SetTitle("T2K_NuMuAntiNuMu_CC0pi_XSec_joint");
  fSettings.DefineAllowedSpecies("numu, numub");
  fSettings.SetDescription(descrip);
  fSettings.SetYTitle("d^{2}#sigma/dP_{#mu}dcos#theta_{#mu} (cm^{2}/GeV)");
  fSettings.SetAllowedTypes("DIAG,FULL/FREE,SHAPE,FIX/SYSTCOV/STATCOV","FIX");
  fSettings.SetEnuRange(0.0, 30.0);
  fSettings.DefineAllowedTargets("C,H");
  FinaliseSampleSettings();


  if (fSubInFiles.size() != 2) {
    NUIS_ABORT("T2K NuMu-AntiNuMu joint requires input files in format: NuMu and AntiNuMu");
  }

  std::string inFileNuMu     = fSubInFiles.at(0);
  std::string inFileAntiNuMu = fSubInFiles.at(1);

  // Create some config keys
  nuiskey NuMuKey = Config::CreateKey("sample");
  NuMuKey.SetS("input", inFileNuMu);
  NuMuKey.SetS("type",  fSettings.GetS("type"));
  NuMuKey.SetS("name", "T2K_NuMu_CC0pi_CH_XSec_2DPcos");
  NuMuCC0pi = new T2K_NuMuAntiNuMu_CC0pi_CH_XSec_2DPcos(NuMuKey);

  nuiskey AntiNuMuKey = Config::CreateKey("sample");
  AntiNuMuKey.SetS("input", inFileAntiNuMu);
  AntiNuMuKey.SetS("type",  fSettings.GetS("type"));
  AntiNuMuKey.SetS("name", "T2K_AntiNuMu_CC0pi_CH_XSec_2DPcos");
  AntiNuMuCC0pi = new T2K_NuMuAntiNuMu_CC0pi_CH_XSec_2DPcos(AntiNuMuKey);

  // Sort out the data hist
  this->CombineDataHists();

  // This is a fractional covariance. Need to account for that
  //SetFractCovarFromTextFile(fSettings.GetCovarInput());
  //ScaleCovar(1E76);
  //SetShapeCovar();
  
  // Add to chain for processing
  fSubChain.clear();
  fSubChain.push_back(NuMuCC0pi);
  fSubChain.push_back(AntiNuMuCC0pi);

  // This saves information from the sub-measurements
  fSaveSubMeas = true;
  FinaliseMeasurement();
};

//********************************************************************
void T2K_NuMuAntiNuMu_CC0pi_XSec_joint::CombineDataHists(){
//********************************************************************

  TH1D *hNuMuData     = (TH1D*)NuMuCC0pi->GetDataHistogram();
  TH1D *hAntiNuMuData = (TH1D*)AntiNuMuCC0pi->GetDataHistogram();

  int nbins = hNuMuData->GetNbinsX() + hAntiNuMuData->GetNbinsX();

  fDataHist = new TH1D((fSettings.GetName() + "_data").c_str(),
                       (fSettings.GetFullTitles()).c_str(), nbins, 0, nbins);
  fDataHist->SetDirectory(0);
  
  // Bit ugly, but...
  int count = 0;
  for (int x=0; x<hNuMuData->GetNbinsX(); ++x){
    fDataHist->SetBinContent(count+1, hNuMuData->GetBinContent(x+1));
    fDataHist->SetBinError(count+1,   hNuMuData->GetBinError(x+1));
    fDataHist->GetXaxis()->SetBinLabel(count+1, Form("NuMu CC0pi %.1f-%.1f", hNuMuData->GetXaxis()->GetBinLowEdge(x+1), hNuMuData->GetXaxis()->GetBinUpEdge(x+1)));
    count++;
  }
  for (int x=0; x<hAntiNuMuData->GetNbinsX(); ++x){
    fDataHist->SetBinContent(count+1, hAntiNuMuData->GetBinContent(x+1));
    fDataHist->SetBinError(count+1,   hAntiNuMuData->GetBinError(x+1));
    fDataHist->GetXaxis()->SetBinLabel(count+1, Form("AntiNuMu CC0pi %.1f-%.1f", hAntiNuMuData->GetXaxis()->GetBinLowEdge(x+1), hAntiNuMuData->GetXaxis()->GetBinUpEdge(x+1)));
    count++;
  }
}

//********************************************************************
void T2K_NuMuAntiNuMu_CC0pi_XSec_joint::CombineMCHists() {
//********************************************************************

  TH1D *hNuMuMC     = (TH1D*)NuMuCC0pi->GetMCHistogram();
  TH1D *hAntiNuMuMC = (TH1D*)AntiNuMuCC0pi->GetMCHistogram();

  int count = 0;
  for (int i = 0; i < hNuMuMC->GetNbinsX(); ++i) {
    fMCHist->SetBinContent(count + 1, hNuMuMC->GetBinContent(i + 1));
    fMCHist->SetBinError(count + 1, hNuMuMC->GetBinError(i + 1));
    count++;
  }
  for (int i = 0; i < hAntiNuMuMC->GetNbinsX(); ++i) {
    fMCHist->SetBinContent(count + 1, hAntiNuMuMC->GetBinContent(i + 1));
    fMCHist->SetBinError(count + 1, hAntiNuMuMC->GetBinError(i + 1));
    count++;
  }

  return;
}
