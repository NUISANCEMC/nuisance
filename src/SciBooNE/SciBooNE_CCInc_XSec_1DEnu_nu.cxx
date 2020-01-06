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

#include "SciBooNE_CCInc_XSec_1DEnu_nu.h"

//********************************************************************
SciBooNE_CCInc_XSec_1DEnu_nu::SciBooNE_CCInc_XSec_1DEnu_nu(nuiskey samplekey) {
  //********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "SciBooNE_CCInc_XSec_1DEnu_nu sample. \n"
                        "Target: CH \n"
                        "Flux: SciBooNE Forward Horn Current\n"
                        "Signal: Any event with 1 muon \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("E_{#nu} (GeV)");
  fSettings.SetYTitle("#sigma(E_{#nu}) (cm^{2}/Nucleon)");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/FULL,DIAG/NORM/MASK", "FIX/FULL");
  fSettings.SetEnuRange(0.25, 8.0);
  fSettings.DefineAllowedTargets("C,H");
  fSettings.FoundFill("name", "NUANCE", use_nuance, true);

  // use_nuance plot information
  fSettings.SetTitle("SciBooNE_CCInc_XSec_1DEnu_nu");

  if (!use_nuance) {
    fSettings.SetDataInput(FitPar::GetDataBase() +
                           "SciBooNE/CCInc/ccinc_1denu_neut.txt");
    fSettings.SetCovarInput(FitPar::GetDataBase() +
                            "SciBooNE/CCInc/ccinc_1denu_neut_cov.txt");
  } else {
    fSettings.SetDataInput(FitPar::GetDataBase() +
                           "SciBooNE/CCInc/ccinc_1denu_nuance.txt");
    fSettings.SetCovarInput(FitPar::GetDataBase() +
                            "SciBooNE/CCInc/ccinc_1denu_nuance_cov.txt");
  }
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2
  fScaleFactor =
      GetEventHistogram()->Integral("width") * double(1E-38) / double(fNEvents);

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile(fSettings.GetDataInput());
  SetCorrelationFromTextFile(fSettings.GetCovarInput());
  SetShapeCovar();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

void SciBooNE_CCInc_XSec_1DEnu_nu::FillEventVariables(FitEvent *event) {
  if (isSignal(event)) {
    fXVar = event->GetNeutrinoIn()->fP.E()*1E-3;
  }
};

bool SciBooNE_CCInc_XSec_1DEnu_nu::isSignal(FitEvent *event) {
  return SignalDef::isCCINC(event, 14, EnuMin, EnuMax);
}
