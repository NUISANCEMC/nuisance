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

#include "BNL_CC1npip_Evt_1DWNmu_nu.h"

//********************************************************************
BNL_CC1npip_Evt_1DWNmu_nu::BNL_CC1npip_Evt_1DWNmu_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "BNL CC1npip Event Rate 1D W(N mu) nu sample. \n" \
                        "Target: D2 \n" \
                        "Flux:  \n" \
                        "Signal:  \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);

  fSettings.SetTitle("BNL #nu_mu CC1n#pi^{+}");
  fSettings.SetXTitle("W(N#mu) (GeV)");
  fSettings.SetYTitle("Number of events");
  fSettings.SetAllowedTypes("EVT/SHAPE/DIAG","EVT/SHAPE/DIAG");
  fSettings.DefineAllowedTargets("D,H");
  fSettings.DefineAllowedSpecies("numu");
  fSettings.SetEnuRange(0.0, 10);

  fSettings.SetDataInput(  FitPar::GetDataBase() + "/BNL/CC1pip_on_n/BNL_CC1npip_WNmu_per_0.05GeV.txt" );

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width") / (fNEvents + 0.) * 2. / 1.;

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetPoissonErrors();
  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};


void BNL_CC1npip_Evt_1DWNmu_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(2112) == 0 ||
      event->NumFSParticle(211) == 0 ||
      event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pn   = event->GetHMFSParticle(2112)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double hadMass = (Pn+Pmu).Mag()/1000.;

  fXVar = hadMass;

  return;
};

bool BNL_CC1npip_Evt_1DWNmu_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 211, 2112, EnuMin, EnuMax);
}

