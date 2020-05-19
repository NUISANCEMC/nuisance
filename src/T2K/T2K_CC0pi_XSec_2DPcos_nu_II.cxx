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

#include "T2K_CC0pi_XSec_2DPcos_nu_II.h"

//********************************************************************
T2K_CC0pi_XSec_2DPcos_nu_II::T2K_CC0pi_XSec_2DPcos_nu_II(nuiskey samplekey) {
  //********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "T2K_CC0pi_XSec_2DPcos_nu_II sample. \n"
                        "Target: CH \n"
                        "Flux: T2K 2.5 degree off-axis (ND280)  \n"
                        "Signal: CC0pi with p_mu > 200 MeV\n"
                        "                   cth_mu > 0 \n"
                        "https://journals.aps.org/prd/abstract/10.1103/"
                        "PhysRevD.93.112012 Analysis II";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("P_{#mu} (GeV)");
  fSettings.SetYTitle("cos#theta_{#mu}");
  fSettings.SetZTitle("d^{2}#sigma/dP_{#mu}dcos#theta_{#mu} (cm^{2}/GeV)");
  fSettings.SetAllowedTypes("DIAG,FULL/FREE,SHAPE,FIX/SYSTCOV/STATCOV", "FIX");
  fSettings.SetEnuRange(0.0, 10.0);
  fSettings.DefineAllowedTargets("C,H");

  // CCQELike plot information
  fSettings.SetTitle("T2K_CC0pi_XSec_2DPcos_nu_II");
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

bool T2K_CC0pi_XSec_2DPcos_nu_II::isSignal(FitEvent *event) {
  return SignalDef::isT2K_CC0pi(event, EnuMin, EnuMax, SignalDef::kAnalysis_II);
};

void T2K_CC0pi_XSec_2DPcos_nu_II::FillEventVariables(FitEvent *event) {

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

void T2K_CC0pi_XSec_2DPcos_nu_II::SetHistograms() {

  fIsSystCov = fSettings.GetS("type").find("SYSTCOV") != std::string::npos;
  fIsStatCov = fSettings.GetS("type").find("STATCOV") != std::string::npos;
  fIsNormCov = fSettings.GetS("type").find("NORMCOV") != std::string::npos;
  fNDataPointsX = 12;
  fNDataPointsY = 10;

  // Open file
  std::string infile =
      FitPar::GetDataBase() + "/T2K/CC0pi/T2K_CC0PI_2DPmuCosmu_Data.root";
  TFile *rootfile = new TFile(infile.c_str(), "READ");
  TH2D *tempcov = NULL;

  // Get Data
  fDataHist = (TH2D *)rootfile->Get("analysis2_data");
  fDataHist->SetDirectory(0);
  fDataHist->SetNameTitle((fName + "_data").c_str(),
                          (fName + "_data" + fPlotTitles).c_str());
  // For some reason the error on the data in the data release is 1E-20
  // That is wrong, so set it to zero here
  for (int i = 0; i < fDataHist->GetXaxis()->GetNbins() + 1; ++i) {
    for (int j = 0; j < fDataHist->GetYaxis()->GetNbins() + 1; ++j) {
      fDataHist->SetBinError(i + 1, j + 1, 0.0);
    }
  }

  // Get Syst/Stat Covar
  TH2D *tempsyst = 0;
  rootfile->GetObject("analysis2_systcov", tempsyst);
  TH2D *tempstat = 0;
  rootfile->GetObject("analysis2_statcov", tempstat);
  TH2D *tempnorm = 0;
  rootfile->GetObject("analysis2_normcov", tempnorm);

  if (!tempsyst) {
    NUIS_ABORT("tempsyst not found");
  }

  // Create covar [Default is both]
  tempcov = (TH2D *)tempsyst->Clone();
  tempcov->Reset();

  if (fIsSystCov) {
    tempcov->Add(tempsyst);
  }
  if (fIsStatCov) {
    tempcov->Add(tempstat);
  }
  if (fIsNormCov) {
    tempcov->Add(tempnorm);
  }

  // if nothing is set, add them all!
  if (!fIsSystCov && !fIsStatCov && !fIsNormCov) {
    tempcov->Add(tempsyst);
    tempcov->Add(tempstat);
    tempcov->Add(tempnorm);
  }

  // Get Map
  TH2I *InputMap = (TH2I *)rootfile->Get("analysis2_map");

  // This map doesn't skip over the 0 data/covar bins
  fMapHist = (TH2I *)InputMap->Clone();
  fMapHist->SetDirectory(NULL);
  fMapHist->SetNameTitle((fName + "_map").c_str(),
                         (fName + "_map" + fPlotTitles).c_str());

  int binit = 1;
  for (int j = 0; j < fDataHist->GetYaxis()->GetNbins(); ++j) {
    for (int i = 0; i < fDataHist->GetXaxis()->GetNbins(); ++i) {
      int covarhistbin = InputMap->GetBinContent(i + 1, j + 1);

      std::cout << "i: " << i << ", j: " << j
                << " -> covarhistbin: " << covarhistbin << " = "
                << tempcov->GetBinContent(covarhistbin, covarhistbin)
                << std::endl;
      if (tempcov->GetBinContent(covarhistbin, covarhistbin) > 0) {
        fMapHist->SetBinContent(i + 1, j + 1, binit++);
      } else {
        fMapHist->SetBinContent(i + 1, j + 1, 0);
      }
    }
  }

  int nbins = fMapHist->GetMaximum();
  fFullCovar = new TMatrixDSym(nbins);

  for (int j_1 = 0; j_1 < fDataHist->GetYaxis()->GetNbins(); ++j_1) {
    for (int i_1 = 0; i_1 < fDataHist->GetXaxis()->GetNbins(); ++i_1) {

      for (int j_2 = 0; j_2 < fDataHist->GetYaxis()->GetNbins(); ++j_2) {
        for (int i_2 = 0; i_2 < fDataHist->GetXaxis()->GetNbins(); ++i_2) {

          int i = fMapHist->GetBinContent(i_1 + 1, j_1 + 1);
          int j = fMapHist->GetBinContent(i_2 + 1, j_2 + 1);
          if (i == 0 || j == 0) {
            continue;
          }

          // get the relevant tempcov bin
          int i_input = InputMap->GetBinContent(i_1 + 1, j_1 + 1);
          int j_input = InputMap->GetBinContent(i_2 + 1, j_2 + 1);

          std::cout << "i: " << i << ", j: " << j << "i_input: " << i_input
                    << "j_input: " << j_input
                    << ", covar = " << tempcov->GetBinContent(i_input, j_input)
                    << std::endl;

          // Correct for them being 1-offset;
          i -= 1;
          j -= 1;

          (*fFullCovar)(i, j) = tempcov->GetBinContent(i_input, j_input);

          if ((i == j) && ((*fFullCovar)(i, j) <= 0)) {
            NUIS_ABORT("Input covariance had a 0/negative diagonal element: "
                       << i << ", " << j << ", " << (*fFullCovar)(i, j));
          }
        }
      }
    }
  }

  TFile *fout= new TFile("mattest.root","RECREATE");
  fout->WriteTObject(fFullCovar,"covar");
  fout->Write();
  fout->Close();
  // Setup Covar

  covar = StatUtils::GetInvert(fFullCovar);

  for (int i = 0; i < nbins; i++) {
    for (int j = 0; j < nbins; j++) {
      if ((i == j) && ((*covar)(i, j) <= 0)) {
        NUIS_ABORT("Inverse covariance had a 0/negative diagonal element: "
                   << i << ", " << j << ", " << (*covar)(i, j));
      }
    }
  }

  fDecomp = StatUtils::GetDecomp(covar);
  SetShapeCovar();

  // Set Data Errors
  StatUtils::SetDataErrorFromCov(fDataHist, fFullCovar, fMapHist, 1E-38);

  // Remove root file
  rootfile->Close();
};
