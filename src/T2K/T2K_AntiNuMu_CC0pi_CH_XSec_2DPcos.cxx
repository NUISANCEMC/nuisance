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

#include "T2K_AntiNuMu_CC0pi_CH_XSec_2DPcos.h"

static int    nangbins = 9;
static double angular_binning_costheta[] = {-1,    0.2, 0.6,  0.7,  0.8,
                                             0.85, 0.9, 0.94, 0.98, 1   };

//********************************************************************
T2K_AntiNuMu_CC0pi_CH_XSec_2DPcos::T2K_AntiNuMu_CC0pi_CH_XSec_2DPcos(nuiskey samplekey) {
  //********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "T2K_AntiNuMu_CC0pi_2DPcos sample. \n"
                        "Target: CH \n"
                        "Flux: T2K 2.5 degree off-axis (ND280)  \n"
                        "Signal: CC0pi\n"
                        "arXiv:2002.09323";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("P_{#mu} (GeV)");
  fSettings.SetYTitle("cos#theta_{#mu}");
  fSettings.SetZTitle("d^{2}#sigma/dP_{#mu}dcos#theta_{#mu} (cm^{2}/GeV)");
  fSettings.SetAllowedTypes("DIAG,FULL/FREE,SHAPE,FIX/SYSTCOV/STATCOV","FIX");
  fSettings.SetEnuRange(0.0, 30.0);
  fSettings.DefineAllowedTargets("C,H");

  // CCQELike plot information
  fSettings.SetTitle("T2K_AntiNuMu_CC0pi_CH_XSec_2DPcos");
  fSettings.DefineAllowedSpecies("numub");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = ((GetEventHistogram()->Integral("width")/(fNEvents+0.)) * 1E-38 / (TotalIntegratedFlux()));

  assert(std::isnormal(fScaleFactor));

  // Setup Histograms
  SetHistograms();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

bool T2K_AntiNuMu_CC0pi_CH_XSec_2DPcos::isSignal(FitEvent *event) {
  return SignalDef::isT2K_CC0pi(event, EnuMin, EnuMax, SignalDef::kAnalysis_I);
};

void T2K_AntiNuMu_CC0pi_CH_XSec_2DPcos::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(-13) == 0)
    return;

  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(-13)->fP;

  double pmu = Pmu.Vect().Mag() / 1000.;
  double CosThetaMu = cos(Pnu.Vect().Angle(Pmu.Vect()));

  fXVar = pmu;
  fYVar = CosThetaMu;

  return;
};

void T2K_AntiNuMu_CC0pi_CH_XSec_2DPcos::FillHistograms() {

  Measurement1D::FillHistograms();
  if (Signal) {
    FillMCSlice(fXVar, fYVar, Weight);
  }
}

// Modification is needed after the full reconfigure to move bins around
// Otherwise this would need to be replaced by a TH2Poly which is too awkward.
void T2K_AntiNuMu_CC0pi_CH_XSec_2DPcos::ConvertEventRates() {
  // Do standard conversion.
  Measurement1D::ConvertEventRates();

  // Scale MC slices by their bin area
  for (size_t i = 0; i < nangbins; ++i) {
    fMCHist_Slices[i]->Scale(fScaleFactor / (angular_binning_costheta[i + 1] - angular_binning_costheta[i]), "width");
  }

  // Now Convert into 1D list
  fMCHist->Reset();
  int bincount = 0;
  for (size_t i = 0; i < nangbins; i++) {
    for (int j = 0; j < fDataHist_Slices[i]->GetNbinsX(); j++) {
      fMCHist->SetBinContent(bincount + 1,
                             fMCHist_Slices[i]->GetBinContent(j + 1));
      fMCHist->SetBinError(bincount + 1, fMCHist_Slices[i]->GetBinError(j + 1));
      bincount++;
    }
  }

  return;
}

void T2K_AntiNuMu_CC0pi_CH_XSec_2DPcos::FillMCSlice(double x, double y, double w) {

  for (size_t i = 0; i < nangbins; ++i) {
    if ((y >= angular_binning_costheta[i]) &&
        (y < angular_binning_costheta[i + 1])) {
      fMCHist_Slices[i]->Fill(x, w);
    }
  }
}

void T2K_AntiNuMu_CC0pi_CH_XSec_2DPcos::SetHistograms() {

  // Read in 1D Data Histograms
  fInputFile = new TFile( (FitPar::GetDataBase() + "/T2K/CC0pi/JointNuMu-AntiNuMu/JointNuMuAntiNuMuCC0piXsecDataRelease.root").c_str(),"READ");

  TH1D* hLinearResult = (TH1D*) fInputFile->Get("hAntiNuMuCC0piXsecLinearResult");

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
      (*fFullCovar)(ibin,jbin) = ( (*tmpcovstat)(ibin+Nbins,jbin+Nbins) + (*tmpcovsyst)(ibin+Nbins,jbin+Nbins))*1E38*1E38;
    }
  }
  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);
  
  fDataHist->Reset();
  // Read in 1D Data Slices and Make MC Slices NuMu CC0pi Xsec
  int bincount = 0;
  for (int i = 0; i < nangbins; i++){
    //Get Data Histogram
    fDataHist_Slices.push_back((TH1D*)fInputFile->Get(Form("hXsecAntiNuMuCC0piDataSlice_%i",i))->Clone());
    fDataHist_Slices[i]->SetNameTitle(Form("T2K_AntiNuMu_CC0pi_2DPcos_data_Slice%i",i),
      (Form("T2K_AntiNuMu_CC0pi_2DPcos_data_Slice%i",i)));

      //Loop over nbins and set errors from covar
    for (int j = 0; j < fDataHist_Slices[i]->GetNbinsX(); j++){
      fDataHist_Slices[i]->SetBinError(j+1, sqrt((*fFullCovar)(bincount,bincount))*1E-38);
      fDataHist->SetBinContent(bincount+1, fDataHist_Slices[i]->GetBinContent(j+1));
      fDataHist->SetBinError(bincount+1,   fDataHist_Slices[i]->GetBinError(j+1));
      bincount++;
    }

    //Make MC Clones
    fMCHist_Slices.push_back((TH1D*) fDataHist_Slices[i]->Clone());
    fMCHist_Slices[i]->SetNameTitle(Form("T2K_AntiNuMu_CC0pi_2DPcos_MC_Slice%i",i), (Form("T2K_AntiNuMu_CC0pi_2DPcos_MC_Slice%i",i)));

    SetAutoProcessTH1(fMCHist_Slices[i],kCMD_Write);
    SetAutoProcessTH1(fMCHist_Slices[i]);

  }

  return;

}

