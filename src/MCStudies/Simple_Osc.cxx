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

#include "Simple_Osc.h"

//********************************************************************
Simple_Osc::Simple_Osc(nuiskey samplekey) {
  //********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip =
      "Simple measurement class for doing fake data oscillation studies.\n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);

  fSettings.SetTitle("Osc Studies");
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("XXX");
  fSettings.SetYTitle("Number of events");
  fSettings.SetEnuRange(0.0, 50);
  fSettings.SetAllowedTypes("EVT/SHAPE/DIAG", "EVT/SHAPE/DIAG");
  fSettings.DefineAllowedTargets("*");
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width") / (fNEvents + 0.);

  // Plot Setup -------------------------------------------------------
  if (samplekey.Has("Target_Simple_Osc_File")) {
    SetDataFromRootFile(samplekey.GetS("Target_Simple_Osc_File"),
                        (fSettings.GetName() + "_MC").c_str());
  } else {
    fDataHist = new TH1D((fSettings.GetName() + "_data").c_str(),
                         fSettings.GetFullTitles().c_str(), 200, 0, 3);
  }
  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

void Simple_Osc::FillEventVariables(FitEvent *event) {
  if (event->NumFSParticle(13) == 0) {  // Final state particles
    return;
  }

  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;
  ThetaMu = Pnu.Vect().Angle(Pmu.Vect());
  fXVar = FitUtils::EnuQErec(Pmu, cos(ThetaMu), 34, true);

  return;
};

bool Simple_Osc::isSignal(FitEvent *event) {
  return SignalDef::isCCINC(event, 14, EnuMin, EnuMax);
}
