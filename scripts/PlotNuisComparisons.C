#include "TH1.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TLegend.h"
#include "TStyle.h"

#include <iostream>

void Draw1DComparison(TH1D *data, std::vector<TH1D*> mc, std::vector<std::string> legtitles, TCanvas *c1){

  gStyle->SetOptTitle(0);

  // Fix labels for 1D by-bin-number Plots
  if (data && (strcmp(data->GetName(),"T2K_CC0pi_XSec_2DPcos_nu_I_DATA_1D")==0 || strcmp(data->GetName(),"T2K_CC0pi_XSec_2DPcos_nu_II_data_1D")==0 || strcmp(data->GetName(),"MicroBooNE_CCInc_XSec_2DPcos_nu_DATA")==0 || strcmp(data->GetName(),"error_bands_MicroBooNE_CCInc_XSec_2DPcos_nu_DATA")==0 || strcmp(data->GetName(),"nominal_throw_MicroBooNE_CCInc_XSec_2DPcos_nu_DATA")==0 || strcmp(data->GetName(),"MiniBooNE_CCQE_XSec_2DTcos_nu_data_1D")==0 || strcmp(data->GetName(),"MiniBooNE_CCQELike_XSec_2DTcos_nu_data_1D")==0 || strcmp(data->GetName(),"T2K_CCinc_XSec_2DPcos_nu_nonuniform_data")==0) && mc.size() > 0){
    mc.at(0)->GetXaxis()->SetTitle("Bin number");
    mc.at(0)->GetYaxis()->SetTitle("Cross section");

  }

  // Split TCanvas (only if we have data to draw)
  c1->cd();
  TPad *topPad = new TPad("topPad", "", 0.005, 0.3, 0.995, 0.995);
  TPad *bottomPad = new TPad("bottomPad", "", 0.005, 0.005, 0.995, 0.3);
  if (data){
    // topPad->SetTopMargin(0.11);
    topPad->SetBottomMargin(0.02);
    bottomPad->SetTopMargin(0.0);
    bottomPad->SetBottomMargin(0.20);
    bottomPad->SetGridy();
    // topPad->Draw();
    // bottomPad->Draw();

    topPad->AppendPad();
bottomPad->AppendPad();
  }

  // Style MC
  for (int i=0; i<mc.size(); i++){
    mc.at(i)->SetLineWidth(2);
    mc.at(i)->SetLineColor(kRed+i);
    mc.at(i)->SetFillColor(kRed+i);
    mc.at(i)->SetFillStyle(3002);

    // Set line styles
    // if (i==0) mc.at(i)->SetLineStyle(1);
    // else if (i==1) mc.at(i)->SetLineStyle(9);
    // else if (i==2) mc.at(i)->SetLineStyle(9);
    // else mc.at(i)->SetLineStyle(i);
    mc.at(i)->SetLineStyle(i+1);
  }


  // Style data
  if (data){
    data->SetMarkerColor(kBlack);
    data->SetMarkerStyle(8);
    data->SetLineColor(kBlack);
    data->SetLineWidth(1);
  }

  // Dummy TGraph for drawing with whatever axes I like
  TH1D *gr = new TH1D("gr","",10000,mc.at(0)->GetXaxis()->GetXmin(),mc.at(0)->GetXaxis()->GetXmax());
  gr->GetXaxis()->SetTitle(mc.at(0)->GetXaxis()->GetTitle());
  gr->GetYaxis()->SetTitle(mc.at(0)->GetYaxis()->GetTitle());
  gr->GetYaxis()->SetTitleOffset(0.6);
  gr->GetXaxis()->SetLabelOffset(1);

  // Draw top pad
  if (data) topPad->cd();
  else c1->cd();

  if (mc.at(0)->GetXaxis()->GetBinUpEdge(mc.at(0)->GetNbinsX()) >= 5 && mc.at(0)->GetXaxis()->GetBinUpEdge(mc.at(0)->GetNbinsX()) < 35 && mc.at(0)->GetXaxis()->GetXmin() >= 0){
    // Don't change axis for T2KCC0piWithProtons sample
    if (data && std::string(data->GetName()).find("T2K_CC0piWithProtons_XSec_2018")!=std::string::npos){
      // do nothing
    }
    else{
      std::cout << "Limiting x-axis range to 5 GeV instead of " << mc.at(0)->GetXaxis()->GetBinUpEdge(mc.at(0)->GetNbinsX()) << std::endl;
      gr->GetXaxis()->SetRangeUser(mc.at(0)->GetXaxis()->GetXmin(),5.0);
    }
  }
  if (data && std::string(data->GetName()).find("T2K_CC1pip_CH_XSec_1Dppi")!=std::string::npos){
    std::cout << mc.at(0)->GetName() << " special case: Limiting x-axis range to 2 GeV instead of " << mc.at(0)->GetXaxis()->GetBinUpEdge(mc.at(0)->GetNbinsX()) << std::endl;
    gr->GetXaxis()->SetRangeUser(mc.at(0)->GetXaxis()->GetXmin(),2.0);
  }
  gr->SetFillStyle(0);
  //gr->SetTitle("HIST");
  c1->SetTitle("HIST");
  gr->Draw("");

  for (int i=0; i<mc.size(); i++){
    if (data && std::string(data->GetName()).find("nominal_throw")!=std::string::npos){
      mc.at(i)->Draw("E2 same");
    }
    TH1D *dummy = (TH1D*)mc.at(i)->Clone("dummy");
    dummy->SetFillStyle(0);
    dummy->Draw("hist same");
  }
  if (data) data->Draw("p e0 same");

  // Set maximum on y axis
  double ymax = -9999;
  if (data) ymax = data->GetMaximum();
  for (int i=0; i<mc.size(); i++){
    if (mc.at(i)->GetMaximum() > ymax){
      ymax = mc.at(i)->GetMaximum();
    }
  }

  gr->GetYaxis()->SetRangeUser(0,ymax*1.4);

  // Make legend
  TLegend *l = new TLegend(0.5,0.7,0.90,0.88);
  l->SetFillStyle(0);
  l->SetLineColor(kBlack);
  for (int i=0; i<mc.size(); i++){
    std::string legentry = legtitles.at(i);
    // Get Chi2 (it should be in the title of the MC histogram)
    std::string mc_title = std::string(mc.at(i)->GetTitle()).substr(0,std::string(mc.at(i)->GetTitle()).find(".")+3);
    // Check if this is a chi2 (lazy: check if first number is a digit)
    if (isdigit(mc_title[0])){
      legentry = legtitles.at(i)+std::string(", #chi^{2} = ")+mc_title+std::string("/")+std::to_string(mc.at(i)->GetXaxis()->GetNbins())+std::string(" bins");
    }
    // uB CCinc error band only
    if (data && std::string(data->GetName()).find("nominal_throw")!=std::string::npos && std::string(data->GetName()).find("MicroBooNE_CCInc_XSec_2DPcos_nu_DATA")!=std::string::npos){
      legentry = legtitles.at(i)+std::string(", #chi^{2} = 27.90/42 bins");
    }

    l->AddEntry(mc.at(i),legentry.c_str(),"l");
  }
  if (data){
    std::string data_title = "Data";
    if (std::string(data->GetName()).find("T2K")!=std::string::npos) data_title = "T2K Data";
    else if (std::string(data->GetName()).find("MINERvA")!=std::string::npos) data_title = "MINER#nuA Data";
    else if (std::string(data->GetName()).find("MiniBooNE")!=std::string::npos) data_title = "MiniBooNE Data";
    else if (std::string(data->GetName()).find("MicroBooNE")!=std::string::npos) data_title = "MicroBooNE Data";
    else std::cout << "Did not match experiment name from " << data->GetName() << ". Adding to legend as 'Data'" << std::endl;
    l->AddEntry(data,data_title.c_str(),"lp");
  }

  l->Draw();

  // Draw ratios to data in bottom pad (if we have data to draw)
  TH1D *gr_ratio = (TH1D*)gr->Clone("gr_ratio");
  if (data){
    bottomPad->cd();
      if (data->GetXaxis()->GetBinUpEdge(data->GetNbinsX()) >= 5 && data->GetXaxis()->GetBinUpEdge(data->GetNbinsX()) < 35 && data->GetXaxis()->GetXmin() == 0){
	// Don't change axis for T2KCC0piWithProtons sample
	if (data && std::string(data->GetName()).find("T2K_CC0piWithProtons_XSec_2018")!=std::string::npos){
	  // do nothing   
	}
	else{
	  gr_ratio->GetXaxis()->SetRangeUser(data->GetXaxis()->GetXmin(),5.0);
	}
      }
      if (std::string(data->GetName()).find("T2K_CC1pip_CH_XSec_1Dppi")!=std::string::npos){
	gr_ratio->GetXaxis()->SetRangeUser(mc.at(0)->GetXaxis()->GetXmin(),2.0);
      }

    gr_ratio->GetYaxis()->SetTitle("Ratio to data");
    gr_ratio->GetYaxis()->SetRangeUser(0,2.1);
    gr_ratio->GetYaxis()->SetTitleOffset(0.3);
    gr_ratio->GetYaxis()->SetTitleSize(0.11);
    gr_ratio->GetYaxis()->SetLabelSize(0.09);
    gr_ratio->GetXaxis()->SetTitleSize(0.11);
    gr_ratio->GetXaxis()->SetLabelSize(0.08);
    gr_ratio->GetXaxis()->SetLabelOffset(0.01);
    gr_ratio->GetXaxis()->SetTitle(mc.at(0)->GetXaxis()->GetTitle());
    gr_ratio->GetXaxis()->SetTitleOffset(0.7);
    gr_ratio->SetTitle("");
    gr_ratio->SetFillStyle(0);
    gr_ratio->Draw("");
    TH1D *data_ratio = (TH1D*)data->Clone("data_ratio");
    data_ratio->Sumw2();
    data_ratio->Divide(data);
    data_ratio->Draw("same hist e0");

    for (int i=0; i<mc.size(); i++){
      TH1D *dummy = (TH1D*)mc.at(i)->Clone(Form("dummy_%i",i));
      dummy->Sumw2();
      dummy->Divide(data);
      dummy->GetYaxis()->SetRangeUser(0,2.1);
      if (data && std::string(data->GetName()).find("nominal_throw")!=std::string::npos){
	dummy->Draw("E2 same");
      }
      TH1D *dummy2 = (TH1D*)dummy->Clone("dummy2");
      dummy2->SetFillStyle(0);
      dummy2->Draw("hist same");
    }
  }
}

void PlotComparison(std::vector<std::string> infile_names, std::vector<std::string> legtitles){

  std::vector<std::pair<std::string,std::string>> Names_1D =
    {
      // CC0pi
      {"T2K_CC0pi_XSec_2DPcos_nu_I_DATA_1D","T2K_CC0pi_XSec_2DPcos_nu_I_MC"},
      {"T2K_CC0pi_XSec_2DPcos_nu_I_data_Slice0","T2K_CC0pi_XSec_2DPcos_nu_I_MC_Slice0"},
      {"T2K_CC0pi_XSec_2DPcos_nu_I_data_Slice1","T2K_CC0pi_XSec_2DPcos_nu_I_MC_Slice1"},
      {"T2K_CC0pi_XSec_2DPcos_nu_I_data_Slice2","T2K_CC0pi_XSec_2DPcos_nu_I_MC_Slice2"},
      {"T2K_CC0pi_XSec_2DPcos_nu_I_data_Slice3","T2K_CC0pi_XSec_2DPcos_nu_I_MC_Slice3"},
      {"T2K_CC0pi_XSec_2DPcos_nu_I_data_Slice4","T2K_CC0pi_XSec_2DPcos_nu_I_MC_Slice4"},
      {"T2K_CC0pi_XSec_2DPcos_nu_I_data_Slice5","T2K_CC0pi_XSec_2DPcos_nu_I_MC_Slice5"},
      {"T2K_CC0pi_XSec_2DPcos_nu_I_data_Slice6","T2K_CC0pi_XSec_2DPcos_nu_I_MC_Slice6"},
      {"T2K_CC0pi_XSec_2DPcos_nu_I_data_Slice7","T2K_CC0pi_XSec_2DPcos_nu_I_MC_Slice7"},
      {"T2K_CC0pi_XSec_2DPcos_nu_I_data_Slice8","T2K_CC0pi_XSec_2DPcos_nu_I_MC_Slice8"},
      {"","T2K_CC0pi_XSec_2DPcos_nu_I_RESIDUAL"},
      {"","T2K_CC0pi_XSec_2DPcos_nu_I_Chi2NMinusOne"},
      {"T2K_CC0pi_XSec_2DPcos_nu_II_data_1D","T2K_CC0pi_XSec_2DPcos_nu_II_MC_1D"},
      {"T2K_CC0pi_XSec_2DPcos_nu_II_data_px","T2K_CC0pi_XSec_2DPcos_nu_II_MC_px"},
      {"T2K_CC0pi_XSec_2DPcos_nu_II_data_py","T2K_CC0pi_XSec_2DPcos_nu_II_MC_py"},
      {"","T2K_CC0pi_XSec_2DPcos_nu_II_RESIDUAL"},
      {"","T2K_CC0pi_XSec_2DPcos_nu_II_Chi2NMinusOne"},
      // CCinc
      {"T2K_CCinc_XSec_2DPcos_nu_nonuniform_data","T2K_CCinc_XSec_2DPcos_nu_nonuniform_MC"},
      {"T2K_CCinc_XSec_2DPcos_nu_nonuniform_slice0_data","T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice0_MC"},
      {"T2K_CCinc_XSec_2DPcos_nu_nonuniform_slice1_data","T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice1_MC"},
      {"T2K_CCinc_XSec_2DPcos_nu_nonuniform_slice2_data","T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice2_MC"},
      {"T2K_CCinc_XSec_2DPcos_nu_nonuniform_slice3_data","T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice3_MC"},
      {"T2K_CCinc_XSec_2DPcos_nu_nonuniform_slice4_data","T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice4_MC"},
      {"T2K_CCinc_XSec_2DPcos_nu_nonuniform_slice5_data","T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice5_MC"},
      {"T2K_CCinc_XSec_2DPcos_nu_nonuniform_slice6_data","T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice6_MC"},
      {"T2K_CCinc_XSec_2DPcos_nu_nonuniform_slice7_data","T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice7_MC"},
      {"T2K_CCinc_XSec_2DPcos_nu_nonuniform_slice8_data","T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice8_MC"},
      {"T2K_CCinc_XSec_2DPcos_nu_nonuniform_slice9_data","T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice9_MC"},
      {"T2K_CCinc_XSec_2DPcos_nu_nonuniform_slice10_data","T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice10_MC"},
      {"","T2K_CCinc_XSec_2DPcos_nu_nonuniform_RESIDUAL"},
      {"","T2K_CCinc_XSec_2DPcos_nu_nonuniform_Chi2NMinusOne"},
      // CC1pi
      {"T2K_CC1pip_CH_XSec_2Dpmucosmu_nu_data","T2K_CC1pip_CH_XSec_2Dpmucosmu_nu_MC"},
      {"T2K_CC1pip_CH_XSec_2Dpmucosmu_nu_data_slice0","T2K_CC1pip_CH_XSec_2Dpmucosmu_nu_mc_slice0"},
      {"T2K_CC1pip_CH_XSec_2Dpmucosmu_nu_data_slice1","T2K_CC1pip_CH_XSec_2Dpmucosmu_nu_mc_slice1"},
      {"T2K_CC1pip_CH_XSec_2Dpmucosmu_nu_data_slice2","T2K_CC1pip_CH_XSec_2Dpmucosmu_nu_mc_slice2"},
      {"T2K_CC1pip_CH_XSec_2Dpmucosmu_nu_data_slice3","T2K_CC1pip_CH_XSec_2Dpmucosmu_nu_mc_slice3"},
      {"","T2K_CC1pip_CH_XSec_2Dpmucosmu_nu_RESIDUAL"},
      {"","T2K_CC1pip_CH_XSec_2Dpmucosmu_nu_Chi2NMinusOne"},
      {"T2K_CC1pip_CH_XSec_1Dppi_nu_data","T2K_CC1pip_CH_XSec_1Dppi_nu_MC"},
      {"","T2K_CC1pip_CH_XSec_1Dppi_nu_RESIDUAL"},
      {"","T2K_CC1pip_CH_XSec_1Dppi_nu_Chi2NMinusOne"},
      {"T2K_CC1pip_CH_XSec_1Dthpi_nu_data","T2K_CC1pip_CH_XSec_1Dthpi_nu_MC"},
      {"","T2K_CC1pip_CH_XSec_1Dthpi_nu_RESIDUAL"},
      {"","T2K_CC1pip_CH_XSec_1Dthpi_nu_Chi2NMinusOne"},
      {"T2K_CC1pip_CH_XSec_1Dthmupi_nu_data","T2K_CC1pip_CH_XSec_1Dthmupi_nu_MC"},
      {"","T2K_CC1pip_CH_XSec_1Dthmupi_nu_RESIDUAL"},
      {"","T2K_CC1pip_CH_XSec_1Dthmupi_nu_Chi2NMinusOne"},
      {"T2K_CC1pip_CH_XSec_1DQ2_nu_data","T2K_CC1pip_CH_XSec_1DQ2_nu_MC"},
      {"","T2K_CC1pip_CH_XSec_1DQ2_nu_RESIDUAL"},
      {"","T2K_CC1pip_CH_XSec_1DQ2_nu_Chi2NMinusOne"},
      {"T2K_CC1pip_CH_XSec_1DAdlerPhi_nu_data","T2K_CC1pip_CH_XSec_1DAdlerPhi_nu_MC"},
      {"","T2K_CC1pip_CH_XSec_1DAdlerPhi_nu_RESIDUAL"},
      {"","T2K_CC1pip_CH_XSec_1DAdlerPhi_nu_Chi2NMinusOne"},
      {"T2K_CC1pip_CH_XSec_1DCosThAdler_nu_data","T2K_CC1pip_CH_XSec_1DCosThAdler_nu_MC"},
      {"","T2K_CC1pip_CH_XSec_1DCosThAdler_nu_RESIDUAL"},
      {"","T2K_CC1pip_CH_XSec_1DCosThAdler_nu_Chi2NMinusOne"},
      // CC0piNp: STV
      {"T2K_CC0pinp_STV_XSec_1Ddpt_nu_data","T2K_CC0pinp_STV_XSec_1Ddpt_nu_MC"},
      {"","T2K_CC0pinp_STV_XSec_1Ddpt_nu_RESIDUAL"},
      {"","T2K_CC0pinp_STV_XSec_1Ddpt_nu_Chi2NMinusOne"},
      {"T2K_CC0pinp_STV_XSec_1Ddphit_nu_data","T2K_CC0pinp_STV_XSec_1Ddphit_nu_MC"},
      {"","T2K_CC0pinp_STV_XSec_1Ddphit_nu_RESIDUAL"},
      {"","T2K_CC0pinp_STV_XSec_1Ddphit_nu_Chi2NMinusOne"},
      {"T2K_CC0pinp_STV_XSec_1Ddat_nu_data","T2K_CC0pinp_STV_XSec_1Ddat_nu_MC"},
      {"","T2K_CC0pinp_STV_XSec_1Ddat_nu_RESIDUAL"},
      {"","T2K_CC0pinp_STV_XSec_1Ddat_nu_Chi2NMinusOne"},
      // CC0piNp: Inferred kinematics
      // -- infp
      {"T2K_CC0pinp_ifk_XSec_3Dinfp_nu_data","T2K_CC0pinp_ifk_XSec_3Dinfp_nu_MC"},
      {"","T2K_CC0pinp_ifk_XSec_3Dinfp_nu_RESIDUAL"},
      {"","T2K_CC0pinp_ifk_XSec_3Dinfp_nu_Chi2NMinusOne"},
      {"T2K_CC0pinp_ifk_XSec_3Dinfp_nu_data_Slice0","T2K_CC0pinp_ifk_XSec_3Dinfp_nu_MC_Slice0"},
      {"T2K_CC0pinp_ifk_XSec_3Dinfp_nu_data_Slice1","T2K_CC0pinp_ifk_XSec_3Dinfp_nu_MC_Slice1"},
      {"T2K_CC0pinp_ifk_XSec_3Dinfp_nu_data_Slice2","T2K_CC0pinp_ifk_XSec_3Dinfp_nu_MC_Slice2"},
      {"T2K_CC0pinp_ifk_XSec_3Dinfp_nu_data_Slice3","T2K_CC0pinp_ifk_XSec_3Dinfp_nu_MC_Slice3"},
      {"T2K_CC0pinp_ifk_XSec_3Dinfp_nu_data_Slice4","T2K_CC0pinp_ifk_XSec_3Dinfp_nu_MC_Slice4"},
      {"T2K_CC0pinp_ifk_XSec_3Dinfp_nu_data_Slice5","T2K_CC0pinp_ifk_XSec_3Dinfp_nu_MC_Slice5"},
      {"T2K_CC0pinp_ifk_XSec_3Dinfp_nu_data_Slice6","T2K_CC0pinp_ifk_XSec_3Dinfp_nu_MC_Slice6"},
      // -- infa
      {"T2K_CC0pinp_ifk_XSec_3Dinfa_nu_data","T2K_CC0pinp_ifk_XSec_3Dinfa_nu_MC"},
      {"","T2K_CC0pinp_ifk_XSec_3Dinfa_nu_RESIDUAL"},
      {"","T2K_CC0pinp_ifk_XSec_3Dinfa_nu_Chi2NMinusOne"},
      {"T2K_CC0pinp_ifk_XSec_3Dinfa_nu_data_Slice0","T2K_CC0pinp_ifk_XSec_3Dinfa_nu_MC_Slice0"},
      {"T2K_CC0pinp_ifk_XSec_3Dinfa_nu_data_Slice1","T2K_CC0pinp_ifk_XSec_3Dinfa_nu_MC_Slice1"},
      {"T2K_CC0pinp_ifk_XSec_3Dinfa_nu_data_Slice2","T2K_CC0pinp_ifk_XSec_3Dinfa_nu_MC_Slice2"},
      {"T2K_CC0pinp_ifk_XSec_3Dinfa_nu_data_Slice3","T2K_CC0pinp_ifk_XSec_3Dinfa_nu_MC_Slice3"},
      {"T2K_CC0pinp_ifk_XSec_3Dinfa_nu_data_Slice4","T2K_CC0pinp_ifk_XSec_3Dinfa_nu_MC_Slice4"},
      {"T2K_CC0pinp_ifk_XSec_3Dinfa_nu_data_Slice5","T2K_CC0pinp_ifk_XSec_3Dinfa_nu_MC_Slice5"},
      {"T2K_CC0pinp_ifk_XSec_3Dinfa_nu_data_Slice6","T2K_CC0pinp_ifk_XSec_3Dinfa_nu_MC_Slice6"},
      // -- infip
      {"T2K_CC0pinp_ifk_XSec_3Dinfip_nu_data","T2K_CC0pinp_ifk_XSec_3Dinfip_nu_MC"},
      {"","T2K_CC0pinp_ifk_XSec_3Dinfip_nu_RESIDUAL"},
      {"","T2K_CC0pinp_ifk_XSec_3Dinfip_nu_Chi2NMinusOne"},
      {"T2K_CC0pinp_ifk_XSec_3Dinfip_nu_data_Slice0","T2K_CC0pinp_ifk_XSec_3Dinfip_nu_MC_Slice0"},
      {"T2K_CC0pinp_ifk_XSec_3Dinfip_nu_data_Slice1","T2K_CC0pinp_ifk_XSec_3Dinfip_nu_MC_Slice1"},
      {"T2K_CC0pinp_ifk_XSec_3Dinfip_nu_data_Slice2","T2K_CC0pinp_ifk_XSec_3Dinfip_nu_MC_Slice2"},
      {"T2K_CC0pinp_ifk_XSec_3Dinfip_nu_data_Slice3","T2K_CC0pinp_ifk_XSec_3Dinfip_nu_MC_Slice3"},
      {"T2K_CC0pinp_ifk_XSec_3Dinfip_nu_data_Slice4","T2K_CC0pinp_ifk_XSec_3Dinfip_nu_MC_Slice4"},
      {"T2K_CC0pinp_ifk_XSec_3Dinfip_nu_data_Slice5","T2K_CC0pinp_ifk_XSec_3Dinfip_nu_MC_Slice5"},
      {"T2K_CC0pinp_ifk_XSec_3Dinfip_nu_data_Slice6","T2K_CC0pinp_ifk_XSec_3Dinfip_nu_MC_Slice6"},
      // 2018 CC0pi with protons multi-dimensional
      {"T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p_Np_data","T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p_Np_MC"},    
      {"T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p_data","T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p_MC"},
      {"T2K_CC0piWithProtons_XSec_2018_multidif_0p_data","T2K_CC0piWithProtons_XSec_2018_multidif_0p_MC"},
      {"T2K_CC0piWithProtons_XSec_2018_multidif_1p_data","T2K_CC0piWithProtons_XSec_2018_multidif_1p_MC"},
      {"T2K_CC0pi0p_XSec_2018_MuonCosTheta_data","T2K_CC0pi0p_XSec_2018_MuonCosTheta_MC"}, 
      {"T2K_CC0pi1p_XSec_2018_MuonCosTheta_data","T2K_CC0pi1p_XSec_2018_MuonCosTheta_MC"},
      {"T2K_CC0pi0p_XSec_2018_Data_Slice0","T2K_CC0pi0p_XSec_2018_MC_Slice0"},  
      {"T2K_CC0pi0p_XSec_2018_Data_Slice1","T2K_CC0pi0p_XSec_2018_MC_Slice1"},
      {"T2K_CC0pi0p_XSec_2018_Data_Slice2","T2K_CC0pi0p_XSec_2018_MC_Slice2"},
      {"T2K_CC0pi0p_XSec_2018_Data_Slice3","T2K_CC0pi0p_XSec_2018_MC_Slice3"},
      {"T2K_CC0pi0p_XSec_2018_Data_Slice4","T2K_CC0pi0p_XSec_2018_MC_Slice4"},
      {"T2K_CC0pi0p_XSec_2018_Data_Slice5","T2K_CC0pi0p_XSec_2018_MC_Slice5"},
      {"T2K_CC0pi0p_XSec_2018_Data_Slice6","T2K_CC0pi0p_XSec_2018_MC_Slice6"},
      {"T2K_CC0pi0p_XSec_2018_Data_Slice7","T2K_CC0pi0p_XSec_2018_MC_Slice7"},
      {"T2K_CC0pi0p_XSec_2018_Data_Slice8","T2K_CC0pi0p_XSec_2018_MC_Slice8"},
      {"T2K_CC0pi0p_XSec_2018_Data_Slice9","T2K_CC0pi0p_XSec_2018_MC_Slice9"},
      {"T2K_CC0pi1p_XSec_2018_Data_MuonCosTh1DSlice0","T2K_CC0pi1p_XSec_2018_MC_MuonCosTh1DSlice0"},
      {"T2K_CC0pi1p_XSec_2018_Data_MuonCosTh1DSlice1","T2K_CC0pi1p_XSec_2018_MC_MuonCosTh1DSlice1"},
      {"T2K_CC0pi1p_XSec_2018_Data_MuonCosTh1DSlice2","T2K_CC0pi1p_XSec_2018_MC_MuonCosTh1DSlice2"},
      {"T2K_CC0pi1p_XSec_2018_Data_MuonCosTh1DSlice3","T2K_CC0pi1p_XSec_2018_MC_MuonCosTh1DSlice3"},
      {"T2K_CC0pi1p_XSec_2018_Data_MuCThSlice_1_PCthSlice_0","T2K_CC0pi1p_XSec_2018_MC_MuCThSlice_1_PCthSlice_0"}, 
      {"T2K_CC0pi1p_XSec_2018_Data_MuCThSlice_2_PCthSlice_0","T2K_CC0pi1p_XSec_2018_MC_MuCThSlice_2_PCthSlice_0"},
      {"T2K_CC0pi1p_XSec_2018_Data_MuCThSlice_2_PCthSlice_1","T2K_CC0pi1p_XSec_2018_MC_MuCThSlice_2_PCthSlice_1"},
      {"T2K_CC0pi1p_XSec_2018_Data_MuCThSlice_3_PCthSlice_0","T2K_CC0pi1p_XSec_2018_MC_MuCThSlice_3_PCthSlice_0"},
      //
      // MicroBooNE
      //
      {"MicroBooNE_CCInc_XSec_2DPcos_nu_DATA","MicroBooNE_CCInc_XSec_2DPcos_nu_MC"}
      ,{"nominal_throw/MicroBooNE_CCInc_XSec_2DPcos_nu_DATA","error_bands/MicroBooNE_CCInc_XSec_2DPcos_nu_MC"}
      ,{"MicroBooNE_CC1MuNp_XSec_1DPmu_nu_data","MicroBooNE_CC1MuNp_XSec_1DPmu_nu_MC"}
      ,{"MicroBooNE_CC1MuNp_XSec_1Dcosmu_nu_data","MicroBooNE_CC1MuNp_XSec_1Dcosmu_nu_MC"}
      ,{"MicroBooNE_CC1MuNp_XSec_1DPp_nu_data","MicroBooNE_CC1MuNp_XSec_1DPp_nu_MC"}
      ,{"MicroBooNE_CC1MuNp_XSec_1Dcosp_nu_data","MicroBooNE_CC1MuNp_XSec_1Dcosp_nu_MC"}
      ,{"MicroBooNE_CC1MuNp_XSec_1Dthetamup_nu_data","MicroBooNE_CC1MuNp_XSec_1Dthetamup_nu_MC"}
      ,{"MicroBooNE_CCQE_XSec_1DPmu_nu_FullPS_data","MicroBooNE_CCQE_XSec_1DPmu_nu_FullPS_MC"}
      ,{"MicroBooNE_CCQE_XSec_1Dcosmu_nu_FullPS_data","MicroBooNE_CCQE_XSec_1Dcosmu_nu_FullPS_MC"}
      ,{"MicroBooNE_CCQE_XSec_1DPp_nu_FullPS_data","MicroBooNE_CCQE_XSec_1DPp_nu_FullPS_MC"}
      ,{"MicroBooNE_CCQE_XSec_1Dcosp_nu_FullPS_data","MicroBooNE_CCQE_XSec_1Dcosp_nu_FullPS_MC"}
      ,{"MicroBooNE_CCQE_XSec_1DPmu_nu_PartPS_data","MicroBooNE_CCQE_XSec_1DPmu_nu_PartPS_MC"}
      ,{"MicroBooNE_CCQE_XSec_1Dcosmu_nu_PartPS_data","MicroBooNE_CCQE_XSec_1Dcosmu_nu_PartPS_MC"}
      ,{"MicroBooNE_CCQE_XSec_1DPp_nu_PartPS_data","MicroBooNE_CCQE_XSec_1DPp_nu_PartPS_MC"}
      ,{"MicroBooNE_CCQE_XSec_1Dcosp_nu_PartPS_data","MicroBooNE_CCQE_XSec_1Dcosp_nu_PartPS_MC"}
      ,{"MicroBooNE_CCInc_XSec_2DPcos_nu_DATA_SHAPE","MicroBooNE_CCInc_XSec_2DPcos_nu_MC_SHAPE"}
      ,{"MicroBooNE_CC1MuNp_XSec_1DPmu_nu_data_SHAPE","MicroBooNE_CC1MuNp_XSec_1DPmu_nu_MC_SHAPE"}
      ,{"MicroBooNE_CC1MuNp_XSec_1Dcosmu_nu_data_SHAPE","MicroBooNE_CC1MuNp_XSec_1Dcosmu_nu_MC_SHAPE"}
      ,{"MicroBooNE_CC1MuNp_XSec_1DPp_nu_data_SHAPE","MicroBooNE_CC1MuNp_XSec_1DPp_nu_MC_SHAPE"}
      ,{"MicroBooNE_CC1MuNp_XSec_1Dcosp_nu_data_SHAPE","MicroBooNE_CC1MuNp_XSec_1Dcosp_nu_MC_SHAPE"}
      ,{"MicroBooNE_CC1MuNp_XSec_1Dthetamup_nu_data_SHAPE","MicroBooNE_CC1MuNp_XSec_1Dthetamup_nu_MC_SHAPE"}
      ,{"MicroBooNE_CCQE_XSec_1DPmu_nu_FullPS_data_SHAPE","MicroBooNE_CCQE_XSec_1DPmu_nu_FullPS_MC_SHAPE"}
      ,{"MicroBooNE_CCQE_XSec_1Dcosmu_nu_FullPS_data_SHAPE","MicroBooNE_CCQE_XSec_1Dcosmu_nu_FullPS_MC_SHAPE"}
      ,{"MicroBooNE_CCQE_XSec_1DPp_nu_FullPS_data_SHAPE","MicroBooNE_CCQE_XSec_1DPp_nu_FullPS_MC_SHAPE"}
      ,{"MicroBooNE_CCQE_XSec_1Dcosp_nu_FullPS_data_SHAPE","MicroBooNE_CCQE_XSec_1Dcosp_nu_FullPS_MC_SHAPE"}
      ,{"MicroBooNE_CCQE_XSec_1DPmu_nu_PartPS_data_SHAPE","MicroBooNE_CCQE_XSec_1DPmu_nu_PartPS_MC_SHAPE"}
      ,{"MicroBooNE_CCQE_XSec_1Dcosmu_nu_PartPS_data_SHAPE","MicroBooNE_CCQE_XSec_1Dcosmu_nu_PartPS_MC_SHAPE"}
      ,{"MicroBooNE_CCQE_XSec_1DPp_nu_PartPS_data_SHAPE","MicroBooNE_CCQE_XSec_1DPp_nu_PartPS_MC_SHAPE"}
      ,{"MicroBooNE_CCQE_XSec_1Dcosp_nu_PartPS_data_SHAPE","MicroBooNE_CCQE_XSec_1Dcosp_nu_PartPS_MC_SHAPE"}
      //
      // MiniBooNE
      //
      ,{"MiniBooNE_CCQE_XSec_1DQ2_nu_data","MiniBooNE_CCQE_XSec_1DQ2_nu_MC"}
      ,{"MiniBooNE_CCQELike_XSec_1DQ2_nu","MiniBooNE_CCQELike_XSec_1DQ2_nu_MC"}
      ,{"MiniBooNE_CCQE_XSec_2DTcos_nu_data_1D","MiniBooNE_CCQE_XSec_2DTcos_nu_MC_1D"}
      ,{"MiniBooNE_CCQELike_XSec_2DTcos_nu_data_1D","MiniBooNE_CCQELike_XSec_2DTcos_nu_MC_1D"}
      ,{"MiniBooNE_CCQE_XSec_1DEnu_nu_data","MiniBooNE_CCQE_XSec_1DEnu_nu_MC"}
      ,{"MiniBooNE_CCQELike_XSec_1DEnu_nu_data","MiniBooNE_CCQELike_XSec_1DEnu_nu_MC"}
      ,{"MiniBooNE_CC1pip_XSec_1DEnu_nu_data","MiniBooNE_CC1pip_XSec_1DEnu_nu_MC"}
      ,{"MiniBooNE_CC1pip_XSec_1DQ2_nu_data","MiniBooNE_CC1pip_XSec_1DQ2_nu_MC"}
      ,{"MiniBooNE_CC1pip_XSec_1DTpi_nu_data","MiniBooNE_CC1pip_XSec_1DTpi_nu_MC"}
      ,{"MiniBooNE_CC1pip_XSec_1DTu_nu_data","MiniBooNE_CC1pip_XSec_1DTu_nu_MC"}
      ,{"MiniBooNE_CC1pip_XSec_2DQ2Enu_nu_data_1D","MiniBooNE_CC1pip_XSec_2DQ2Enu_nu_MC_1D"}
      ,{"MiniBooNE_CC1pip_XSec_2DTpiCospi_nu_data_1D","MiniBooNE_CC1pip_XSec_2DTpiCospi_nu_MC_1D"}
      ,{"MiniBooNE_CC1pip_XSec_2DTpiEnu_nu_data_1D","MiniBooNE_CC1pip_XSec_2DTpiEnu_nu_MC_1D"}
      ,{"MiniBooNE_CC1pip_XSec_2DTuCosmu_nu_data_1D","MiniBooNE_CC1pip_XSec_2DTuCosmu_nu_MC_1D"}
      ,{"MiniBooNE_CC1pip_XSec_2DTuEnu_nu_data_1D","MiniBooNE_CC1pip_XSec_2DTuEnu_nu_MC_1D"}
      ,{"MiniBooNE_CC1pi0_XSec_1DEnu_nu_data","MiniBooNE_CC1pi0_XSec_1DEnu_nu_MC"}
      ,{"MiniBooNE_CC1pi0_XSec_1DQ2_nu_data","MiniBooNE_CC1pi0_XSec_1DQ2_nu_MC"}
      ,{"MiniBooNE_CC1pi0_XSec_1DTu_nu_data","MiniBooNE_CC1pi0_XSec_1DTu_nu_MC"}
      ,{"MiniBooNE_CC1pi0_XSec_1Dcosmu_nu_data","MiniBooNE_CC1pi0_XSec_1Dcosmu_nu_MC"}
      ,{"MiniBooNE_CC1pi0_XSec_1Dcospi0_nu_data","MiniBooNE_CC1pi0_XSec_1Dcospi0_nu_MC"}
      ,{"MiniBooNE_CC1pi0_XSec_1Dppi0_nu_data","MiniBooNE_CC1pi0_XSec_1Dppi0_nu_MC"}
      ,{"MiniBooNE_NC1pi0_XSec_1Dcospi0_nu_data","MiniBooNE_NC1pi0_XSec_1Dcospi0_nu_MC"}
      ,{"MiniBooNE_NC1pi0_XSec_1Dppi0_nu_data","MiniBooNE_NC1pi0_XSec_1Dppi0_nu_MC"}
      //
      // MINERvA
      //
      // CC0pinp STV
      ,{"MINERvA_CC0pinp_STV_XSec_1Dpmu_nu_data","MINERvA_CC0pinp_STV_XSec_1Dpmu_nu_MC"}
      ,{"MINERvA_CC0pinp_STV_XSec_1Dthmu_nu_data","MINERvA_CC0pinp_STV_XSec_1Dthmu_nu_MC"}
      ,{"MINERvA_CC0pinp_STV_XSec_1Dpprot_nu_data","MINERvA_CC0pinp_STV_XSec_1Dpprot_nu_MC"}
      ,{"MINERvA_CC0pinp_STV_XSec_1Dthprot_nu_data","MINERvA_CC0pinp_STV_XSec_1Dthprot_nu_MC"}
      ,{"MINERvA_CC0pinp_STV_XSec_1Dpnreco_nu_data","MINERvA_CC0pinp_STV_XSec_1Dpnreco_nu_MC"}
      ,{"MINERvA_CC0pinp_STV_XSec_1Ddalphat_nu_data","MINERvA_CC0pinp_STV_XSec_1Ddalphat_nu_MC"}
      ,{"MINERvA_CC0pinp_STV_XSec_1Ddpt_nu_data","MINERvA_CC0pinp_STV_XSec_1Ddpt_nu_MC"}
      ,{"MINERvA_CC0pinp_STV_XSec_1Ddphit_nu_data","MINERvA_CC0pinp_STV_XSec_1Ddphit_nu_MC"}
      // CC0pi
      ,{"MINERvA_CC0pi_XSec_1DQ2_nu_proton_data","MINERvA_CC0pi_XSec_1DQ2_nu_proton_MC"}
      ,{"MINERvA_CC0pi_XSec_1DQ2_TgtCH_nu_data","MINERvA_CC0pi_XSec_1DQ2_TgtCH_nu_MC"}
      ,{"MINERvA_CC0pi_XSec_2Dptpz_nu_data_1D","MINERvA_CC0pi_XSec_2Dptpz_nu_MC_1D"}
      // CC1pi+
      ,{"MINERvA_CC1pip_XSec_1DTpi_nu_2017_data","MINERvA_CC1pip_XSec_1DTpi_nu_2017_MC"}
      ,{"MINERvA_CC1pip_XSec_1Dth_nu_2017_data","MINERvA_CC1pip_XSec_1Dth_nu_2017_MC"}
      ,{"MINERvA_CC1pip_XSec_1Dpmu_nu_2017_data","MINERvA_CC1pip_XSec_1Dpmu_nu_2017_MC"}
      ,{"MINERvA_CC1pip_XSec_1Dthmu_nu_2017_data","MINERvA_CC1pip_XSec_1Dthmu_nu_2017_MC"}
      ,{"MINERvA_CC1pip_XSec_1DQ2_nu_2017_data","MINERvA_CC1pip_XSec_1DQ2_nu_2017_MC"}
      ,{"MINERvA_CC1pip_XSec_1DEnu_nu_2017_data","MINERvA_CC1pip_XSec_1DEnu_nu_2017_MC"}
      // CCpi0
      ,{"MINERvA_CC1pi0_XSec_1DTpi_nu_data","MINERvA_CC1pi0_XSec_1DTpi_nu_MC"}
      ,{"MINERvA_CC1pi0_XSec_1Dth_nu_data","MINERvA_CC1pi0_XSec_1Dth_nu_MC"}
      ,{"MINERvA_CC1pi0_XSec_1Dpmu_nu_data","MINERvA_CC1pi0_XSec_1Dpmu_nu_MC"}
      ,{"MINERvA_CC1pi0_XSec_1Dthmu_nu_data","MINERvA_CC1pi0_XSec_1Dthmu_nu_MC"}
      ,{"MINERvA_CC1pi0_XSec_1DQ2_nu_data","MINERvA_CC1pi0_XSec_1DQ2_nu_MC"}
      ,{"MINERvA_CC1pi0_XSec_1DEnu_nu_data","MINERvA_CC1pi0_XSec_1DEnu_nu_MC"}
      ,{"MINERvA_CC1pi0_XSec_1DWexp_nu_data","MINERvA_CC1pi0_XSec_1DWexp_nu_MC"}
      ,{"MINERvA_CC1pi0_XSec_1DPPi0Mass_nu_data","MINERvA_CC1pi0_XSec_1DPPi0Mass_nu_MC"}
      ,{"MINERvA_CC1pi0_XSec_1DPPi0MassDelta_nu_data","MINERvA_CC1pi0_XSec_1DPPi0MassDelta_nu_MC"}
      ,{"MINERvA_CC1pi0_XSec_1DCosAdler_nu_data","MINERvA_CC1pi0_XSec_1DCosAdler_nu_MC"}
      ,{"MINERvA_CC1pi0_XSec_1DPhiAdler_nu_data","MINERvA_CC1pi0_XSec_1DPhiAdler_nu_MC"}
      ,{"MINERvA_CCinc_XSec_2DEavq3_nu_data_1D","MINERvA_CCinc_XSec_2DEavq3_nu_MC_1D"}
      //
      // SciBooNE
      //
      ,{"SciBooNE_CCInc_XSec_1DEnu_nu_data","SciBooNE_CCInc_XSec_1DEnu_nu_MC"}
      ,{"SciBooNE_CCInc_XSec_1DEnu_nu_NEUT_data","SciBooNE_CCInc_XSec_1DEnu_nu_NEUT_MC"}
      ,{"SciBooNE_CCInc_XSec_1DEnu_nu_NUANCE_data","SciBooNE_CCInc_XSec_1DEnu_nu_NUANCE_MC"}

    };

  gStyle->SetOptStat(0);

  TCanvas *c1 = new TCanvas();
  std::string labelSample;
  for (int i=0; i<Names_1D.size(); i++){
    c1->Clear();
    c1->SetLeftMargin(0.11);
    c1->SetBottomMargin(0.15);

    TH1D *data=NULL;
    std::vector<TH1D*> mc;
    for (int i_file=0; i_file<infile_names.size(); i_file++){
      TFile *file_tmp = new TFile(infile_names.at(i_file).c_str(),"open");
      labelSample=legtitles.at(i_file);
      if (i_file==0) data = (TH1D*)file_tmp->Get(Names_1D.at(i).first.c_str());

      TH1D *mc_tmp = (TH1D*)file_tmp->Get(Names_1D.at(i).second.c_str());
      if (mc_tmp) mc_tmp->SetName(Form("mc_%i",i+1));
      if (mc_tmp) mc.push_back(mc_tmp);
    }

    // Set data histogram name so we can look at it in Draw1DComparison (especially to distinguish the uB error bands plot)
    if (data){
      std::string dataname = Names_1D.at(i).first;
      if (dataname.find("/")!=std::string::npos) dataname.replace(dataname.find("/"),1,"_");
      data->SetName(dataname.c_str());
      std::cout << data->GetName() << std::endl;
    }

    if (mc.size()>0){
      Draw1DComparison(data,mc,legtitles,c1);
      
      //if (fullFit==1) labelSample="FullTune";
      //if (fullFit==2) labelSample="CombinedFits";
      //if (fullFit==3) labelSample="3parFits";
      //if (fullFit==4) labelSample="4parFits";
      //if (fullFit==5) labelSample="5parFits";
      std::string savename = Names_1D.at(i).second +"_Comparison.pdf";
      std::string savename2=Names_1D.at(i).second+"_Comparison.png";  
    // Check if name contains a / and remove it because we can''t save that as a pdf'
      if (savename.find("/")!=std::string::npos) savename.replace(savename.find("/"),1,"_");
      c1->Print(savename.c_str());
      c1->Print(savename2.c_str());
      // Also save as .root (comment out by default because this makes a lot of root files)
      /*      savename = Names_1D.at(i).second + ".root";
      // Check if name contains a / and remove it because we can''t save that as a pdf'                                                           
      if (savename.find("/")!=std::string::npos) savename.replace(savename.find("/"),1,"_");
      c1->Print(savename.c_str());*/
    }
  }
};
