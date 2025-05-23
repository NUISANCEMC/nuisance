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

#include "K2K_NC1pi0_Evt_1Dppi0_nu.h"

// The constructor
K2K_NC1pi0_Evt_1Dppi0_nu::K2K_NC1pi0_Evt_1Dppi0_nu(nuiskey samplekey){
  
// Sample overview ---------------------------------------------------
  std::string descrip = "K2K_NC1pi0_Evt_1Dppi0_nu sample. \n" \
                        "Target: H20 \n" \
                        "Flux:  \n" \
                        "Signal:  \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("p_{#pi^{0}} (MeV/c)");
  fSettings.SetYTitle("Number of events");
  fSettings.SetAllowedTypes("EVT/SHAPE/DIAG", "EVT/SHAPE/DIAG/MASK");
  fSettings.SetEnuRange(0.0, 5.0);
  fSettings.DefineAllowedTargets("H,O");

  // plot information
  fSettings.SetTitle("K2K_NC1pi0_Evt_1Dppi0_nu");
  fSettings.DefineAllowedSpecies("numu");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "K2K/nc1pi0/ppi0.csv");

  FinaliseSampleSettings();

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetCovarFromDiagonal();

  // Scale for shape
  fScaleFactor = fDataHist->Integral()/double(fNEvents);

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

void K2K_NC1pi0_Evt_1Dppi0_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(111) == 0)
    return;

  TLorentzVector Ppi0 = event->GetHMFSParticle(111)->fP;
  double ppi0 = FitUtils::p(Ppi0)*1000.;
  fXVar = ppi0;

  return;
};

bool K2K_NC1pi0_Evt_1Dppi0_nu::isSignal(FitEvent *event) {
  return SignalDef::isNC1pi(event, 14, 111, EnuMin, EnuMax); // Check this signal definition
}
