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

#include "BNL_CC2pi_1pim1pip_Evt_1DWpippim_nu.h"

//********************************************************************
BNL_CC2pi_1pim1pip_Evt_1DWpippim_nu::BNL_CC2pi_1pim1pip_Evt_1DWpippim_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "BNL_CC2pi_1pim1pip_Evt_1DWpippim_nu sample. \n" \
                        "Target: D2 \n" \
                        "Flux:  \n" \
                        "Signal:  \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("W(#pi^{+}#pi^{-}) (GeV)");
  fSettings.SetYTitle("Number of events");
  fSettings.SetAllowedTypes("EVT/SHAPE/DIAG","EVT/SHAPE/DIAG");
  fSettings.SetEnuRange(0.0, 15.0);
  fSettings.DefineAllowedTargets("D,H");

  // CCQELike plot information
  fSettings.SetTitle("BNL #nu_mu CC2#pi");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "/BNL/CCmultipi/BNL_CC1pim1pip1p_Wpimpip_per_0.05GeV.txt" );
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width")/double(fNEvents) * 2.0 / 1.0 ;

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetPoissonErrors();
  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};


void BNL_CC2pi_1pim1pip_Evt_1DWpippim_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(211) == 0 ||
      event->NumFSParticle(-211) == 0)
    return;

  TLorentzVector Ppim  = event->GetHMFSParticle(-211)->fP;
  TLorentzVector Ppip  = event->GetHMFSParticle(211)->fP;
  fXVar = (Ppim+Ppip).Mag()/1000;
  return;
}

// Signal asks for 1pi-, 1pi+, 1mu-, 1p
bool BNL_CC2pi_1pim1pip_Evt_1DWpippim_nu::isSignal(FitEvent *event) {
  int pdgs[] = {13, 211, -211, 2212};
  return SignalDef::isCCWithFS(event, 14, pdgs,
                               EnuMin, EnuMax);
}


