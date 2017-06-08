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
MiniBooNE_CC1pip_XSec_2DQ2Enu_nu::MiniBooNE_CC1pip_XSec_2DQ2Enu_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MiniBooNE_CC1pip_XSec_2DQ2Enu_nu sample. \n" \
                        "Target: CH \n" \
                        "Flux: MiniBooNE Forward Horn Current nue + nuebar \n" \
                        "Signal: Any event with 1 electron, any nucleons, and no other FS particles \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("E_{#nu} (MeV); Q^{2} (MeV^{2}/c^{4})");
  fSettings.SetYTitle("Q^{2} (MeV^{2}/c^{4})");
  fSettings.SetZTitle(" d#sigma(E_{#nu})/dQ^{2} (cm^{2}/(MeV^{2}/c^{4})/CH_{2})");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG/NORM/MASK", "FIX/DIAG");
  fSettings.SetEnuRange(0.0, 3.0);
  fSettings.SetNormError(0.107);
  fSettings.DefineAllowedTargets("C,H");

  fSettings.SetTitle("MiniBooNE_CC1pip_XSec_2DQ2Enu_nu");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "/MiniBooNE/CC1pip/ccpipXSecs.root;QSQVENUXSec" );
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width") * double(1E-38) / double(fNEvents) * (14.08);

  // Plot Setup -------------------------------------------------------
  SetDataFromRootFile( fSettings.GetDataInput() );
  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};

//********************************************************************
void MiniBooNE_CC1pip_XSec_2DQ2Enu_nu::FillEventVariables(FitEvent *event) {
//********************************************************************

  if (event->NumFSParticle(211) == 0 ||
      event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double Enu = FitUtils::EnuCC1piprec(Pnu, Pmu, Ppip) * 1000.;
  double Q2 = FitUtils::Q2CC1piprec(Pnu, Pmu, Ppip) * 1E6;

  fXVar = Enu;
  fYVar = Q2;

  return;
};

//********************************************************************
bool MiniBooNE_CC1pip_XSec_2DQ2Enu_nu::isSignal(FitEvent *event) {
//********************************************************************
  return SignalDef::isCC1pi(event, 14, 211, EnuMin, EnuMax);
}
