#include "T2K_nueCCinc_XSec_1Dpe_joint.h"

//********************************************************************
T2K_nueCCinc_XSec_1Dpe_joint::T2K_nueCCinc_XSec_1Dpe_joint(nuiskey samplekey){
//********************************************************************

  fSettings = LoadSampleSettings(samplekey);
  std::string descrip = "T2K_nueCCinc_XSec_1Dpe_joint. \n"
    "Target: CH \n"
    "Flux: T2K FHC nue  \n"
    "Signal: CC-inclusive \n";
  fSettings.SetTitle("T2K #nu_{e}-CC-inclusive p_{e} joint");
  fSettings.DefineAllowedSpecies("nue, nueb");
  fSettings.SetCovarInput(FitPar::GetDataBase() + "/T2K/CCinc/nue_2019/fract_covar_pe.txt");
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("p_{e} (GeV)");
  fSettings.SetYTitle("#frac{d#sigma}{dp_{e}} (cm^{2}/nucleon)");
  fSettings.SetEnuRange(0.0, 30.0);
  fSettings.DefineAllowedTargets("C,H");
  FinaliseSampleSettings();

  if (fSubInFiles.size() != 3) {
    NUIS_ABORT("T2K nue joint requires input files in format: FHC nue; RHC nue; RHC nuebar");
  }

  std::string inFileFHCNue    = fSubInFiles.at(0);
  std::string inFileRHCNue    = fSubInFiles.at(1);
  std::string inFileRHCNuebar = fSubInFiles.at(2);

  // Create some config keys
  nuiskey FHCNueKey = Config::CreateKey("sample");
  FHCNueKey.SetS("input", inFileFHCNue);
  FHCNueKey.SetS("type", fSettings.GetS("type"));
  FHCNueKey.SetS("name", "T2K_nueCCinc_XSec_1Dpe_FHC");
  FHC_nue = new T2K_nueCCinc_XSec_1Dpe(FHCNueKey);

  nuiskey RHCNueKey = Config::CreateKey("sample");
  RHCNueKey.SetS("input", inFileRHCNue);
  RHCNueKey.SetS("type", fSettings.GetS("type"));
  RHCNueKey.SetS("name", "T2K_nueCCinc_XSec_1Dpe_RHC");
  RHC_nue = new T2K_nueCCinc_XSec_1Dpe(RHCNueKey);

  nuiskey RHCNuebarKey = Config::CreateKey("sample");
  RHCNuebarKey.SetS("input", inFileRHCNuebar);
  RHCNuebarKey.SetS("type", fSettings.GetS("type"));
  RHCNuebarKey.SetS("name", "T2K_nuebarCCinc_XSec_1Dpe_RHC");
  RHC_nuebar = new T2K_nueCCinc_XSec_1Dpe(RHCNuebarKey);

  // Sort out the data hist
  this->CombineDataHists();

  // This is a fractional covariance. Need to account for that
  SetFractCovarFromTextFile(fSettings.GetCovarInput());
  SetShapeCovar();
  
  // Add to chain for processing
  fSubChain.clear();
  fSubChain.push_back(FHC_nue);
  fSubChain.push_back(RHC_nue);
  fSubChain.push_back(RHC_nuebar);

  // This saves information from the sub-measurements
  fSaveSubMeas = true;
  FinaliseMeasurement();
};

//********************************************************************
void T2K_nueCCinc_XSec_1Dpe_joint::SetFractCovarFromTextFile(std::string covfile){
//********************************************************************

  if (!fDataHist){
    NUIS_ERR(FTL, "fDataHist is required to call T2K_nueCCinc_XSec_1Dpe_joint::SetFractCovarFromTextFile");
    throw;
  }

  NUIS_LOG(SAM, "Reading fractional covariance from text file: " << covfile);

  int dim = fDataHist->GetNbinsX();
  TMatrixD *tempmat = StatUtils::GetMatrixFromTextFile(covfile, dim, dim);

  // Make a symmetric covariance
  fFullCovar = new TMatrixDSym(tempmat->GetNrows());
  for (int i = 0; i < tempmat->GetNrows(); i++) {
    for (int j = 0; j < tempmat->GetNrows(); j++) {
      (*fFullCovar)(i, j) = fDataHist->GetBinContent(i+1)*1e38
	*(*tempmat)(i, j)*fDataHist->GetBinContent(j+1)*1e38;
    }
  }

  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);
}


//********************************************************************
void T2K_nueCCinc_XSec_1Dpe_joint::CombineDataHists(){
//********************************************************************

  TH1D *FHC_nue_data = (TH1D*)FHC_nue->GetDataHistogram();
  TH1D *RHC_nue_data = (TH1D*)RHC_nue->GetDataHistogram();
  TH1D *RHC_nuebar_data = (TH1D*)RHC_nuebar->GetDataHistogram();

  int nbins = FHC_nue_data->GetNbinsX() + 
    RHC_nue_data->GetNbinsX() + 
    RHC_nuebar_data->GetNbinsX();

  fDataHist = new TH1D((fSettings.GetName() + "_data").c_str(),
                       (fSettings.GetFullTitles()).c_str(), nbins, 0, nbins);
  fDataHist->SetDirectory(0);
  
  // Bit ugly, but...
  int count = 0;
  for (int x=0; x<FHC_nue_data->GetNbinsX(); ++x){
    fDataHist->SetBinContent(count+1, FHC_nue_data->GetBinContent(x+1));
    fDataHist->SetBinError(count+1, FHC_nue_data->GetBinError(x+1));
    fDataHist->GetXaxis()->SetBinLabel(count+1, Form("FHC #nu_{e} %.1f-%.1f",
						     FHC_nue_data->GetXaxis()->GetBinLowEdge(x+1),
						     FHC_nue_data->GetXaxis()->GetBinUpEdge(x+1)));
    count++;
  }
  for (int x=0; x<RHC_nue_data->GetNbinsX(); ++x){
    fDataHist->SetBinContent(count+1, RHC_nue_data->GetBinContent(x+1));
    fDataHist->SetBinError(count+1, RHC_nue_data->GetBinError(x+1));
    fDataHist->GetXaxis()->SetBinLabel(count+1, Form("RHC #nu_{e} %.1f-%.1f",
                                                     RHC_nue_data->GetXaxis()->GetBinLowEdge(x+1),
                                                     RHC_nue_data->GetXaxis()->GetBinUpEdge(x+1)));
    count++;
  }
  for (int x=0; x<RHC_nuebar_data->GetNbinsX(); ++x){
    fDataHist->SetBinContent(count+1, RHC_nuebar_data->GetBinContent(x+1));
    fDataHist->SetBinError(count+1, RHC_nuebar_data->GetBinError(x+1));
    fDataHist->GetXaxis()->SetBinLabel(count+1, Form("RHC #bar{#nu}_{e} %.1f-%.1f",
                                                     RHC_nuebar_data->GetXaxis()->GetBinLowEdge(x+1),
                                                     RHC_nuebar_data->GetXaxis()->GetBinUpEdge(x+1)));
    count++;
  } 
}

//********************************************************************
void T2K_nueCCinc_XSec_1Dpe_joint::MakePlots() {
//********************************************************************

  TH1D *FHC_nue_mc = (TH1D*)FHC_nue->GetMCHistogram();
  TH1D *RHC_nue_mc = (TH1D*)RHC_nue->GetMCHistogram();
  TH1D *RHC_nuebar_mc = (TH1D*)RHC_nuebar->GetMCHistogram();

  int count = 0;
  for (int i = 0; i < FHC_nue_mc->GetNbinsX(); ++i) {
    fMCHist->SetBinContent(count + 1, FHC_nue_mc->GetBinContent(i + 1));
    fMCHist->SetBinError(count + 1, FHC_nue_mc->GetBinError(i + 1));
    count++;
  }
  for (int i = 0; i < RHC_nue_mc->GetNbinsX(); ++i) {
    fMCHist->SetBinContent(count + 1, RHC_nue_mc->GetBinContent(i + 1));
    fMCHist->SetBinError(count + 1, RHC_nue_mc->GetBinError(i + 1));
    count++;
  }
  for (int i = 0; i < RHC_nuebar_mc->GetNbinsX(); ++i) {
    fMCHist->SetBinContent(count + 1, RHC_nuebar_mc->GetBinContent(i + 1));
    fMCHist->SetBinError(count + 1, RHC_nuebar_mc->GetBinError(i + 1));
    count++;
  }

  return;
}
