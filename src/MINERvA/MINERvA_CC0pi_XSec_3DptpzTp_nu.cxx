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

/*
  Authors: Clarence Wret (v2 2018)
*/

#include "MINERvA_CC0pi_XSec_3DptpzTp_nu.h"
#include "MINERvA_SignalDef.h"

//********************************************************************
void MINERvA_CC0pi_XSec_3DptpzTp_nu::SetupDataSettings() {
  //********************************************************************
  // Define what files to use from the dist
  std::string datafile = "";
  std::string corrfile = "";
  std::string titles = "";
  std::string distdescript = "";
  std::string histname = "";

  datafile = "MINERvA/CC0pi_3D/cov_ptpl_3DptpzTp_qelike.root";
  corrfile = "MINERvA/CC0pi_3D/cov_ptpl_3DptpzTp_qelike.root";
  titles = "MINERvA CC0#pi #nu_{#mu} p_{t} p_{z};p_{z} (GeV);p_{t} "
    "(GeV);d^{2}#sigma/dP_{t}dP_{z} (cm^{2}/GeV^{2}/nucleon)";
  distdescript = "MINERvA_CC0pi_XSec_3DptpzTp_nu sample";
  histname = "pt_pl_cross_section";

  fSettings.SetTitle(GeneralUtils::ParseToStr(titles, ";")[0]);
  fSettings.SetXTitle(GeneralUtils::ParseToStr(titles, ";")[1]);
  fSettings.SetYTitle(GeneralUtils::ParseToStr(titles, ";")[2]);
  fSettings.SetZTitle(GeneralUtils::ParseToStr(titles, ";")[3]);

  // Sample overview ---------------------------------------------------
  std::string descrip = distdescript + "\n"
    "Target: CH \n"
    "Flux: MINERvA Low Energy FHC numu  \n"
    "Signal: CC-0pi \n";
  fSettings.SetDescription(descrip);

  // The input ROOT file
  //fSettings.SetDataInput(FitPar::GetDataBase() + datafile);
  //fSettings.SetCovarInput(FitPar::GetDataBase() + corrfile);

  nptbins = 7;
  ptbins = {0, 0.15, 0.25, 0.4, 0.7, 1, 2.5}; // GeV
  npzbins = 4;
  pzbins = {1.5, 3.5, 8, 20}; // GeV
  ntpbins = 15;
  sumTpbins = {0, 40, 80, 120, 160, 200, 240, 280, 320, 360, 400, 600, 800, 1000, 10000}; // MeV

  // Data is actually an array of 2D measurements
  // Have the 2D in pt pz and then Tp to be the left over axis
  //fDataHist = new TH2D("minerva_test", "minerva_test", nptbins, ptbins, npzbins, pzbins);

  for (int i = 0; i < ntpbins; ++i) {
    fDataHist_Slices.push_back(new TH2D(Form("minerva_data_test_%i", i), Form("minerva_data_test_%i", i), nptbins, ptbins, npzbins, pzbins);
    fMCHist_Slices.push_back(new TH2D(Form("minerva_mc_test_%i", i), Form("minerva_mc_test_%i", i), nptbins, ptbins, npzbins, pzbins);
  }

  //fDataHist->SetName((fSettings.GetName() + "_data").c_str());
  //fDataHist->SetTitle(fSettings.GetFullTitles().c_str());

}

//********************************************************************
MINERvA_CC0pi_XSec_3DptpzTp_nu::MINERvA_CC0pi_XSec_3DptpzTp_nu(nuiskey samplekey) {
  //********************************************************************

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/FULL,DIAG/MASK", "FIX/FULL");
  fSettings.SetEnuRange(0.0, 100.0);
  fSettings.DefineAllowedTargets("C,H");
  fSettings.DefineAllowedSpecies("numu");

  SetupDataSettings();
  FinaliseSampleSettings();

  fScaleFactor =
    (GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.)) /
    this->TotalIntegratedFlux();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

//********************************************************************
void MINERvA_CC0pi_XSec_3DptpzTp_nu::FillEventVariables(FitEvent *event) {
  //********************************************************************
  // Checking to see if there is a Muon
  if (event->NumFSParticle(13) == 0) return;

  // Get the muon kinematics
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;

  Double_t px = Pmu.X() / 1000;
  Double_t py = Pmu.Y() / 1000;
  Double_t pt = sqrt(px * px + py * py);

  // y-axis is transverse momentum for both measurements
  fYVar = pt;

  // Don't want to assume the event generators all have neutrino coming along
  // z pz is muon momentum projected onto the neutrino direction
  Double_t pz = Pmu.Vect().Dot(Pnu.Vect() * (1.0 / Pnu.Vect().Mag())) / 1000.;
  // Set Hist Variables
  fXVar = pz;

  // Sum up kinetic energy of protons
  double sum = 0.0;
  for (std::vector<FitParticle*>::iterator it = event->GetAllFSProton().begin(); 
      it != event->GetAllFSProton().end(); ++it) {
    sum += (*it)->KE();
  }
  fZVar = sum;

};

void MINERvA_CC0pi_XSec_3DptpzTp_nu::FillHistograms() {
  Measurement2D::FillHistograms();
  if (Signal) {
    FillMCSlice(fXVar, fYVar, fZVar, Weight);
  }
}

void MINERvA_CC0pi_XSec_3DptpzTp_nu::FillMCSlice(double x, double y, double z, double w) {
  // Find the bin
  for (int i = 0; i < ntpbins; ++i) {
    if (z > sumTpbins[i] && z < sumTpbins[i+1]) fMCHist_Slices[i]->Fill(y, x, w);
  }
}

//********************************************************************
bool MINERvA_CC0pi_XSec_3DptpzTp_nu::isSignal(FitEvent *event) {
  //********************************************************************
  return SignalDef::isCC0pi_MINERvAPTPZ(event, 14, EnuMin, EnuMax);

  // From Dan
  // if not (2212 or 2112 or 22 and E<10) BAD BAD BAD
};

//********************************************************************
// Custom likelihood calculator because binning of covariance matrix
double MINERvA_CC0pi_XSec_3DptpzTp_nu::GetLikelihood() {
  //********************************************************************

  // The calculated chi2
  double chi2 = 0.0;

  // Support shape comparisons
  double scaleF = fDataHist->Integral() / fMCHist->Integral();
  if (fIsShape) {
    fMCHist->Scale(scaleF);
    fMCFine->Scale(scaleF);
  }

  // Even though this chi2 calculation looks ugly it is _EXACTLY_ what MINERvA
  // used for their measurement Can be prettified in due time but for now keep

  int nbinsx = fMCHist->GetNbinsX();
  int nbinsy = fMCHist->GetNbinsY();
  Int_t Nbins = nbinsx * nbinsy;

  // Loop over the covariance matrix bins
  for (int i = 0; i < Nbins; ++i) {
    int xbin = (i % nbinsx) + 1;
    int ybin = (i / nbinsx) + 1;
    double datax = fDataHist->GetBinContent(xbin, ybin);
    double mcx = fMCHist->GetBinContent(xbin, ybin);
    double chi2_bin = 0;
    for (int j = 0; j < Nbins; ++j) {
      int xbin2 = (j % nbinsx) + 1;
      int ybin2 = (j / nbinsx) + 1;

      double datay = fDataHist->GetBinContent(xbin2, ybin2);
      double mcy = fMCHist->GetBinContent(xbin2, ybin2);

      double chi2_xy = (datax - mcx) * (*covar)(i, j) * (datay - mcy);
      chi2_bin += chi2_xy;
    }
    if (fResidualHist) {
      fResidualHist->SetBinContent(xbin, ybin, chi2_bin);
    }
    chi2 += chi2_bin;
  }

  if (fChi2LessBinHist) {
    for (int igbin = 0; igbin < Nbins; ++igbin) {
      int igxbin = (igbin % nbinsx) + 1;
      int igybin = (igbin / nbinsx) + 1;
      double tchi2 = 0;
      for (int i = 0; i < Nbins; ++i) {
        int xbin = (i % nbinsx) + 1;
        int ybin = (i / nbinsx) + 1;
        if ((xbin == igxbin) && (ybin == igybin)) {
          continue;
        }
        double datax = fDataHist->GetBinContent(xbin, ybin);
        double mcx = fMCHist->GetBinContent(xbin, ybin);
        double chi2_bin = 0;
        for (int j = 0; j < Nbins; ++j) {
          int xbin2 = (j % nbinsx) + 1;
          int ybin2 = (j / nbinsx) + 1;
          if ((xbin2 == igxbin) && (ybin2 == igybin)) {
            continue;
          }
          double datay = fDataHist->GetBinContent(xbin2, ybin2);
          double mcy = fMCHist->GetBinContent(xbin2, ybin2);

          double chi2_xy = (datax - mcx) * (*covar)(i, j) * (datay - mcy);
          chi2_bin += chi2_xy;
        }
        tchi2 += chi2_bin;
      }

      fChi2LessBinHist->SetBinContent(igxbin, igybin, tchi2);
    }
  }

  // Normalisation penalty term if included
  if (fAddNormPen) {
    chi2 +=
      (1 - (fCurrentNorm)) * (1 - (fCurrentNorm)) / (fNormError * fNormError);
    NUIS_LOG(REC, "Norm penalty = " << (1 - (fCurrentNorm)) *
        (1 - (fCurrentNorm)) /
        (fNormError * fNormError));
  }

  // Adjust the shape back to where it was.
  if (fIsShape and !FitPar::Config().GetParB("saveshapescaling")) {
    fMCHist->Scale(1. / scaleF);
    fMCFine->Scale(1. / scaleF);
  }

  fLikelihood = chi2;

  return chi2;
};
