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
  Authors: Adrian Orea (v1 2017)
           Clarence Wret (v2 2018)
*/

#include "MINERvA_CC0pi_XSec_2D_nu.h"
#include "MINERvA_SignalDef.h"

//********************************************************************
void MINERvA_CC0pi_XSec_2D_nu::SetupDataSettings() {
  //********************************************************************
  // Set Distribution
  // See header file for enum and some descriptions
  std::string name = fSettings.GetS("name");
  // Have one distribution as of summer 2018
  if (!name.compare("MINERvA_CC0pi_XSec_2Dptpz_nu"))
    fDist = kPtPz;

  // Define what files to use from the dist
  std::string datafile = "";
  std::string corrfile = "";
  std::string titles = "";
  std::string distdescript = "";
  std::string histname = "";

  switch (fDist) {
  case (kPtPz):
    datafile = "MINERvA/CC0pi_2D/cov_ptpl_2D_qelike.root";
    corrfile = "MINERvA/CC0pi_2D/cov_ptpl_2D_qelike.root";
    titles = "MINERvA CC0#pi #nu_{#mu} p_{t} p_{z};p_{z} (GeV);p_{t} "
             "(GeV);d^{2}#sigma/dP_{t}dP_{z} (cm^{2}/GeV^{2}/nucleon)";
    distdescript = "MINERvA_CC0pi_XSec_2Dptpz_nu sample";
    histname = "pt_pl_cross_section";
    break;
  default:
    NUIS_ABORT("Unknown Analysis Distribution : " << name);
  }

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
  fSettings.SetDataInput(FitPar::GetDataBase() + datafile);
  fSettings.SetCovarInput(FitPar::GetDataBase() + corrfile);

  // Set the data file
  SetDataValues(fSettings.GetDataInput(), histname);
}

//********************************************************************
MINERvA_CC0pi_XSec_2D_nu::MINERvA_CC0pi_XSec_2D_nu(nuiskey samplekey) {
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

  TMatrixDSym *tempmat = StatUtils::GetCovarFromRootFile(
      fSettings.GetCovarInput(), "TotalCovariance");
  fFullCovar = tempmat;
  // Decomposition is stable for entries that aren't E-xxx
  double ScalingFactor = 1E38 * 1E38;
  (*fFullCovar) *= ScalingFactor;

  // Just check that the data error and covariance are the same
  for (int i = 0; i < fFullCovar->GetNrows(); ++i) {
    for (int j = 0; j < fFullCovar->GetNcols(); ++j) {
      // Get the global bin
      int xbin1, ybin1, zbin1;
      fDataHist->GetBinXYZ(i, xbin1, ybin1, zbin1);
      double xlo1 = fDataHist->GetXaxis()->GetBinLowEdge(xbin1);
      double xhi1 = fDataHist->GetXaxis()->GetBinLowEdge(xbin1 + 1);
      double ylo1 = fDataHist->GetYaxis()->GetBinLowEdge(ybin1);
      double yhi1 = fDataHist->GetYaxis()->GetBinLowEdge(ybin1 + 1);
      if (xlo1 < fDataHist->GetXaxis()->GetBinLowEdge(1) ||
          ylo1 < fDataHist->GetYaxis()->GetBinLowEdge(1) ||
          xhi1 > fDataHist->GetXaxis()->GetBinLowEdge(
                     fDataHist->GetXaxis()->GetNbins() + 1) ||
          yhi1 > fDataHist->GetYaxis()->GetBinLowEdge(
                     fDataHist->GetYaxis()->GetNbins() + 1))
        continue;
      double data_error = fDataHist->GetBinError(xbin1, ybin1);
      double cov_error = sqrt((*fFullCovar)(i, i) / ScalingFactor);
      if (fabs(data_error - cov_error) > 1E-5) {
        std::cerr << "Error on data is different to that of covariance"
                  << std::endl;
        NUIS_ERR(FTL, "Data error: " << data_error);
        NUIS_ERR(FTL, "Cov error: " << cov_error);
        NUIS_ERR(FTL, "Data/Cov: " << data_error / cov_error);
        NUIS_ERR(FTL, "Data-Cov: " << data_error - cov_error);
        NUIS_ERR(FTL, "For x: " << xlo1 << "-" << xhi1);
        NUIS_ABORT("For y: " << ylo1 << "-" << yhi1);
      }
    }
  }

  // Now can make the inverted covariance
  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);

  // Now scale back
  (*fFullCovar) *= 1.0 / ScalingFactor;
  (*covar) *= ScalingFactor;
  (*fDecomp) *= ScalingFactor;

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

//********************************************************************
void MINERvA_CC0pi_XSec_2D_nu::FillEventVariables(FitEvent *event) {
  //********************************************************************
  // Checking to see if there is a Muon
  if (event->NumFSParticle(13) == 0)
    return;

  // Get the muon kinematics
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;

  Double_t px = Pmu.X() / 1000;
  Double_t py = Pmu.Y() / 1000;
  Double_t pt = sqrt(px * px + py * py);

  // y-axis is transverse momentum for both measurements
  fYVar = pt;

  // Now we set the x-axis
  switch (fDist) {
  case kPtPz: {
    // Don't want to assume the event generators all have neutrino coming along
    // z pz is muon momentum projected onto the neutrino direction
    Double_t pz = Pmu.Vect().Dot(Pnu.Vect() * (1.0 / Pnu.Vect().Mag())) / 1000.;
    // Set Hist Variables
    fXVar = pz;
    break;
  }
  default:
    NUIS_ABORT("DIST NOT FOUND : " << fDist);
    break;
  }
};

//********************************************************************
bool MINERvA_CC0pi_XSec_2D_nu::isSignal(FitEvent *event) {
  //********************************************************************
  return SignalDef::isCC0pi_MINERvAPTPZ(event, 14, EnuMin, EnuMax);
};

//********************************************************************
// Custom likelihood calculator because binning of covariance matrix
double MINERvA_CC0pi_XSec_2D_nu::GetLikelihood() {
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
