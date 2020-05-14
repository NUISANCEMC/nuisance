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

static size_t nangbins = 9;
static double angular_binning_costheta[] = {-1,   0,   0.6,  0.7,  0.8,
                                            0.85, 0.9, 0.94, 0.98, 1};

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
  fSettings.DefineAllowedTargets("C,H");
  fSettings.SetEnuRangeFromFlux(fFluxHist);

  // CCQELike plot information
  fSettings.SetTitle("T2K_CC0pi_XSec_2DPcos_nu_I");
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  fMaskMomOverflow = false;
  if (samplekey.Has("mask_mom_overflow")) {
    fMaskMomOverflow = samplekey.GetB("mask_mom_overflow");
  }

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = ((GetEventHistogram()->Integral("width") / (fNEvents + 0.)) *
                  1E-38 / (TotalIntegratedFlux()));

  assert(std::isnormal(fScaleFactor));

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
  // Do standard conversion.
  Measurement1D::ConvertEventRates();

  // Scale MC slices by their bin area
  for (size_t i = 0; i < nangbins; ++i) {
    fMCHist_Slices[i]->Scale(fScaleFactor / (angular_binning_costheta[i + 1] -
                                             angular_binning_costheta[i]),
                             "width");
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

void T2K_CC0pi_XSec_2DPcos_nu_I::FillMCSlice(double x, double y, double w) {

  for (size_t i = 0; i < nangbins; ++i) {
    if ((y >= angular_binning_costheta[i]) &&
        (y < angular_binning_costheta[i + 1])) {
      fMCHist_Slices[i]->Fill(x, w);
    }
  }
}

void T2K_CC0pi_XSec_2DPcos_nu_I::SetHistograms() {

  // Read in 1D Data Histograms
  TFile input(
      (FitPar::GetDataBase() + "/T2K/CC0pi/T2K_CC0PI_2DPmuCosmu_Data.root")
          .c_str(),
      "READ");
  fMCHist_Fine2D = new TH2D("T2K_CC0pi_XSec_2DPcos_nu_I_Fine2D",
                            "T2K_CC0pi_XSec_2DPcos_nu_I_Fine2D", 400, 0.0, 30.0,
                            100, -1.0, 1.0);
  fMCHist_Fine2D->SetDirectory(NULL);
  SetAutoProcessTH1(fMCHist_Fine2D);

  TH2D *tempcov = (TH2D *)input.Get("analysis1_totcov");

  fFullCovar = new TMatrixDSym(tempcov->GetNbinsX());
  for (int i = 0; i < tempcov->GetNbinsX(); i++) {
    for (int j = 0; j < tempcov->GetNbinsX(); j++) {
      (*fFullCovar)(i, j) = tempcov->GetBinContent(i + 1, j + 1);
    }
  }

  // Read in 2D Data Slices and Make MC Slices
  int bincount = 0;
  for (size_t i = 0; i < nangbins; i++) {

    fDataHist_Slices.push_back(
        (TH1D *)input.Get(Form("dataslice_%lu", i))->Clone());

    fDataHist_Slices[i]->SetNameTitle(
        Form("T2K_CC0pi_XSec_2DPcos_nu_I_data_Slice%lu", i),
        (Form("T2K_CC0pi_XSec_2DPcos_nu_I_data_Slice%lu, cos(#theta) [%f,%f] ",
              i, angular_binning_costheta[i],
              angular_binning_costheta[i + 1])));
    fDataHist_Slices.back()->SetDirectory(NULL);

    // Loop over nbins and set errors from covar
    for (int j = 0; j < fDataHist_Slices[i]->GetNbinsX(); j++) {
      fDataHist_Slices[i]->SetBinError(
          j + 1, sqrt((*fFullCovar)(bincount, bincount)) * 1E-38);
      bincount++;
    }
  }

  assert(bincount == tempcov->GetNbinsX());

  if (fMaskMomOverflow) {
    MaskMomOverflow();
    bincount = fFullCovar->GetNcols();
  }

  std::vector<std::pair<double, double> > data_slice_bcbes;
  for (size_t i = 0; i < nangbins; i++) {
    for (int j = 0; j < fDataHist_Slices[i]->GetNbinsX(); j++) {
      data_slice_bcbes.push_back(
          std::make_pair(fDataHist_Slices[i]->GetBinContent(j + 1),
                         fDataHist_Slices[i]->GetBinError(j + 1)));
    }
  }

  for (size_t i = 0; i < nangbins; i++) {
    fMCHist_Slices.push_back((TH1D *)fDataHist_Slices[i]->Clone());
    fMCHist_Slices.back()->SetDirectory(NULL);
    fMCHist_Slices.back()->Reset();
    fMCHist_Slices.back()->SetLineColor(kRed);
    fMCHist_Slices[i]->SetNameTitle(
        Form("T2K_CC0pi_XSec_2DPcos_nu_I_MC_Slice%lu", i),
        (Form("T2K_CC0pi_XSec_2DPcos_nu_I_MC_Slice%lu, cos(#theta) [%f,%f] ", i,
              angular_binning_costheta[i], angular_binning_costheta[i + 1])));
  }

  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);

  fDataHist =
      new TH1D("T2K_CC0pi_XSec_2DPcos_nu_I_DATA_1D",
               "T2K_CC0pi_XSec_2DPcos_nu_I_DATA_1D", bincount, 0, bincount);
  fDataHist->SetDirectory(NULL);
  for (size_t i = 0; i < data_slice_bcbes.size(); ++i) {
    fDataHist->SetBinContent(i + 1, data_slice_bcbes[i].first);
    fDataHist->SetBinError(i + 1, data_slice_bcbes[i].second);
  }

  SetShapeCovar();

  fMCHist = (TH1D *)fDataHist->Clone("T2K_CC0pi_XSec_2DPcos_nu_I_MC_1D");
  fMCHist->SetDirectory(NULL);
  return;
}

void T2K_CC0pi_XSec_2DPcos_nu_I::MaskMomOverflow() {

  std::vector<int> bins_to_cut;
  size_t nallbins = 0;
  for (size_t i = 0; i < nangbins; i++) {

    std::vector<double> slice_bin_edges;
    slice_bin_edges.push_back(
        fDataHist_Slices[i]->GetXaxis()->GetBinLowEdge(1));
    for (int j = 0; j < (fDataHist_Slices[i]->GetNbinsX() - 1); j++) {
      slice_bin_edges.push_back(
          fDataHist_Slices[i]->GetXaxis()->GetBinUpEdge(j + 1));
      nallbins++;
    }

    bins_to_cut.push_back(nallbins++);
    TH1D *tmp = new TH1D(fDataHist_Slices[i]->GetName(),
                         fDataHist_Slices[i]->GetTitle(),
                         slice_bin_edges.size() - 1, slice_bin_edges.data());
    tmp->SetDirectory(NULL);
    for (int j = 0; j < (fDataHist_Slices[i]->GetNbinsX() - 1); j++) {
      tmp->SetBinContent(j + 1, fDataHist_Slices[i]->GetBinContent(j + 1));
      tmp->SetBinError(j + 1, fDataHist_Slices[i]->GetBinError(j + 1));
    }

    delete fDataHist_Slices[i];
    fDataHist_Slices[i] = tmp;
  }

  TMatrixDSym *tmpcovar = new TMatrixDSym(nallbins - bins_to_cut.size());
  int icut = 0;
  for (int ifull = 0; ifull < fFullCovar->GetNcols(); ifull++) {
    if (std::find(bins_to_cut.begin(), bins_to_cut.end(), ifull) !=
        bins_to_cut.end()) {
      continue;
    }
    int jcut = 0;
    for (int jfull = 0; jfull < fFullCovar->GetNcols(); jfull++) {
      if (std::find(bins_to_cut.begin(), bins_to_cut.end(), jfull) !=
          bins_to_cut.end()) {
        continue;
      }
      (*tmpcovar)[icut][jcut] = (*fFullCovar)[ifull][jfull];
      jcut++;
    }
    icut++;
  }
  delete fFullCovar;
  fFullCovar = tmpcovar;
}

void T2K_CC0pi_XSec_2DPcos_nu_I::Write(std::string drawopt) {
  this->Measurement1D::Write(drawopt);

  for (size_t i = 0; i < nangbins; i++) {
    fMCHist_Slices[i]->Write();
    fDataHist_Slices[i]->Write();
  }

  if (fResidualHist) {
    std::vector<TH1D *> MCResidual_Slices;
    size_t tb_it = 0;
    for (size_t i = 0; i < fMCHist_Slices.size(); ++i) {
      std::string name =
          Form("T2K_CC0pi_XSec_2DPcos_nu_I_RESIDUAL_Slice%lu", i);
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
          Form("T2K_CC0pi_XSec_2DPcos_nu_I_Chi2NMinusOne_Slice%lu", i);
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
