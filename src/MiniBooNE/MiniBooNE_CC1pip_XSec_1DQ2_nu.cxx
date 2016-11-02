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

#include "MiniBooNE_CC1pip_XSec_1DQ2_nu.h"

//********************************************************************
/// @brief MiniBooNE CC1pi+ numu 1DQ2 Measurement on CH2 (Ref: - )
///
// The constructor
MiniBooNE_CC1pip_XSec_1DQ2_nu::MiniBooNE_CC1pip_XSec_1DQ2_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){

  fName = "MiniBooNE_CC1pip_XSec_1DQ2_nu";
  fPlotTitles = "; Q^{2}_{CC#pi} (GeV^{2}); d#sigma/dQ_{CC#pi^{+}}^{2} (cm^{2}/MeV^{2}/CH_{2})";
  EnuMin = 0.5;
  EnuMax = 2.;
  fIsDiag = true;
  fNormError = 0.107;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/MiniBooNE/CC1pip/ccpipXSec_Q2.txt");
  this->SetupDefaultHist();

  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar     = StatUtils::GetInvert(fFullCovar);

  // Calculates a flux averaged cross-section from (Evt("width")/Flux("width")) * 14.08/6.0
  this->fScaleFactor = this->fEventHist->Integral("width")*double(1E-38)/double(fNEvents)*(14.08)/TotalIntegratedFlux("width")/1E6;
  // Added /1E6. comes from Q2 being in MeV^2, not GeV^2 I think... Or maybe the units in the paper are simply wrong; 1E-45 is very small! :D
};

//********************************************************************
void  MiniBooNE_CC1pip_XSec_1DQ2_nu::FillEventVariables(FitEvent *event){
//********************************************************************

  if (event->NumFSParticle(211) == 0 ||
      event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  // No W cut on MiniBooNE CC1pi+
  double Q2CC1pip = FitUtils::Q2CC1piprec(Pnu, Pmu, Ppip);

  fXVar = Q2CC1pip;

  return;
};

//********************************************************************
bool MiniBooNE_CC1pip_XSec_1DQ2_nu::isSignal(FitEvent *event) {
//********************************************************************
  return SignalDef::isCC1pi(event, 14, 211, EnuMin, EnuMax);
}
