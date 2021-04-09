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

#include "MiniBooNE_CC1pip_XSec_1DQ2_nu.h"


//********************************************************************
MiniBooNE_CC1pip_XSec_1DQ2_nu::MiniBooNE_CC1pip_XSec_1DQ2_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MiniBooNE_CC1pip_XSec_1DQ2_nu sample. \n" \
                        "Target: CH \n" \
                        "Flux: MiniBooNE Forward Horn Current nue + nuebar \n" \
                        "Signal: Any event with 1 muon, any nucleons, and no other FS particles \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("Q^{2}_{CC#pi^{+}} (GeV^{2})");
  fSettings.SetYTitle("d#sigma/dQ_{CC#pi^{+}}^{2} (cm^{2}/GeV^{2}/CH_{2})");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG/NORM/MASK", "FIX/DIAG");
  fSettings.SetEnuRange(0.0, 100.0); // No energy range given in v1r0
  fSettings.DefineAllowedTargets("C,H");

  // CCQELike plot information
  fSettings.SetTitle("MiniBooNE CC1pi");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "MiniBooNE/CC1pip/ccpipXSec_Q2.txt" );
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width")*double(1E-38)/double(fNEvents)*(14.08)/TotalIntegratedFlux("width")/1E6;
  // Added /1E6. comes from Q2 being in MeV^2, not GeV^2 I think... Or maybe the units in the paper are simply wrong; 1E-45 is very small! :D

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};

//********************************************************************
void  MiniBooNE_CC1pip_XSec_1DQ2_nu::FillEventVariables(FitEvent *event){
//********************************************************************

  if (event->NumFSParticle(13) == 0) return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  // No W cut on MiniBooNE CC1pi+
  double q2 = -1*(Pnu-Pmu).Mag2()/1e6;

  fXVar = q2;

  return;
};

//********************************************************************
bool MiniBooNE_CC1pip_XSec_1DQ2_nu::isSignal(FitEvent *event) {
//********************************************************************
  return SignalDef::isCC1pi(event, 14, 211, EnuMin, EnuMax);
}
