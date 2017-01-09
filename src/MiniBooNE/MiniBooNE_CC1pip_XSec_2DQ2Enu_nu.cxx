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

#include "MiniBooNE_CC1pip_XSec_2DQ2Enu_nu.h"

//********************************************************************
MiniBooNE_CC1pip_XSec_2DQ2Enu_nu::MiniBooNE_CC1pip_XSec_2DQ2Enu_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){
//********************************************************************

  fName = "MiniBooNE_CC1pip_XSec_2DQ2Enu_nu";
  fPlotTitles = "; E_{#nu} (MeV); Q^{2} (MeV^{2}/c^{4}); d#sigma(E_{#nu})/dQ^{2} (cm^{2}/(MeV^{2}/c^{4})/CH_{2})";
  fIsDiag = true;
  fNormError = 0.107;
  Measurement2D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/MiniBooNE/CC1pip/ccpipXSecs.root", std::string("QSQVENUXSec"));//data comes in .root file, yes!
  this->SetupDefaultHist();

  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar     = StatUtils::GetInvert(fFullCovar);

  // Calculates a flux averaged cross-section from (Evt("width")/Flux("width")) * 14.08/6.0
  this->fScaleFactor = GetEventHistogram()->Integral("width")*double(1E-38)/double(fNEvents)*(14.08);

};

void MiniBooNE_CC1pip_XSec_2DQ2Enu_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(211) == 0 ||
      event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double Enu = FitUtils::EnuCC1piprec(Pnu, Pmu, Ppip)*1000.;
  double Q2 = FitUtils::Q2CC1piprec(Pnu, Pmu, Ppip)*1E6;

  fXVar = Enu;
  fYVar = Q2;

  return;
};

//********************************************************************
bool MiniBooNE_CC1pip_XSec_2DQ2Enu_nu::isSignal(FitEvent *event) {
//********************************************************************
  return SignalDef::isCC1pi(event, 14, 211, EnuMin, EnuMax);
}
