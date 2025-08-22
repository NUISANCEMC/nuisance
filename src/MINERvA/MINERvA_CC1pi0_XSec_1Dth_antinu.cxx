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

#include "MINERvA_SignalDef.h"

#include "MINERvA_CC1pi0_XSec_1Dth_antinu.h"


//********************************************************************
MINERvA_CC1pi0_XSec_1Dth_antinu::MINERvA_CC1pi0_XSec_1Dth_antinu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MINERvA_CC1pi0_XSec_1Dth_antinu sample. \n" \
                        "Target: CH \n" \
                        "Flux: MINERvA Forward Horn Current numubar \n" \
                        "Signal: Any event with 1 muon, 1 pion, no other tracks \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("#theta_{#pi} (degrees)");
  fSettings.SetYTitle("d#sigma/d#theta_{#pi} (cm^{2}/nucleon/degree)");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/FULL");
  fSettings.SetEnuRange(1.5, 10.0);
  fSettings.DefineAllowedTargets("C,H");
  // Default to updated data unless specified
  fUpdatedData = !fSettings.Found("name", "2015");
  fFluxCorrection = fSettings.Found("name","fluxcorr");
  
  // CCQELike plot information
  fSettings.SetTitle("MINERvA_CC1pi0_XSec_1Dth_antinu");
  fSettings.DefineAllowedSpecies("numubar");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width") * double(1E-38) / double(fNEvents) / TotalIntegratedFlux("width");

  // Plot Setup -------------------------------------------------------
  if (fUpdatedData) {

    // Updated data has a hadronic mass cut of 1.8 GeV/c2
    hadMassCut = 1800;
    fIsDiag = false;

    SetDataFromTextFile(GeneralUtils::GetTopLevelDir() + "/data/MINERvA/CC1pi0/2016/anu-cc1pi0-xsec-pion-angle.csv");

    // Error is given as percentage of cross-section
    // Need to scale the bin error properly before we do correlation -> covariance conversion
    for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
      fDataHist->SetBinError(i + 1, fDataHist->GetBinContent(i + 1)*fDataHist->GetBinError(i + 1) / 100.);
    }

    SetCorrelationFromTextFile(GeneralUtils::GetTopLevelDir() + "/data/MINERvA/CC1pi0/2016/anu-cc1pi0-correlation-pion-angle.csv");
    SetShapeCovar();
  } else {
    // Although the covariance is given for MINERvA CC1pi0 nubar from 2015, it doesn't Cholesky decompose, hinting at something bad
    // I've tried adding small numbers to the diagonal but it still didn't work and the chi2s are crazy
    fIsDiag = true;
    fNormError = 0.15;
    // No hadronic mass cut on old publication
    hadMassCut = 99999;

    SetDataFromTextFile(GeneralUtils::GetTopLevelDir() + "/data/MINERvA/CC1pi0/2015/ccpi0_th.csv");
    SetCovarFromDiagonal();
  } // end special treatment depending on release year


  if (fFluxCorrection) {
    for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
      fDataHist->SetBinContent(i + 1, fDataHist->GetBinContent(i + 1) * 1.11);
    }

  }

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};



void MINERvA_CC1pi0_XSec_1Dth_antinu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(111) == 0 ||
      event->NumFSParticle(-13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppi0 = event->GetHMFSParticle(111)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(-13)->fP;

  double hadMass = FitUtils::Wrec(Pnu, Pmu);
  double th      = -999;

  if (hadMass < hadMassCut)
    th = (180. / M_PI) * FitUtils::th(Pnu, Ppi0);

  fXVar = th;

  return;
};

//********************************************************************
bool MINERvA_CC1pi0_XSec_1Dth_antinu::isSignal(FitEvent *event) {
//********************************************************************
  if (fUpdatedData) {
    return SignalDef::isCC1pi0_MINERvA_2016(event, EnuMin, EnuMax);
  } else {
    return SignalDef::isCC1pi0_MINERvA_2015(event, EnuMin, EnuMax);
  }
}
