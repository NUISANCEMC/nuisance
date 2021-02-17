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

#include "MINERvA_CC1pip_XSec_1DTpi_20deg_nu.h"

// The constructor
MINERvA_CC1pip_XSec_1DTpi_20deg_nu::MINERvA_CC1pip_XSec_1DTpi_20deg_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  fName = "MINERvA_CC1pip_XSec_1DTpi_20deg_nu";
  fPlotTitles = "; T_{#pi} (MeV); d#sigma/dT_{#pi} (cm^{2}/MeV/nucleon)";
  EnuMin = 1.5;
  EnuMax = 10;
  fIsDiag = false;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  if (this->fIsShape) {
    this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pip/ccpip_Tpi_20_shape.csv");
    this->SetCovarMatrixFromCorrText(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pip/ccpip_Tpi_20_cov_shape.csv", fDataHist->GetNbinsX());
  } else {
    this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pip/ccpip_Tpi_20.csv");
    this->SetCovarMatrixFromCorrText(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pip/ccpip_Tpi_20_cov.csv", fDataHist->GetNbinsX());
  }

  this->SetupDefaultHist();

  for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
    fDataHist->SetBinContent(i+1, fDataHist->GetBinContent(i+1)*1.11);
  }

  this->fScaleFactor = GetEventHistogram()->Integral("width")*double(1E-38)/double(fNEvents)/TotalIntegratedFlux("width");
};


void MINERvA_CC1pip_XSec_1DTpi_20deg_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(PhysConst::pdg_charged_pions) == 0 ||
      event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(PhysConst::pdg_charged_pions)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double hadMass = FitUtils::Wrec(Pnu, Pmu);
  double Tpi     = -999;

  if (hadMass > 100 && hadMass < 1400)
    Tpi = FitUtils::T(Ppip)*1000.;

  fXVar = Tpi;

  return;
};

//********************************************************************
bool MINERvA_CC1pip_XSec_1DTpi_20deg_nu::isSignal(FitEvent *event) {
//********************************************************************
  // Last true refers to that this is the restricted MINERvA phase space, in which only forward-going muons are accepted
  return SignalDef::isCC1pip_MINERvA(event, EnuMin, EnuMax, true);
}
