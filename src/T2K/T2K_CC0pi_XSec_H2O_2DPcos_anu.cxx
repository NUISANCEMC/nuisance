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

#include "T2K_CC0pi_XSec_H2O_2DPcos_anu.h"


static size_t nmombins = 8;
static double mom_binning[] = { 400., 530., 670., 800., 1000., 1380., 2010., 3410. }

static int    ncosbins[]    = { 2, 3, 3, 3, 3, 3, 3, 2 };
static double costheta_binning[][] = { { 0.84, 0.94, 1.       },  
                                       { 0.85, 0.92, 0.96, 1. },
                                       { 0.88, 0.93, 0.97, 1. },
                                       { 0.90, 0.94, 0.97, 1. },
                                       { 0.91, 0.95, 0.97, 1. },
                                       { 0.92, 0.96, 0.98, 1. },
                                       { 0.95, 0.98, 1.       }  };

//********************************************************************
T2K_CC0pi_XSec_H2O_2DPcos_anu::T2K_CC0pi_XSec_H2O_2DPcos_anu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "T2K_CC0pi_XSec_H2O_2DPcos_anu. \n"
                        "Target: H2O \n"
                        "Flux: T2K 2.5 degree off-axis (ND280-P0D)  \n"
                        "Signal: CC0pi\n"
                        "arXiv:1908.10249";
  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("p_{#mu} (GeV)");
  fSettings.SetYTitle("cos#theta_{#mu}");
  fSettings.SetZTitle("d^{2}#sigma/dp_{#mu}dcos#theta_{#mu} (cm^{2}/GeV)");
  fSettings.SetAllowedTypes("FULL,DIAG/FREE,SHAPE,FIX/SYSTCOV/STATCOV","FIX/FULL");
  fSettings.SetEnuRange(0.0, 10.0);
  fSettings.DefineAllowedTargets("H,O");

  // CCQELike plot information
  fSettings.SetTitle("T2K_CC0pi_XSec_H2O_2DPcos_anu");
  fSettings.DefineAllowedSpecies("numub");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = ((GetEventHistogram()->Integral("weight")/(fNEvents+0.)) * 18E-38 / (TotalIntegratedFlux("")));

  // Setup Histograms
  SetHistograms();

  // Final setup  
  FinaliseMeasurement();

};


bool T2K_CC0pi_XSec_H2O_2DPcos_anu::isSignal(FitEvent *event){
  return SignalDef::isT2K_CC0piAnuP0D(event, EnuMin, EnuMax);
};

void T2K_CC0pi_XSec_H2O_2DPcos_anu::FillEventVariables(FitEvent* event){

  if (event->NumFSParticle(-13) == 0)
    return;

  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(-13)->fP;

  double pmu = Pmu.Vect().Mag();
  double CosThetaMu = cos(Pnu.Vect().Angle(Pmu.Vect()));

  fXVar = pmu;
  fYVar = CosThetaMu;

  return;
};

void T2K_CC0pi_XSec_H2O_2DPcos_anu::FillHistograms(){

  Measurement1D::FillHistograms();
  if (Signal){
    FillMCSlice( fXVar, fYVar, Weight );
  }

}

// Modification is needed after the full reconfigure to move bins around
// Otherwise this would need to be replaced by a TH2Poly which is too awkward.
void T2K_CC0pi_XSec_H2O_2DPcos_anu::ConvertEventRates(){

  for (int i = 0; i < 7; i++){
    fMCHist_Slices[i]->GetSumw2();
  }

  // Do standard conversion.
  Measurement1D::ConvertEventRates();
  
  // Do we need this?? Maybe already normalized in Thomas data results
  for (size_t i = 0; i < nmombins; ++i) {
    fMCHist_Slices[i]->Scale(1. / (mom_binning[i + 1] - mom_binning[i]));
  }

  // Now Convert into 1D list
  fMCHist->Reset();
  int bincount = 0;
  for (int i = 0; i < 7; i++){
    for (int j = 0; j < fDataHist_Slices[i]->GetNbinsX(); j++){
      fMCHist->SetBinContent(bincount+1, fMCHist_Slices[i]->GetBinContent(j+1));
      bincount++;
    }
  }

  return;

}

void T2K_CC0pi_XSec_H2O_2DPcos_anu::FillMCSlice(double x, double y, double w){

  for (size_t i = 0; i < nmombins; ++i) {
    if ((y > mom_binning[i]) && (y <= mom_binning[i + 1])) {
      fMCHist_Slices[i]->Fill(x, w);
    }
  } 
}


void T2K_CC0pi_XSec_H2O_2DPcos_anu::SetHistograms(){

  // Read in 1D Data Histograms
  fInputFile = new TFile( (FitPar::GetDataBase() + "/T2K/CC0pi/AntiNuMuH2O/AntiNuMuOnH2O_unreg.root").c_str(),"READ");

  fDataHist = (TH1D*) fInputFile->Get("xsecDataRelease");
  int Nbins = fDataHist->GetNbinsX();
  /*TH2D* tempcorr = (TH2D*) fInputFile->Get("covDataRelease");
  // Read in 1D Data
  fFullCovar = new TMatrixDSym(fDataHist->GetNbinsX());
  for (int i = 0; i < fDataHist->GetNbinsX(); i++){
    for (int j = 0; j < fDataHist->GetNbinsX(); j++){
      (*fFullCovar)(i,j) = tempcorr->GetBinContent(i+1, j+1)*fDataHist->GetBinError(i+1)*1e38*fDataHist->GetBinError(j+1)*1e38;
    }
  }*/
  fFullCovar = (TMatrixDSym*) fInputFile->Get("covDataRelease");
  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);


  // Read in 2D Data Slices and Make MC Slices
  int bincount = 0;
  for (int i = 0; i < nmombins; ++i)
  {
    // Get Data Histogram
    fDataHist_Slices.push_back(new TH1D(Form("T2K_CC0pi_XSec_H2O_2DPcos_anu_data_Slice%i",i),Form("T2K_CC0pi_XSec_H2O_2DPcos_anu_data_Slice%i",i),ncosbins[i],costheta_binning[i]););
    for (int j = 0; j < ncosbins[i]; ++j)
    {

      fDataHist_Slices[i]->SetBinContent(j+1, fDataHist->GetBinContent(bincount+1));
      fDataHist_Slices[i]->SetBinError(j+1,   sqrt((*fFullCovar)(bincount,bincount))*1e-38);
      bincount++;
    }

    fMCHist_Slices.push_back((TH1D*) fDataHist_Slices[i]->Clone());
    fMCHist_Slices[i]->SetNameTitle(Form("T2K_CC0pi_XSec_H2O_2DPcos_anu_MC_Slice%i",i),
                                      (Form("T2K_CC0pi_XSec_H2O_2DPcos_anu_MC_Slice%i",i)));

    SetAutoProcessTH1(fDataHist_Slices[i],kCMD_Write);
    SetAutoProcessTH1(fMCHist_Slices[i]);
  }

  return;
};
