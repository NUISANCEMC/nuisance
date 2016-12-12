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

#include "MINERvA_CC1pi0_XSec_1DTpi0_antinu.h"

// The constructor
MINERvA_CC1pi0_XSec_1DTpi0_antinu::MINERvA_CC1pi0_XSec_1DTpi0_antinu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  fName = "MINERvA_CC1pi0_XSec_1DTpi0_antinu_2016";
  fPlotTitles = "; T_{#pi} (GeV); d#sigma/dT_{#pi} (cm^{2}/GeV/nucleon)";
  hadMassCut = 1800;
  fIsDiag = false;
  EnuMin = 1.5;
  EnuMax = 10;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pi0/2016/anu-cc1pi0-xsec-pion-kinetic-energy.csv");

  // Error is given as percentage of cross-section
  // Need to scale the bin error properly before we do correlation -> covariance conversion
  for (int i = 0; i < fDataHist->GetNbinsX()+1; i++) {
    fDataHist->SetBinError(i+1, fDataHist->GetBinContent(i+1)*fDataHist->GetBinError(i+1)/100.);
  }

  this->SetCovarMatrixFromCorrText(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pi0/2016/anu-cc1pi0-correlation-pion-kinetic-energy.csv", fDataHist->GetNbinsX());

  this->SetupDefaultHist();

  this->fScaleFactor = this->fEventHist->Integral("width")*double(1E-38)/double(fNEvents)/TotalIntegratedFlux("width");
};

void MINERvA_CC1pi0_XSec_1DTpi0_antinu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(111) == 0 ||
      event->NumFSParticle(-13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppi0 = event->GetHMFSParticle(111)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(-13)->fP;

  double hadMass = FitUtils::Wrec(Pnu, Pmu);
  double Tpi0 = -999;

  if (hadMass > 100 && hadMass < hadMassCut) {
      Tpi0 = FitUtils::T(Ppi0);
  }

  fXVar = Tpi0;

  return;
};

//********************************************************************
bool MINERvA_CC1pi0_XSec_1DTpi0_antinu::isSignal(FitEvent *event) {
//********************************************************************
  return SignalDef::isCC1pi(event, -14, 111, EnuMin, EnuMax);
}
