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

#include "MINERvA_SignalDef.h"

#include "MINERvA_CC1pi0_XSec_1Dth_antinu.h"

// The constructor
MINERvA_CC1pi0_XSec_1Dth_antinu::MINERvA_CC1pi0_XSec_1Dth_antinu(std::string name, std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile) {

  fName = name;
  fPlotTitles = "; #theta_{#pi} (degrees); d#sigma/d#theta_{#pi} (cm^{2}/degrees/nucleon)";
  fUpdatedData = fName.find("2015") == std::string::npos;
  EnuMin = 1.5;
  EnuMax = 10;

  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  if (fUpdatedData){

    hadMassCut = 1800;
    fIsDiag = false;

    this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pi0/2016/anu-cc1pi0-xsec-pion-angle.csv");

    // Error is given as percentage of cross-section
    // Need to scale the bin error properly before we do correlation -> covariance conversion
    for (int i = 0; i < fDataHist->GetNbinsX()+1; i++) {
      fDataHist->SetBinError(i+1, fDataHist->GetBinContent(i+1)*fDataHist->GetBinError(i+1)/100.);
    }

    this->SetCovarMatrixFromCorrText(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pi0/2016/anu-cc1pi0-correlation-pion-angle.csv", fDataHist->GetNbinsX());

  } else {

    // Although the covariance is given for MINERvA CC1pi0 nubar from 2015, it doesn't Cholesky decompose, hinting at something bad
    // I've tried adding small numbers to the diagonal but it still didn't work and the chi2s are crazy
    fIsDiag = true;
    fNormError = 0.15;
    // No hadronic mass cut on old publication
    hadMassCut = 99999;

    this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pi0/2015/ccpi0_th.csv");
    this->SetupDefaultHist();

    // Adjust MINERvA data to flux correction; roughly a 11% normalisation increase in data
    // Please change when MINERvA releases new data!
    for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
      fDataHist->SetBinContent(i+1, fDataHist->GetBinContent(i+1)*1.11);
    }

    fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
    covar     = StatUtils::GetInvert(fFullCovar);
  } // end special treatment depending on release year

  this->SetupDefaultHist();


  fScaleFactor = GetEventHistogram()->Integral("width")*double(1E-38)/double(fNEvents)/TotalIntegratedFlux("width");
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
    th = (180./M_PI)*FitUtils::th(Pnu, Ppi0);

  fXVar = th;

  return;
};

//********************************************************************
bool MINERvA_CC1pi0_XSec_1Dth_antinu::isSignal(FitEvent *event) {
//********************************************************************
  return SignalDef::isCC1pi(event, -14, 111, EnuMin, EnuMax);
}
