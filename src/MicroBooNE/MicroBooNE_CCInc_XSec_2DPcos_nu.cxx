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

#include "MicroBooNE_CCInc_XSec_2DPcos_nu.h"

//********************************************************************
MicroBooNE_CCInc_XSec_2DPcos_nu::MicroBooNE_CCInc_XSec_2DPcos_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MicroBooNE_CCInc_XSec_2DPcos_nu sample. \n" \
                        "Target: Ar \n" \
                        "Flux: BNB FHC numu \n" \
                        "Signal: CC inclusive \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("P_{#mu}^{reco} (GeV)");
  fSettings.SetYTitle("cos#theta_{#mu}^{reco}");
  fSettings.SetZTitle("d^{2}#sigma/dP_{#mu}^{reco}dcos#theta_{#mu}^{reco} (cm^{2}/GeV)");
  fSettings.SetAllowedTypes("FULL,DIAG/FREE,SHAPE,FIX/SYSTCOV/STATCOV","FIX/FULL");
  fSettings.SetEnuRange(0.0, 10.0);
  fSettings.DefineAllowedTargets("Ar");

  // Plot information
  fSettings.SetTitle("MicroBooNE_CCInc_XSec_2DPcos_nu");
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


bool MicroBooNE_CCInc_XSec_2DPcos_nu::isSignal(FitEvent* event) {
  return SignalDef::isCCINC(event, 14, EnuMin, EnuMax);
};


void MicroBooNE_CCInc_XSec_2DPcos_nu::FillEventVariables(FitEvent* event) {
  if (event->NumFSParticle(13) == 0) {
    return;
  }

  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;

  double pmu = Pmu.Vect().Mag() / 1000;
  double CosThetaMu = cos(Pnu.Vect().Angle(Pmu.Vect()));

  fXVar = pmu;
  fYVar = CosThetaMu;

  return;
};


void MicroBooNE_CCInc_XSec_2DPcos_nu::FillHistograms() {
  Measurement1D::FillHistograms();
  if (Signal) {
    fMCHist_Fine2D->Fill(fXVar, fYVar, Weight);
    FillMCSlice(fXVar, fYVar, Weight);
  }
}


void MicroBooNE_CCInc_XSec_2DPcos_nu::ConvertEventRates() {
  for (size_t i=0; i<fMCHist_Slices.size(); i++) {
    fMCHist_Slices[i]->GetSumw2();
  }

  // Do standard conversion
  Measurement1D::ConvertEventRates();

  // Apply MC truth -> reco smearing
  std::vector<TH1D*> slices_true;
  for (size_t i=0; i<fMCHist_Slices.size(); i++) {
    TH1D* h = (TH1D*) fMCHist_Slices[i]->Clone(TString(fMCHist_Slices[i]->GetName()) + "_true");
    slices_true.push_back(h);
  }

  for (int ireco=1; ireco<fMCHist->GetNbinsX()+1; ireco++) {
    double total = 0;
    for (int itrue=1; itrue<fMCHist->GetNbinsX()+1; itrue++) {
      std::pair<size_t, size_t> idx = fPolyBinMap[itrue];
      TH1D* h = slices_true[idx.first];
      total += h->GetBinContent(idx.second+1) * h->GetBinWidth(idx.second+1) * fSmearingMatrix->operator()(ireco-1, itrue-1);
    }
    std::pair<size_t, size_t> idx = fPolyBinMap[ireco];
    TH1D* h = fMCHist_Slices[idx.first];
    h->SetBinContent(idx.second+1, total / h->GetBinWidth(idx.second+1));
  }

  for (size_t i=0; i<slices_true.size(); i++) {
    delete slices_true[i];
  }

  // Scale MC slices also by their width in Y
  for (size_t i=0; i<fMCHist_Slices.size(); i++) {
    float w = fEdgesCt[i+1] - fEdgesCt[i];
    fMCHist_Slices[i]->Scale(1.0 / w);
  }

  // Convert into 1D list
  fMCHist->Reset();
  int bincount = 0;
  for (size_t i=0; i<fDataHist_Slices.size(); i++) {
    for (int j=0; j<fDataHist_Slices[i]->GetNbinsX(); j++) {
      fMCHist->SetBinContent(bincount+1, fMCHist_Slices[i]->GetBinContent(j+1));
      fMCHist->SetBinError(bincount+1, fMCHist_Slices[i]->GetBinError(j+1));
      bincount++;
    }
  }
}


void MicroBooNE_CCInc_XSec_2DPcos_nu::FillMCSlice(double x, double y, double w) {
  if      (y >= -1.00 && y < -0.50) fMCHist_Slices[0]->Fill(x, w);
  else if (y >= -0.50 && y <  0.00) fMCHist_Slices[1]->Fill(x, w);
  else if (y >=  0.00 && y <  0.27) fMCHist_Slices[2]->Fill(x, w);
  else if (y >=  0.27 && y <  0.45) fMCHist_Slices[3]->Fill(x, w);
  else if (y >=  0.45 && y <  0.62) fMCHist_Slices[4]->Fill(x, w);
  else if (y >=  0.62 && y <  0.76) fMCHist_Slices[5]->Fill(x, w);
  else if (y >=  0.76 && y <  0.86) fMCHist_Slices[6]->Fill(x, w);
  else if (y >=  0.86 && y <  0.94) fMCHist_Slices[7]->Fill(x, w);
  else if (y >=  0.94 && y <= 1.00) fMCHist_Slices[8]->Fill(x, w);
}


void MicroBooNE_CCInc_XSec_2DPcos_nu::SetHistograms() {
  // Read in 1D Data Histograms
  fInputFile = new TFile( (FitPar::GetDataBase() + "/MicroBooNE/CCinc/microboone_numu_cc_inclusive.root").c_str());

  // Read in 1D Data
  fDataHist = (TH1D*) fInputFile->Get("xsec_data");
  fDataHist->Scale(1e-38);

  fMCHist_Fine2D = new TH2D("MicroBooNE_CCInc_XSec_2DPcos_nu_Fine2D",
                            "MicroBooNE_CCInc_XSec_2DPcos_nu_Fine2D",
                            400, 0.0, 2.5, 100, -1.0, 1.0);
  SetAutoProcessTH1(fMCHist_Fine2D);

  // Load covariance matrix
  TH2D* tempcov = (TH2D*) fInputFile->Get("covariance_matrix");

  fFullCovar = new TMatrixDSym(fDataHist->GetNbinsX());
  for (int i=0; i<fDataHist->GetNbinsX(); i++) {
    for (int j=0; j<fDataHist->GetNbinsX(); j++) {
      (*fFullCovar)(i,j) = tempcov->GetBinContent(i+1, j+1);
    }
  }

  fDecomp = StatUtils::GetDecomp(fFullCovar);

  // Load smearing matrix
  TH2D* tempsmear = (TH2D*) fInputFile->Get("smearing_matrix");

  fSmearingMatrix = new TMatrixDSym(fDataHist->GetNbinsX());
  for (int i=0; i<fDataHist->GetNbinsX(); i++) {
    for (int j=0; j<fDataHist->GetNbinsX(); j++) {
      (*fSmearingMatrix)(i,j) = tempsmear->GetBinContent(i+1, j+1);
    }
  }

  // Mapping of polybins to costheta slices
  fPolyBinIDs = {
    {  1,  2,  3,  4,  5 },
    {  6,  7,  8,  9, 10 },
    { 11, 12, 13, 14, 15 },
    { 16, 17, 18, 19     },
    { 20, 21, 22, 23     },
    { 24, 25, 26, 27     },
    { 28, 29, 30, 31, 32 },
    { 33, 34, 35, 36, 37 },
    { 38, 39, 40, 41, 42 }
  };

  for (size_t i=0; i<fPolyBinIDs.size(); i++) {
    for (size_t j=0; j<fPolyBinIDs[i].size(); j++) {
      int id = fPolyBinIDs[i][j];
      fPolyBinMap[id] = { i, j };
    }
  }

  // Bin edges
  fEdgesCt = { -1.00, -0.50, 0.00, 0.27, 0.45, 0.62, 0.76, 0.86, 0.94, 1.00 };

  fEdgesP = {
    { 0.00, 0.18, 0.30, 0.45, 0.77, 2.50 },
    { 0.00, 0.18, 0.30, 0.45, 0.77, 2.50 },
    { 0.00, 0.18, 0.30, 0.45, 0.77, 2.50 },
    { 0.00, 0.30, 0.45, 0.77, 2.50       },
    { 0.00, 0.30, 0.45, 0.77, 2.50       },
    { 0.00, 0.30, 0.45, 0.77, 2.50       },
    { 0.00, 0.30, 0.45, 0.77, 1.28, 2.50 },
    { 0.00, 0.30, 0.45, 0.77, 1.28, 2.50 },
    { 0.00, 0.30, 0.45, 0.77, 1.28, 2.50 }
  };

  // Split 1D data into cos(theta) slices
  for (size_t i=0; i<fPolyBinIDs.size(); i++) {
    TString name = Form("MicroBooNE_CCInc_XSec_2DPcos_nu_data_Slice%lu",i);
    TH1D* h = new TH1D(name, name, fPolyBinIDs[i].size(), fEdgesP[i].data());
    h->Sumw2();

    fDataHist_Slices.push_back(h);

    for (size_t j=0; j<fPolyBinIDs[i].size(); j++) {
      int binid = fPolyBinIDs[i][j];
      h->SetBinContent(j+1, fDataHist->GetBinContent(binid));

      float err = sqrt((*fFullCovar)(binid-1, binid-1)) * 1e-38;
      h->SetBinError(j+1, err);
      fDataHist->SetBinError(binid, err);
    }

    fMCHist_Slices.push_back((TH1D*) h->Clone());
    name = Form("MicroBooNE_CCInc_XSec_2DPcos_nu_MC_Slice%lu",i);
    fMCHist_Slices[i]->SetNameTitle(name, name);
    fMCHist_Slices[i]->Reset();

    SetAutoProcessTH1(fDataHist_Slices[i], kCMD_Write);
    SetAutoProcessTH1(fMCHist_Slices[i]);
  }
}

