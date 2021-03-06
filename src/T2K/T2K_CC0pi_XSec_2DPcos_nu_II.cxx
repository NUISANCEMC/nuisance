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
  fSettings.SetXTitle("p_{#mu} (GeV)");
  fSettings.SetYTitle("cos#theta_{#mu}");
  fSettings.SetZTitle("d^{2}#sigma/dp_{#mu}dcos#theta_{#mu} (cm^{2}/GeV)");
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

  fNDataPointsX = 12;
  fNDataPointsY = 10;

  // Open file
  std::string infile =
      FitPar::GetDataBase() + "/T2K/CC0pi/T2K_CC0PI_2DPmuCosmu_Data.root";
  TFile *rootfile = new TFile(infile.c_str(), "READ");
  TH2D *tempcov = (TH2D *)rootfile->Get("analysis2_totcov");;

  // Get Data
  fDataHist = (TH2D *)rootfile->Get("analysis2_data");
  fDataHist->SetDirectory(0);
  fDataHist->SetNameTitle((fName + "_data").c_str(),
                          (fName + "_data" + fSettings.GetFullTitles()).c_str());
  // For some reason the error on the data in the data release is 1E-20
  // That is wrong, so set it to zero here
  for (int i = 0; i < fDataHist->GetXaxis()->GetNbins() + 1; ++i) {
    for (int j = 0; j < fDataHist->GetYaxis()->GetNbins() + 1; ++j) {
      fDataHist->SetBinError(i + 1, j + 1, 0.0);
    }
  }

  // Get Map
  TH2I *InputMap = (TH2I *)rootfile->Get("analysis2_map");

  // This map doesn't skip over the 0 data/covar bins
  fMapHist = (TH2I *)InputMap->Clone();
  fMapHist->SetDirectory(NULL);
  fMapHist->SetNameTitle((fName + "_map").c_str(),
                         (fName + "_map" + fPlotTitles).c_str());

  int nbins = fMapHist->GetMaximum();
  fFullCovar = new TMatrixDSym(nbins);

  for (int i=0; i< nbins; ++i){
    for (int j=0;j< nbins; ++j){
      (*fFullCovar)(i, j) = tempcov->GetBinContent(i+1, j+1);
    }
  }

  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(covar);
  SetShapeCovar();

  // Set Data Errors
  StatUtils::SetDataErrorFromCov(fDataHist, fFullCovar, fMapHist, 1E-38);

  // Remove root file
  rootfile->Close();
};
