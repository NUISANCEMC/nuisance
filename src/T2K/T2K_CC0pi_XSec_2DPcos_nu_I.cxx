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

#include "T2K_CC0pi_XSec_2DPcos_nu_I.h"

//********************************************************************
T2K_CC0pi_XSec_2DPcos_nu_I::T2K_CC0pi_XSec_2DPcos_nu_I(nuiskey samplekey) {
  //********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "T2K_CC0pi_XSec_2DPcos_nu_I sample. \n"
                        "Target: CH \n"
                        "Flux: T2K 2.5 degree off-axis (ND280)  \n"
                        "Signal: CC0pi\n"
                        "https://journals.aps.org/prd/abstract/10.1103/"
                        "PhysRevD.93.112012 Analysis I";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("P_{#mu} (GeV)");
  fSettings.SetYTitle("cos#theta_{#mu}");
  fSettings.SetZTitle("d^{2}#sigma/dP_{#mu}dcos#theta_{#mu} (cm^{2}/GeV)");
  fSettings.SetAllowedTypes("FULL,DIAG/FREE,SHAPE,FIX/SYSTCOV/STATCOV",
                            "FIX/FULL");
  fSettings.SetEnuRange(0.0, 10.0);
  fSettings.DefineAllowedTargets("C,H");

  // CCQELike plot information
  fSettings.SetTitle("T2K_CC0pi_XSec_2DPcos_nu_I");
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = ((GetEventHistogram()->Integral("width") / (fNEvents + 0.)) *
                  1E-38 / (TotalIntegratedFlux()));

  // Setup Histograms
  SetHistograms();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

bool T2K_CC0pi_XSec_2DPcos_nu_I::isSignal(FitEvent *event) {
  return SignalDef::isT2K_CC0pi(event, EnuMin, EnuMax, SignalDef::kAnalysis_I);
};

void T2K_CC0pi_XSec_2DPcos_nu_I::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;

  double pmu = Pmu.Vect().Mag() / 1000.;
  double CosThetaMu = cos(Pnu.Vect().Angle(Pmu.Vect()));

  fXVar = pmu;
  fYVar = CosThetaMu;

  return;
};

void T2K_CC0pi_XSec_2DPcos_nu_I::FillHistograms() {

  Measurement1D::FillHistograms();
  if (Signal) {
    fMCHist_Fine2D->Fill(fXVar, fYVar, Weight);
    FillMCSlice(fXVar, fYVar, Weight);
  }
}

// Modification is needed after the full reconfigure to move bins around
// Otherwise this would need to be replaced by a TH2Poly which is too awkward.
void T2K_CC0pi_XSec_2DPcos_nu_I::ConvertEventRates() {

  for (int i = 0; i < 9; i++) {
    fMCHist_Slices[i]->GetSumw2();
  }

  // Do standard conversion.
  Measurement1D::ConvertEventRates();

  // First scale MC slices also by their width in Y
  fMCHist_Slices[0]->Scale(1.0 / 1.00);
  fMCHist_Slices[1]->Scale(1.0 / 0.60);
  fMCHist_Slices[2]->Scale(1.0 / 0.10);
  fMCHist_Slices[3]->Scale(1.0 / 0.10);
  fMCHist_Slices[4]->Scale(1.0 / 0.05);
  fMCHist_Slices[5]->Scale(1.0 / 0.05);
  fMCHist_Slices[6]->Scale(1.0 / 0.04);
  fMCHist_Slices[7]->Scale(1.0 / 0.04);
  fMCHist_Slices[8]->Scale(1.0 / 0.02);

  // Now Convert into 1D list
  fMCHist->Reset();
  int bincount = 0;
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < fDataHist_Slices[i]->GetNbinsX(); j++) {
      fMCHist->SetBinContent(bincount + 1,
                             fMCHist_Slices[i]->GetBinContent(j + 1));
      fMCHist->SetBinError(bincount + 1, fMCHist_Slices[i]->GetBinError(j + 1));
      bincount++;
    }
  }

  return;
}

void T2K_CC0pi_XSec_2DPcos_nu_I::FillMCSlice(double x, double y, double w) {

  if (y >= -1.0 and y < 0.0)
    fMCHist_Slices[0]->Fill(x, w);
  else if (y >= 0.0 and y < 0.6)
    fMCHist_Slices[1]->Fill(x, w);
  else if (y >= 0.6 and y < 0.7)
    fMCHist_Slices[2]->Fill(x, w);
  else if (y >= 0.7 and y < 0.8)
    fMCHist_Slices[3]->Fill(x, w);
  else if (y >= 0.8 and y < 0.85)
    fMCHist_Slices[4]->Fill(x, w);
  else if (y >= 0.85 and y < 0.90)
    fMCHist_Slices[5]->Fill(x, w);
  else if (y >= 0.90 and y < 0.94)
    fMCHist_Slices[6]->Fill(x, w);
  else if (y >= 0.94 and y < 0.98)
    fMCHist_Slices[7]->Fill(x, w);
  else if (y >= 0.98 and y <= 1.00)
    fMCHist_Slices[8]->Fill(x, w);
}

void T2K_CC0pi_XSec_2DPcos_nu_I::SetHistograms() {

  // Read in 1D Data Histograms
  fInputFile = new TFile(
      (FitPar::GetDataBase() + "/T2K/CC0pi/T2K_CC0PI_2DPmuCosmu_Data.root")
          .c_str(),
      "READ");
  // fInputFile->ls();

  // Read in 1D Data
  fDataHist = (TH1D *)fInputFile->Get("datahist");

  fMCHist_Fine2D = new TH2D("T2K_CC0pi_XSec_2DPcos_nu_I_Fine2D",
                            "T2K_CC0pi_XSec_2DPcos_nu_I_Fine2D", 400, 0.0, 30.0,
                            100, -1.0, 1.0);
  SetAutoProcessTH1(fMCHist_Fine2D);

  TH2D *tempcov = (TH2D *)fInputFile->Get("analysis1_totcov");

  fFullCovar = new TMatrixDSym(fDataHist->GetNbinsX());
  for (int i = 0; i < fDataHist->GetNbinsX(); i++) {
    for (int j = 0; j < fDataHist->GetNbinsX(); j++) {
      (*fFullCovar)(i, j) = tempcov->GetBinContent(i + 1, j + 1);
    }
  }
  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);

  // Read in 2D Data
  fDataPoly = (TH2Poly *)fInputFile->Get("datapoly");
  fDataPoly->SetNameTitle("T2K_CC0pi_XSec_2DPcos_nu_I_datapoly",
                          "T2K_CC0pi_XSec_2DPcos_nu_I_datapoly");
  SetAutoProcessTH1(fDataPoly, kCMD_Write);
  fDataHist->Reset();

  // Read in 2D Data Slices and Make MC Slices
  int bincount = 0;
  for (int i = 0; i < 9; i++) {

    // Get Data Histogram
    // fInputFile->ls();
    fDataHist_Slices.push_back(
        (TH1D *)fInputFile->Get(Form("dataslice_%i", i))->Clone());
    fDataHist_Slices[i]->SetNameTitle(
        Form("T2K_CC0pi_XSec_2DPcos_nu_I_data_Slice%i", i),
        (Form("T2K_CC0pi_XSec_2DPcos_nu_I_data_Slice%i", i)));

    // Loop over nbins and set errors from covar
    for (int j = 0; j < fDataHist_Slices[i]->GetNbinsX(); j++) {
      fDataHist_Slices[i]->SetBinError(
          j + 1, sqrt((*fFullCovar)(bincount, bincount)) * 1E-38);

      // std::cout << "Setting data hist " <<
      // fDataHist_Slices[i]->GetBinContent(j+1) << " " <<
      // fDataHist_Slices[i]->GetBinError(j+1) << std::endl;
      fDataHist->SetBinContent(bincount + 1,
                               fDataHist_Slices[i]->GetBinContent(j + 1));
      fDataHist->SetBinError(bincount + 1,
                             fDataHist_Slices[i]->GetBinError(j + 1));

      bincount++;
    }

    // Make MC Clones
    fMCHist_Slices.push_back((TH1D *)fDataHist_Slices[i]->Clone());
    fMCHist_Slices[i]->SetNameTitle(
        Form("T2K_CC0pi_XSec_2DPcos_nu_I_MC_Slice%i", i),
        (Form("T2K_CC0pi_XSec_2DPcos_nu_I_MC_Slice%i", i)));

    SetAutoProcessTH1(fDataHist_Slices[i], kCMD_Write);
    SetAutoProcessTH1(fMCHist_Slices[i]);
    //    fMCHist_Slices[i]->Reset();
  }

  return;
};

void T2K_CC0pi_XSec_2DPcos_nu_I::Write(std::string drawopt) {
  this->Measurement1D::Write(drawopt);

  if (fResidualHist) {
    std::vector<TH1D *> MCResidual_Slices;
    size_t tb_it = 0;
    for (size_t i = 0; i < fMCHist_Slices.size(); ++i) {
      std::string name = Form("T2K_CC0pi_XSec_2DPcos_nu_I_RESIDUAL_Slice%i", i);
      MCResidual_Slices.push_back(
          static_cast<TH1D *>(fMCHist_Slices[i]->Clone(name.c_str())));
      MCResidual_Slices.back()->Reset();

      for (int j = 0; j < fMCHist_Slices[i]->GetXaxis()->GetNbins(); ++j) {
        double bc = fResidualHist->GetBinContent(tb_it + 1);
        MCResidual_Slices.back()->SetBinContent(j + 1, bc);
        tb_it++;
      }
      MCResidual_Slices.back()->Write();
    }
  }

  if (fChi2LessBinHist) {
    std::vector<TH1D *> MCChi2LessBin_Slices;
    size_t tb_it = 0;
    for (size_t i = 0; i < fMCHist_Slices.size(); ++i) {
      std::string name =
          Form("T2K_CC0pi_XSec_2DPcos_nu_I_Chi2NMinusOne_Slice%i", i);
      MCChi2LessBin_Slices.push_back(
          static_cast<TH1D *>(fMCHist_Slices[i]->Clone(name.c_str())));
      MCChi2LessBin_Slices.back()->Reset();

      for (int j = 0; j < fMCHist_Slices[i]->GetXaxis()->GetNbins(); ++j) {
        double bc = fChi2LessBinHist->GetBinContent(tb_it + 1);
        MCChi2LessBin_Slices.back()->SetBinContent(j + 1, bc);
        tb_it++;
      }
      MCChi2LessBin_Slices.back()->Write();
    }
  }
}