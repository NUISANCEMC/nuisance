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

#include "MiniBooNE_CC1pi0_XSec_1Dcosmu_nu.h"

// The constructor
MiniBooNE_CC1pi0_XSec_1Dcosmu_nu::MiniBooNE_CC1pi0_XSec_1Dcosmu_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){

  fName = "MiniBooNE_CC1pi0_XSec_1Dcosmu_nu";
  fPlotTitles = "; cos#theta_{#mu}; d#sigma/dcos#theta_{#mu} (cm^{2}/CH_{2})";
  fIsDiag = false;
  fNormError = 0.107;
  EnuMin = 0.5;
  EnuMax = 2.0;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/MiniBooNE/CC1pi0/dxsecdcosmu_edit.txt");
  this->SetCovarMatrixFromCorrText(GeneralUtils::GetTopLevelDir()+"/data/MiniBooNE/CC1pi0/dxsecdcosmu_covar.txt", this->fNDataPointsX);
  this->SetupDefaultHist();

  // Calculates a flux averaged cross-section from (Evt("width")/Flux("width")) * 14.08/6.0
  this->fScaleFactor = GetEventHistogram()->Integral("width")*double(1E-38)/double(fNEvents)*(14.08)/TotalIntegratedFlux("width");
};

void MiniBooNE_CC1pi0_XSec_1Dcosmu_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(111) == 0 ||
      event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppi0 = event->GetHMFSParticle(111)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  // No W cut on MiniBooNE CC1pi+
  double CosMu = cos(FitUtils::th(Pnu, Pmu));

  fXVar = CosMu;

  return;
};

//********************************************************************
bool MiniBooNE_CC1pi0_XSec_1Dcosmu_nu::isSignal(FitEvent *event) {
//********************************************************************
  return SignalDef::isCC1pi(event, 14, 111, EnuMin, EnuMax);
}
