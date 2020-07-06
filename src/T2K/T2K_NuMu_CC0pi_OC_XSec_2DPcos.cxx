// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NUISANCE.
*
*    NUISANCE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NUISANCE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NUISANCE.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include "T2K_NuMu_CC0pi_OC_XSec_2DPcos.h"

static size_t nangbins = 6;
static double angular_binning_costheta[] = {-1, 0., 0.6, 0.75, 0.86, 0.93, 1.};

//********************************************************************
T2K_NuMu_CC0pi_OC_XSec_2DPcos::T2K_NuMu_CC0pi_OC_XSec_2DPcos(nuiskey samplekey) {
//********************************************************************

  // Samples overview ---------------------------------------------------
  fSettings = LoadSampleSettings(samplekey);
  std::string name = fSettings.GetS("name");
  std::string descrip = "";

  // This has to deal with NuMu FHC, and AntiNuMu RHC
  if (!name.compare("T2K_NuMu_CC0pi_O_XSec_2DPcos")){
    descrip = name +". \n"
                    "Target: Oxygen \n"
                    "Flux: T2K 2.5 degree off-axis (ND280)  \n"
                    "Signal: CC0pi\n"
                    "arXiv:2004.05434";
    fSettings.SetTitle("T2K_NuMu_CC0pi_O_XSec_2DPcos");
    fSettings.DefineAllowedTargets("O");
    Target = "O";
  } 
  else if (!name.compare("T2K_NuMu_CC0pi_C_XSec_2DPcos")){
    descrip = name +". \n"
                    "Target: Carbon \n"
                    "Flux: T2K 2.5 degree off-axis (ND280)  \n"
                    "Signal: CC0pi\n"
                    "arXiv:2004.05434";
    fSettings.SetTitle("T2K_NuMu_CC0pi_C_XSec_2DPcos");
    fSettings.DefineAllowedTargets("C");
    Target = "C";
  }
  // Setup common settings
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("p_{#mu}-cos#theta_{#mu}");
  fSettings.SetYTitle("d^{2}#sigma/dP_{#mu}dcos#theta_{#mu} (cm^{2}/GeV)");
  fSettings.SetAllowedTypes("DIAG,FULL/FREE,SHAPE,FIX/SYSTCOV/STATCOV","FIX");
  fSettings.SetEnuRangeFromFlux(fFluxHist);
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = ((GetEventHistogram()->Integral("width")/(fNEvents+0.)) * 1E-38 / (TotalIntegratedFlux()));

  // Setup Histograms
  SetHistograms();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};


bool T2K_NuMu_CC0pi_OC_XSec_2DPcos::isSignal(FitEvent *event){
  return SignalDef::isCC0pi(event, 14, EnuMin, EnuMax);
};

void T2K_NuMu_CC0pi_OC_XSec_2DPcos::FillEventVariables(FitEvent* event){

  if (event->NumFSParticle(13) == 0)
    return;
  
  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;

  double pmu = Pmu.Vect().Mag()/1000.;
  double CosThetaMu = cos(Pnu.Vect().Angle(Pmu.Vect()));

  fXVar = pmu;
  fYVar = CosThetaMu;
  return;
};

void T2K_NuMu_CC0pi_OC_XSec_2DPcos::FillHistograms(){

  Measurement1D::FillHistograms();
  if (Signal){
    FillMCSlice( fXVar, fYVar, Weight );
  }

}

void T2K_NuMu_CC0pi_OC_XSec_2DPcos::ConvertEventRates(){

  for (int i = 0; i < nangbins; i++){
    if(Target=="O") fMCHistNuMuO_Slices[i]->GetSumw2();
    else if(Target=="C") fMCHistNuMuC_Slices[i]->GetSumw2();
  }

  // Do standard conversion.
  Measurement1D::ConvertEventRates();

  // Scale MC slices by their bin area
  for (size_t i = 0; i < nangbins; ++i) {
    if(Target=="O") fMCHistNuMuO_Slices[i]->Scale(1. / (angular_binning_costheta[i + 1] - angular_binning_costheta[i]));
    else if(Target=="C") fMCHistNuMuC_Slices[i]->Scale(1. / (angular_binning_costheta[i + 1] - angular_binning_costheta[i]));
  }

  // Now Convert into 1D lists
  fMCHist->Reset();
  int bincount = 0;
  for (int i = 0; i < nangbins; i++){
    if(Target=="O"){
      for (int j = 0; j < fMCHistNuMuO_Slices[i]->GetNbinsX(); j++){
        fMCHist->SetBinContent(bincount+1, fMCHistNuMuO_Slices[i]->GetBinContent(j+1));
        bincount++;
      }
    }
    else if(Target=="C"){
      for (int j = 0; j < fMCHistNuMuC_Slices[i]->GetNbinsX(); j++){
        fMCHist->SetBinContent(bincount+1, fMCHistNuMuC_Slices[i]->GetBinContent(j+1));
        bincount++;
      }
    }
  } 

  return;
}

void T2K_NuMu_CC0pi_OC_XSec_2DPcos::FillMCSlice(double x, double y, double w){

  for (size_t i = 0; i < nangbins; ++i) {
    if ((y > angular_binning_costheta[i]) && (y <= angular_binning_costheta[i + 1])) {
      if(Target=="O") fMCHistNuMuC_Slices[i]->Fill(x, w);
      else if(Target=="C") fMCHistNuMuC_Slices[i]->Fill(x, w);
    }
  }
}

void T2K_NuMu_CC0pi_OC_XSec_2DPcos::SetHistograms(){

  // Read covariance matrix
  fInputFileCov = new TFile( (FitPar::GetDataBase() + "/T2K/CC0pi/JointO-C/covmatrix_noreg.root").c_str(),"READ");

  TH1D* hLinearResult;
  int Nbins;

  if(Target=="O"){
    // Read in 1D Data Histograms
    fInputFile = new TFile( (FitPar::GetDataBase() + "/T2K/CC0pi/JointO-C/linear_unreg_results_O_nuisance.root").c_str(),"READ");
    hLinearResult = (TH1D*) fInputFile->Get("LinResult");
    
    fFullCovar = (TMatrixDSym*) fInputFileCov->Get("covmatrixObin");
    covar = StatUtils::GetInvert(fFullCovar);
    fDecomp = StatUtils::GetDecomp(fFullCovar);


    Nbins = hLinearResult->GetNbinsX();
    // Now Convert into 1D list
    fDataHist = new TH1D("LinarResultOxygen","LinarResultOxygen",Nbins,0,Nbins);
    for (int bin = 0; bin < Nbins; bin++){
      fDataHist->SetBinContent(bin+1, hLinearResult->GetBinContent(bin+1));
    }

    fDataHist->Reset();
    int bincount = 0;
    for (int i = 0; i < nangbins; i++){
      // Get Data Histogram
      fDataHistNuMuO_Slices.push_back((TH1D*) fInputFile->Get(Form("dataslice_%i",i))->Clone());
      fDataHistNuMuO_Slices[i]->SetNameTitle(Form("T2K_NuMu_CC0pi_O_2DPcos_data_Slice%i",i),
      (Form("T2K_NuMu_CC0pi_O_2DPcos_data_Slice%i",i)));

      // Loop over nbins and set errors from covar
      for (int j = 0; j < fDataHistNuMuO_Slices[i]->GetNbinsX(); j++){
        fDataHistNuMuO_Slices[i]->SetBinError(j+1, sqrt((*fFullCovar)(bincount,bincount)));
        fDataHist->SetBinContent(bincount+1, fDataHistNuMuO_Slices[i]->GetBinContent(j+1));
        fDataHist->SetBinError(bincount+1,   fDataHistNuMuO_Slices[i]->GetBinError(j+1));
        bincount++;
      }

      // Make MC Clones
      fMCHistNuMuO_Slices.push_back((TH1D*) fDataHistNuMuO_Slices[i]->Clone());
      fMCHistNuMuO_Slices[i]->SetNameTitle(Form("T2K_NuMu_CC0pi_O_2DPcos_MC_Slice%i",i), (Form("T2K_NuMu_CC0pi_O_2DPcos_MC_Slice%i",i)));

      SetAutoProcessTH1(fDataHistNuMuO_Slices[i],kCMD_Write);
      SetAutoProcessTH1(fMCHistNuMuO_Slices[i]);
    }
  } 
  else if(Target=="C"){
    
    fInputFile = new TFile( (FitPar::GetDataBase() + "/T2K/CC0pi/JointO-C/linear_unreg_results_C_nuisance.root").c_str(),"READ");
    hLinearResult = (TH1D*) fInputFile->Get("LinResult");

    fFullCovar = (TMatrixDSym*) fInputFileCov->Get("covmatrixCbin");
    covar = StatUtils::GetInvert(fFullCovar);
    fDecomp = StatUtils::GetDecomp(fFullCovar);

    Nbins = hLinearResult->GetNbinsX();

    // Now Convert into 1D list
    fDataHist = new TH1D("LinarResultCarbon","LinarResultCarbon",Nbins,0,Nbins);
    for (int bin = 0; bin < Nbins; bin++){
      fDataHist->SetBinContent(bin+1, hLinearResult->GetBinContent(bin+1));
    }

    fDataHist->Reset();

    int bincount=0;
    for (int i = 0; i < nangbins; i++){
      //Get Data Histogram
      fDataHistNuMuC_Slices.push_back((TH1D*) fInputFile->Get(Form("dataslice_%i",i))->Clone());
      fDataHistNuMuC_Slices[i]->SetNameTitle(Form("T2K_NuMu_CC0pi_C_2DPcos_data_Slice%i",i),
      (Form("T2K_NuMu_CC0pi_C_2DPcos_data_Slice%i",i)));

      //Loop over nbins and set errors from covar
      for (int j = 0; j < fDataHistNuMuC_Slices[i]->GetNbinsX(); j++){
        fDataHistNuMuC_Slices[i]->SetBinError(j+1, sqrt((*fFullCovar)(bincount,bincount)));
        fDataHist->SetBinContent(bincount+1, fDataHistNuMuC_Slices[i]->GetBinContent(j+1));
        fDataHist->SetBinError(bincount+1,   fDataHistNuMuC_Slices[i]->GetBinError(j+1));
        bincount++;
      }

      //Save MC slices
      fMCHistNuMuC_Slices.push_back((TH1D*) fDataHistNuMuC_Slices[i]->Clone());
      fMCHistNuMuC_Slices[i]->SetNameTitle(Form("T2K_NuMu_CC0pi_C_2DPcos_MC_Slice%i",i), (Form("T2K_NuMu_CC0pi_C_2DPcos_MC_Slice%i",i)));

      SetAutoProcessTH1(fDataHistNuMuC_Slices[i],kCMD_Write);
      SetAutoProcessTH1(fMCHistNuMuC_Slices[i]);

    }
  } 

  return;

};


