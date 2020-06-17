#include "T2K_nueCCinc_XSec_joint.h"

//********************************************************************
T2K_nueCCinc_XSec_joint::T2K_nueCCinc_XSec_joint(nuiskey samplekey){
//********************************************************************

  fSettings = LoadSampleSettings(samplekey);
  std::string descrip = "T2K_nueCCinc_XSec_joint. \n"
    "Target: CH \n"
    "Flux: T2K FHC nue  \n"
    "Signal: CC-inclusive \n";
  fSettings.SetTitle("T2K #nu_{e}-CC-inclusive p_{e} and cos#theta_{e} joint");
  fSettings.DefineAllowedSpecies("nue, nueb");
  fSettings.SetCovarInput(FitPar::GetDataBase() + "/T2K/CCinc/nue_2019/fract_covar_both.txt");
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("");
  fSettings.SetYTitle("#frac{d#sigma}{dx} (cm^{2}/nucleon)");
  fSettings.SetEnuRange(0.0, 30.0);
  fSettings.DefineAllowedTargets("C,H");
  FinaliseSampleSettings();

  if (fSubInFiles.size() != 3) {
    NUIS_ABORT("T2K nue joint requires input files in format: FHC nue; RHC nue; RHC nuebar");
  }

  std::string inFileFHCNue    = fSubInFiles.at(0);
  std::string inFileRHCNue    = fSubInFiles.at(1);
  std::string inFileRHCNuebar = fSubInFiles.at(2);

  // Make the Pe distributions
  nuiskey FHCNuePeKey = Config::CreateKey("sample");
  FHCNuePeKey.SetS("input", inFileFHCNue);
  FHCNuePeKey.SetS("type", fSettings.GetS("type"));
  FHCNuePeKey.SetS("name", "T2K_nueCCinc_XSec_1Dpe_FHC");
  FHC_nue_pe = new T2K_nueCCinc_XSec_1Dpe(FHCNuePeKey);

  nuiskey RHCNuePeKey = Config::CreateKey("sample");
  RHCNuePeKey.SetS("input", inFileRHCNue);
  RHCNuePeKey.SetS("type", fSettings.GetS("type"));
  RHCNuePeKey.SetS("name", "T2K_nueCCinc_XSec_1Dpe_RHC");
  RHC_nue_pe = new T2K_nueCCinc_XSec_1Dpe(RHCNuePeKey);

  nuiskey RHCNuebarPeKey = Config::CreateKey("sample");
  RHCNuebarPeKey.SetS("input", inFileRHCNuebar);
  RHCNuebarPeKey.SetS("type", fSettings.GetS("type"));
  RHCNuebarPeKey.SetS("name", "T2K_nuebarCCinc_XSec_1Dpe_RHC");
  RHC_nuebar_pe = new T2K_nueCCinc_XSec_1Dpe(RHCNuebarPeKey);

  // Now the thetae distributions
  nuiskey FHCNueTheKey = Config::CreateKey("sample");
  FHCNueTheKey.SetS("input", inFileFHCNue);
  FHCNueTheKey.SetS("type", fSettings.GetS("type"));
  FHCNueTheKey.SetS("name", "T2K_nueCCinc_XSec_1Dthe_FHC");
  FHC_nue_the = new T2K_nueCCinc_XSec_1Dthe(FHCNueTheKey);

  nuiskey RHCNueTheKey = Config::CreateKey("sample");
  RHCNueTheKey.SetS("input", inFileRHCNue);
  RHCNueTheKey.SetS("type", fSettings.GetS("type"));
  RHCNueTheKey.SetS("name", "T2K_nueCCinc_XSec_1Dthe_RHC");
  RHC_nue_the = new T2K_nueCCinc_XSec_1Dthe(RHCNueTheKey);

  nuiskey RHCNuebarTheKey = Config::CreateKey("sample");
  RHCNuebarTheKey.SetS("input", inFileRHCNuebar);
  RHCNuebarTheKey.SetS("type", fSettings.GetS("type"));
  RHCNuebarTheKey.SetS("name", "T2K_nuebarCCinc_XSec_1Dthe_RHC");
  RHC_nuebar_the = new T2K_nueCCinc_XSec_1Dthe(RHCNuebarTheKey);

  // Sort out the data hist
  this->CombineDataHists();

  // This is a fractional covariance. Need to account for that
  SetFractCovarFromTextFile(fSettings.GetCovarInput());
  SetShapeCovar();
  
  // Add to chain for processing
  fSubChain.clear();
  fSubChain.push_back(FHC_nue_pe);
  fSubChain.push_back(RHC_nue_pe);
  fSubChain.push_back(RHC_nuebar_pe);
  fSubChain.push_back(FHC_nue_the);
  fSubChain.push_back(RHC_nue_the);
  fSubChain.push_back(RHC_nuebar_the);

  // This saves information from the sub-measurements
  fSaveSubMeas = true;
  FinaliseMeasurement();
};

//********************************************************************
void T2K_nueCCinc_XSec_joint::SetFractCovarFromTextFile(std::string covfile){
//********************************************************************

  if (!fDataHist){
    NUIS_ERR(FTL, "fDataHist is required to call T2K_nueCCinc_XSec_joint::SetFractCovarFromTextFile");
    throw;
  }

  NUIS_LOG(SAM, "Reading fractional covariance from text file: " << covfile);

  int dim = fDataHist->GetNbinsX();
  TMatrixD *tempmat = StatUtils::GetMatrixFromTextFile(covfile, dim, dim);

  // Make a symmetric covariance
  fFullCovar = new TMatrixDSym(tempmat->GetNrows());
  for (int i = 0; i < tempmat->GetNrows(); i++) {
    for (int j = 0; j < tempmat->GetNrows(); j++) {
      (*fFullCovar)(i, j) = fDataHist->GetBinContent(i+1)*1E38
	*(*tempmat)(i, j)*fDataHist->GetBinContent(j+1)*1E38;
    }
  }

  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);
}


//********************************************************************
void T2K_nueCCinc_XSec_joint::CombineDataHists(){
//********************************************************************

  TH1D *FHC_nue_pe_data = (TH1D*)FHC_nue_pe->GetDataHistogram();
  TH1D *RHC_nue_pe_data = (TH1D*)RHC_nue_pe->GetDataHistogram();
  TH1D *RHC_nuebar_pe_data = (TH1D*)RHC_nuebar_pe->GetDataHistogram();
  TH1D *FHC_nue_the_data = (TH1D*)FHC_nue_the->GetDataHistogram();
  TH1D *RHC_nue_the_data = (TH1D*)RHC_nue_the->GetDataHistogram();
  TH1D *RHC_nuebar_the_data = (TH1D*)RHC_nuebar_the->GetDataHistogram();


  int nbins = FHC_nue_pe_data->GetNbinsX() +
    RHC_nue_pe_data->GetNbinsX() +
    RHC_nuebar_pe_data->GetNbinsX() +
    FHC_nue_the_data->GetNbinsX() +
    RHC_nue_the_data->GetNbinsX() +
    RHC_nuebar_the_data->GetNbinsX();

  fDataHist = new TH1D((fSettings.GetName() + "_data").c_str(),
                       (fSettings.GetFullTitles()).c_str(), nbins, 0, nbins);
  fDataHist->SetDirectory(0);
  
  // Bit ugly, but...
  int count = 0;
  
  // Start with pe
  for (int x=0; x<FHC_nue_pe_data->GetNbinsX(); ++x){
    fDataHist->SetBinContent(count+1, FHC_nue_pe_data->GetBinContent(x+1));
    fDataHist->SetBinError(count+1, FHC_nue_pe_data->GetBinError(x+1));
    fDataHist->GetXaxis()->SetBinLabel(count+1, Form("FHC #nu_{e} %.1f #leq p_{e} #leq %.1f",
                                                     FHC_nue_pe_data->GetXaxis()->GetBinLowEdge(x+1),
                                                     FHC_nue_pe_data->GetXaxis()->GetBinUpEdge(x+1)));
    count++;
  }
  for (int x=0; x<RHC_nue_pe_data->GetNbinsX(); ++x){
    fDataHist->SetBinContent(count+1, RHC_nue_pe_data->GetBinContent(x+1));
    fDataHist->SetBinError(count+1, RHC_nue_pe_data->GetBinError(x+1));
    fDataHist->GetXaxis()->SetBinLabel(count+1, Form("RHC #nu_{e} %.1f #leq p_{e} #leq %.1f",
                                                     RHC_nue_pe_data->GetXaxis()->GetBinLowEdge(x+1),
                                                     RHC_nue_pe_data->GetXaxis()->GetBinUpEdge(x+1)));
    count++;
  }
  for (int x=0; x<RHC_nuebar_pe_data->GetNbinsX(); ++x){
    fDataHist->SetBinContent(count+1, RHC_nuebar_pe_data->GetBinContent(x+1));
    fDataHist->SetBinError(count+1, RHC_nuebar_pe_data->GetBinError(x+1));
    fDataHist->GetXaxis()->SetBinLabel(count+1, Form("RHC #bar{#nu}_{e} %.1f #leq p_{e} #leq %.1f",
                                                     RHC_nuebar_pe_data->GetXaxis()->GetBinLowEdge(x+1),
                                                     RHC_nuebar_pe_data->GetXaxis()->GetBinUpEdge(x+1)));
    count++;
  }

  // Now thetee
  for (int x=0; x<FHC_nue_the_data->GetNbinsX(); ++x){
    fDataHist->SetBinContent(count+1, FHC_nue_the_data->GetBinContent(x+1));
    fDataHist->SetBinError(count+1, FHC_nue_the_data->GetBinError(x+1));
    fDataHist->GetXaxis()->SetBinLabel(count+1, Form("FHC #nu_{e} %.1f #leq cos#theta_e #leq %.1f",
                                                     FHC_nue_the_data->GetXaxis()->GetBinLowEdge(x+1),
                                                     FHC_nue_the_data->GetXaxis()->GetBinUpEdge(x+1)));
    count++;
  }
  for (int x=0; x<RHC_nue_the_data->GetNbinsX(); ++x){
    fDataHist->SetBinContent(count+1, RHC_nue_the_data->GetBinContent(x+1));
    fDataHist->SetBinError(count+1, RHC_nue_the_data->GetBinError(x+1));
    fDataHist->GetXaxis()->SetBinLabel(count+1, Form("RHC #nu_{e} %.1f #leq cos#theta_e #leq %.1f",
						     RHC_nue_the_data->GetXaxis()->GetBinLowEdge(x+1),
                                                     RHC_nue_the_data->GetXaxis()->GetBinUpEdge(x+1)));
    count++;
  }
  for (int x=0; x<RHC_nuebar_the_data->GetNbinsX(); ++x){
    fDataHist->SetBinContent(count+1, RHC_nuebar_the_data->GetBinContent(x+1));
    fDataHist->SetBinError(count+1, RHC_nuebar_the_data->GetBinError(x+1));
    fDataHist->GetXaxis()->SetBinLabel(count+1, Form("RHC #bar{#nu}_{e} %.1f #leq cos#theta_e #leq %.1f",
                                                     RHC_nuebar_the_data->GetXaxis()->GetBinLowEdge(x+1),
                                                     RHC_nuebar_the_data->GetXaxis()->GetBinUpEdge(x+1)));
    count++;
  }


}

//********************************************************************
void T2K_nueCCinc_XSec_joint::MakePlots() {
//********************************************************************

  TH1D *FHC_nue_pe_mc = (TH1D*)FHC_nue_pe->GetMCHistogram();
  TH1D *RHC_nue_pe_mc = (TH1D*)RHC_nue_pe->GetMCHistogram();
  TH1D *RHC_nuebar_pe_mc = (TH1D*)RHC_nuebar_pe->GetMCHistogram();
  TH1D *FHC_nue_the_mc = (TH1D*)FHC_nue_the->GetMCHistogram();
  TH1D *RHC_nue_the_mc = (TH1D*)RHC_nue_the->GetMCHistogram();
  TH1D *RHC_nuebar_the_mc = (TH1D*)RHC_nuebar_the->GetMCHistogram();
  
  int count = 0;
  // Start with pe
  for (int i = 0; i < FHC_nue_pe_mc->GetNbinsX(); ++i) {
    fMCHist->SetBinContent(count + 1, FHC_nue_pe_mc->GetBinContent(i + 1));
    fMCHist->SetBinError(count + 1, FHC_nue_pe_mc->GetBinError(i + 1));
    count++;
  }
  for (int i = 0; i < RHC_nue_pe_mc->GetNbinsX(); ++i) {
    fMCHist->SetBinContent(count + 1, RHC_nue_pe_mc->GetBinContent(i + 1));
    fMCHist->SetBinError(count + 1, RHC_nue_pe_mc->GetBinError(i + 1));
    count++;
  }
  for (int i = 0; i < RHC_nuebar_pe_mc->GetNbinsX(); ++i) {
    fMCHist->SetBinContent(count + 1, RHC_nuebar_pe_mc->GetBinContent(i + 1));
    fMCHist->SetBinError(count + 1, RHC_nuebar_pe_mc->GetBinError(i + 1));
    count++;
  }

  // Now thetae
  for (int i = 0; i < FHC_nue_the_mc->GetNbinsX(); ++i) {
    fMCHist->SetBinContent(count + 1, FHC_nue_the_mc->GetBinContent(i + 1));
    fMCHist->SetBinError(count + 1, FHC_nue_the_mc->GetBinError(i + 1));
    count++;
  }
  for (int i = 0; i < RHC_nue_the_mc->GetNbinsX(); ++i) {
    fMCHist->SetBinContent(count + 1, RHC_nue_the_mc->GetBinContent(i + 1));
    fMCHist->SetBinError(count + 1, RHC_nue_the_mc->GetBinError(i + 1));
    count++;
  }
  for (int i = 0; i < RHC_nuebar_the_mc->GetNbinsX(); ++i) {
    fMCHist->SetBinContent(count + 1, RHC_nuebar_the_mc->GetBinContent(i + 1));
    fMCHist->SetBinError(count + 1, RHC_nuebar_the_mc->GetBinError(i + 1));
    count++;
  }

  return;
}
