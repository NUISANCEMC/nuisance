// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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
  std::string name = fSettings.GetName();
  std::string descrip = "";

  // This has to deal with NuMu FHC, and AntiNuMu RHC
  if (!name.compare("T2K_NuMu_CC0pi_CH_XSec_2DPcos")){
    descrip = name +". \n"
                    "Target: CH \n"
                    "Flux: T2K 2.5 degree off-axis (ND280)  \n"
                    "Signal: CC0pi\n"
      "DOI:10.1103/PhysRevD.101.112001";
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
      "DOI:10.1103/PhysRevD.101.112001";
    fSettings.SetTitle("T2K_AntiNuMu_CC0pi_CH_XSec_2DPcos");
    fSettings.DefineAllowedSpecies("numub");
    NuPDG  = -14;
    LepPDG = -13;
  }
  // Setup common settings
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("p_{#mu}-cos#theta_{#mu}");
  fSettings.SetYTitle("d^{2}#sigma/dp_{#mu}dcos#theta_{#mu} (cm^{2}/GeV)");
  fSettings.SetAllowedTypes("DIAG,FULL/FREE,SHAPE,FIX/SYSTCOV/STATCOV","FIX");
  fSettings.SetEnuRangeFromFlux(fFluxHist);
  fSettings.DefineAllowedTargets("C,H");
  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = ((GetEventHistogram()->Integral("width")/(fNEvents+0.)) * 1E-38 / (TotalIntegratedFlux()));

  // Setup Histograms
  SetHistograms();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
  //fSaveFine = false;
};


bool T2K_NuMuAntiNuMu_CC0pi_CH_XSec_2DPcos::isSignal(FitEvent *event){
  return SignalDef::isCC0pi(event, NuPDG, EnuMin, EnuMax);
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

  for (size_t i = 0; i < nangbins; i++){
    fMCHist_Slices[i]->GetSumw2();
  }

  // Do standard conversion.
  Measurement1D::ConvertEventRates();

  // Scale MC slices by their bin area
  for (size_t i = 0; i < nangbins; ++i) {
    fMCHist_Slices[i]->Scale(1. / (angular_binning_costheta[i + 1] - angular_binning_costheta[i]));
  }

  // Now Convert into 1D lists
  fMCHist->Reset();
  int bincount = 0;
  for (size_t i = 0; i < nangbins; i++){
    for (int j = 0; j < fDataHist_Slices[i]->GetNbinsX(); j++){
      fMCHist->SetBinContent(bincount+1, fMCHist_Slices[i]->GetBinContent(j+1));
      bincount++;
    }
  }

  return;
}

void T2K_NuMuAntiNuMu_CC0pi_CH_XSec_2DPcos::FillMCSlice(double x, double y, double w){

  for (size_t i = 0; i < nangbins; ++i) {
    if ((y > angular_binning_costheta[i]) && (y <= angular_binning_costheta[i + 1])) {
      fMCHist_Slices[i]->Fill(x, w);
    }
  }
}

void T2K_NuMuAntiNuMu_CC0pi_CH_XSec_2DPcos::SetHistograms(){

  std::string name = fSettings.GetName();
  std::string titles = fSettings.GetFullTitles();

  std::string nuType;
  if (NuPDG==14) nuType = "NuMu";
  else nuType = "AntiNuMu";

  // Read in 1D Data Histograms
  fInputFile = new TFile( (FitPar::GetDataBase() + "/T2K/CC0pi/JointNuMuAntiNuMuCC0piXsecDataRelease.root").c_str(),"READ");
  
  TH1D* hLinearResult = (TH1D*) fInputFile->Get(Form("h%sCC0piXsecLinearResult", nuType.c_str()));
  int Nbins = hLinearResult->GetNbinsX();
  
  // Now Convert into 1D list
  fDataHist = new TH1D(Form("%s_data", name.c_str()),
		       Form("%s_data%s", name.c_str(), titles.c_str()),
		       Nbins,0,Nbins);
  for (int bin = 0; bin < Nbins; bin++){
    fDataHist->SetBinContent(bin+1, hLinearResult->GetBinContent(bin+1));
  }
  
  // Make covariance matrix
  fFullCovar = new TMatrixDSym(Nbins);

  TMatrixDSym* tmpcovstat = (TMatrixDSym*) fInputFile->Get("JointNuMuAntiNuMuCC0piXsecCovMatrixStat");
  TMatrixDSym* tmpcovsyst = (TMatrixDSym*) fInputFile->Get("JointNuMuAntiNuMuCC0piXsecCovMatrixSyst");

  for(int ibin=0; ibin<Nbins; ibin++){  
    for(int jbin=0; jbin<Nbins; jbin++){
      if(NuPDG==14) (*fFullCovar)(ibin,jbin) = ((*tmpcovstat)(ibin,jbin) + (*tmpcovsyst)(ibin,jbin))*1E76;
      else if(NuPDG==-14) (*fFullCovar)(ibin,jbin) = ((*tmpcovstat)(ibin+Nbins,jbin+Nbins) + (*tmpcovsyst)(ibin+Nbins,jbin+Nbins))*1E76;
    }
  }
  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);
  
  fDataHist->Reset();

  int bincount = 0;
  for (size_t i = 0; i < nangbins; i++){

    // Make slices for data
    fDataHist_Slices.push_back((TH1D*)fInputFile->Get(Form("hXsec%sCC0piDataSlice_%zu",nuType.c_str(),i))->Clone());
    fDataHist_Slices[i]->SetNameTitle(Form("%s_data_Slice%zu",name.c_str(), i),
				      Form("%s_data_Slice%zu%s",name.c_str(), i, titles.c_str()));
    
    // Loop over nbins and set errors from covar
    for (int j = 0; j < fDataHist_Slices[i]->GetNbinsX(); j++){
      fDataHist_Slices[i]->SetBinError(j+1, sqrt((*fFullCovar)(bincount,bincount))*1E-38);
      fDataHist->SetBinContent(bincount+1, fDataHist_Slices[i]->GetBinContent(j+1));
      fDataHist->SetBinError(bincount+1,   fDataHist_Slices[i]->GetBinError(j+1));
      bincount++;
    }
    
    // Save MC slices
    fMCHist_Slices.push_back((TH1D*) fDataHist_Slices[i]->Clone());
    fMCHist_Slices[i]->SetNameTitle(Form("%s_MC_Slice%zu",name.c_str(), i), 
				    Form("%s_MC_Slice%zu%s",name.c_str(),i,titles.c_str()));
    
    SetAutoProcessTH1(fDataHist_Slices[i],kCMD_Write);
    SetAutoProcessTH1(fMCHist_Slices[i]);    
  } 

  return;

};
