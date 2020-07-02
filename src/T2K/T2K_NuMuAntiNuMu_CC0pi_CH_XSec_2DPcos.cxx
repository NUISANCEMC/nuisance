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

#include "T2K_SignalDef.h"

#include "T2K_NuMuAntiNuMu_CC0pi_CH_XSec_2DPcos.h"

static size_t nangbins = 9;
static double angular_binning_costheta[] = {-1,    0.2, 0.6,  0.7,  0.8,
                                             0.85, 0.9, 0.94, 0.98, 1   };

//********************************************************************
T2K_NuMuAntiNuMu_CC0pi_CH_XSec_2DPcos::T2K_NuMuAntiNuMu_CC0pi_CH_XSec_2DPcos(nuiskey samplekey) {
//********************************************************************

  // Samples overview ---------------------------------------------------
  fSettings = LoadSampleSettings(samplekey);
  std::string name = fSettings.GetS("name");
  std::string descrip = "";

  // This has to deal with NuMu FHC, and AntiNuMu RHC
  if (!name.compare("T2K_NuMu_CC0pi_CH_XSec_2DPcos")){
    descrip = name +". \n"
                    "Target: CH \n"
                    "Flux: T2K 2.5 degree off-axis (ND280)  \n"
                    "Signal: CC0pi\n"
                    "arXiv:2002.09323";
    fSettings.SetTitle("T2K_NuMu_CC0pi_CH_XSec_2DPcos");
    fSettings.DefineAllowedSpecies("numu");
    NuPDG  = 14;
    LepPDG = 13;
  } 
  else if (!name.compare("T2K_AntiNuMu_CC0pi_CH_XSec_2DPcos")){
    descrip = name +". \n"
                    "Target: CH \n"
                    "Flux: T2K 2.5 degree off-axis (ND280)  \n"
                    "Signal: CC0pi\n"
                    "arXiv:2002.09323";

    fSettings.SetTitle("T2K_AntiNuMu_CC0pi_CH_XSec_2DPcos");
    fSettings.DefineAllowedSpecies("numub");
    NuPDG  = -14;
    LepPDG = -13;
  }
  // Setup common settings
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("P_{#mu} (GeV)");
  fSettings.SetYTitle("cos#theta_{#mu}");
  fSettings.SetZTitle("d^{2}#sigma/dP_{#mu}dcos#theta_{#mu} (cm^{2}/GeV)");
  fSettings.SetAllowedTypes("DIAG,FULL/FREE,SHAPE,FIX/SYSTCOV/STATCOV","FIX");
  fSettings.SetEnuRange(0.0, 30.0);
  fSettings.DefineAllowedTargets("C,H");
  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = ((GetEventHistogram()->Integral("width")/(fNEvents+0.)) * 1E-38 / (TotalIntegratedFlux()));

  // Setup Histograms
  SetHistograms();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};


bool T2K_NuMuAntiNuMu_CC0pi_CH_XSec_2DPcos::isSignal(FitEvent *event){
  if (!SignalDef::isCC0pi(event, NuPDG, EnuMin, EnuMax)) return false;
};

void T2K_NuMuAntiNuMu_CC0pi_CH_XSec_2DPcos::FillEventVariables(FitEvent* event){

  if (event->NumFSParticle(LepPDG) == 0)
    return;
  
  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(LepPDG)->fP;

  double pmu = Pmu.Vect().Mag()/1000.;
  double CosThetaMu = cos(Pnu.Vect().Angle(Pmu.Vect()));

  fXVar = pmu;
  fYVar = CosThetaMu;
  return;
};

void T2K_NuMuAntiNuMu_CC0pi_CH_XSec_2DPcos::FillHistograms(){

  Measurement1D::FillHistograms();
  if (Signal){
    FillMCSlice( fXVar, fYVar, Weight );
  }

}

void T2K_NuMuAntiNuMu_CC0pi_CH_XSec_2DPcos::ConvertEventRates(){

  for (int i = 0; i < 9; i++){
    if(NuPDG==14) fMCNuMuHist_Slices[i]->GetSumw2();
    else if(NuPDG==-14) fMCAntiNuMuHist_Slices[i]->GetSumw2();
  }

  // Do standard conversion.
  Measurement1D::ConvertEventRates();

  // Scale MC slices by their bin area
  for (size_t i = 0; i < nangbins; ++i) {
    if(NuPDG==14) fMCNuMuHist_Slices[i]->Scale(fScaleFactor / (angular_binning_costheta[i + 1] - angular_binning_costheta[i]));
    else if(NuPDG==-14) fMCAntiNuMuHist_Slices[i]->Scale(fScaleFactor / (angular_binning_costheta[i + 1] - angular_binning_costheta[i]));
  }

  // Now Convert into 1D lists
  fMCHist->Reset();
  int bincount = 0;
  for (int i = 0; i < nangbins; i++){
    if(NuPDG==14){
      for (int j = 0; j < fDataNuMuHist_Slices[i]->GetNbinsX(); j++){
        fMCHist->SetBinContent(bincount+1, fMCNuMuHist_Slices[i]->GetBinContent(j+1));
        std::cout<< "fMCNuMuHist_Slices[i]->GetBinContent(j+1) " << fMCNuMuHist_Slices[i]->GetBinContent(j+1) <<std::endl;
        bincount++;
      }
    }
    else if(NuPDG==-14){
      for (int j = 0; j < fMCAntiNuMuHist_Slices[i]->GetNbinsX(); j++){
        fMCHist->SetBinContent(bincount+1, fMCAntiNuMuHist_Slices[i]->GetBinContent(j+1));
        bincount++;
      }
    }
  }

  return;
}

void T2K_NuMuAntiNuMu_CC0pi_CH_XSec_2DPcos::FillMCSlice(double x, double y, double w){

  for (size_t i = 0; i < nangbins; ++i) {
    if ((y >= angular_binning_costheta[i]) && (y < angular_binning_costheta[i + 1])) {
      if(NuPDG==14) fMCNuMuHist_Slices[i]->Fill(x, w);
      else if(NuPDG==-14) fMCAntiNuMuHist_Slices[i]->Fill(x, w);
      std::cout<< " w " << w <<cout::endl;
    }
  }
}

void T2K_NuMuAntiNuMu_CC0pi_CH_XSec_2DPcos::SetHistograms(){

    // Read in 1D Data Histograms
  fInputFile = new TFile( (FitPar::GetDataBase() + "/T2K/CC0pi/JointNuMu-AntiNuMu/JointNuMuAntiNuMuCC0piXsecDataRelease.root").c_str(),"READ");

  TH1D* hLinearResult;

  if(NuPDG==14) hLinearResult = (TH1D*) fInputFile->Get("hNuMuCC0piXsecLinearResult");
  else if(NuPDG==-14) hLinearResult = (TH1D*) fInputFile->Get("hAntiNuMuCC0piXsecLinearResult");

  int Nbins = hLinearResult->GetNbinsX();

  // Now Convert into 1D list
  fDataHist = new TH1D("LinarResult","LinarResult",Nbins,0,Nbins);
  for (int bin = 0; bin < Nbins; bin++){
    fDataHist->SetBinContent(bin+1, hLinearResult->GetBinContent(bin+1));
  }
  
  fFullCovar = new TMatrixDSym(Nbins);

  TMatrixDSym* tmpcovstat = (TMatrixDSym*) fInputFile->Get("JointNuMuAntiNuMuCC0piXsecCovMatrixStat");
  TMatrixDSym* tmpcovsyst = (TMatrixDSym*) fInputFile->Get("JointNuMuAntiNuMuCC0piXsecCovMatrixSyst");

  for(int ibin=0; ibin<Nbins; ibin++){  
    for(int jbin=0; jbin<Nbins; jbin++){
      if(NuPDG==14) (*fFullCovar)(ibin,jbin) = ((*tmpcovstat)(ibin,jbin) + (*tmpcovsyst)(ibin,jbin))*1E38*1E38;
      else if(NuPDG==-14) (*fFullCovar)(ibin,jbin) = ((*tmpcovstat)(ibin+Nbins,jbin+Nbins) + (*tmpcovsyst)(ibin+Nbins,jbin+Nbins))*1E38*1E38;
    }
  }
  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);
  
  fDataHist->Reset();

  // Read in 1D Data Slices and Make MC Slices NuMu CC0pi Xsec
  int bincount = 0;
  for (int i = 0; i < nangbins; i++){
    if(NuPDG==14){
      // Get Data Histogram
      fDataNuMuHist_Slices.push_back((TH1D*)fInputFile->Get(Form("hXsecNuMuCC0piDataSlice_%i",i))->Clone());
      fDataNuMuHist_Slices[i]->SetNameTitle(Form("T2K_NuMu_CC0pi_2DPcos_data_Slice%i",i),
      (Form("T2K_NuMu_CC0pi_2DPcos_data_Slice%i",i)));

      // Loop over nbins and set errors from covar
      for (int j = 0; j < fDataNuMuHist_Slices[i]->GetNbinsX(); j++){
        fDataNuMuHist_Slices[i]->SetBinError(j+1, sqrt((*fFullCovar)(bincount,bincount))*1E-38);
        fDataHist->SetBinContent(bincount+1, fDataNuMuHist_Slices[i]->GetBinContent(j+1));
        fDataHist->SetBinError(bincount+1,   fDataNuMuHist_Slices[i]->GetBinError(j+1));
        bincount++;
      }

      // Make MC Clones
      fMCNuMuHist_Slices.push_back((TH1D*) fDataNuMuHist_Slices[i]->Clone());
      fMCNuMuHist_Slices[i]->SetNameTitle(Form("T2K_NuMu_CC0pi_2DPcos_MC_Slice%i",i),
        (Form("T2K_NuMu_CC0pi_2DPcos_MC_Slice%i",i)));

      SetAutoProcessTH1(fDataNuMuHist_Slices[i],kCMD_Write);
      SetAutoProcessTH1(fMCNuMuHist_Slices[i]);

    } 
    else if(NuPDG==-14) {
      //Get Data Histogram
      fDataAntiNuMuHist_Slices.push_back((TH1D*)fInputFile->Get(Form("hXsecAntiNuMuCC0piDataSlice_%i",i))->Clone());
      fDataAntiNuMuHist_Slices[i]->SetNameTitle(Form("T2K_AntiNuMu_CC0pi_2DPcos_data_Slice%i",i),
      (Form("T2K_AntiNuMu_CC0pi_2DPcos_data_Slice%i",i)));

      //Loop over nbins and set errors from covar
      for (int j = 0; j < fDataAntiNuMuHist_Slices[i]->GetNbinsX(); j++){
        fDataAntiNuMuHist_Slices[i]->SetBinError(j+1, sqrt((*fFullCovar)(bincount,bincount))*1E-38);
        fDataHist->SetBinContent(bincount+1, fDataAntiNuMuHist_Slices[i]->GetBinContent(j+1));
        fDataHist->SetBinError(bincount+1,   fDataAntiNuMuHist_Slices[i]->GetBinError(j+1));
        bincount++;
      }

      //Make MC Clones
      fMCAntiNuMuHist_Slices.push_back((TH1D*) fDataAntiNuMuHist_Slices[i]->Clone());
      fMCAntiNuMuHist_Slices[i]->SetNameTitle(Form("T2K_AntiNuMu_CC0pi_2DPcos_MC_Slice%i",i), 
        (Form("T2K_AntiNuMu_CC0pi_2DPcos_MC_Slice%i",i)));

      SetAutoProcessTH1(fDataAntiNuMuHist_Slices[i],kCMD_Write);
      SetAutoProcessTH1(fMCAntiNuMuHist_Slices[i]);

    }
  }

  return;

};


