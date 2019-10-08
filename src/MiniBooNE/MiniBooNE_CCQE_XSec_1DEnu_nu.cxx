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

#include "MiniBooNE_CCQE_XSec_1DEnu_nu.h"

//********************************************************************
MiniBooNE_CCQE_XSec_1DEnu_nu::MiniBooNE_CCQE_XSec_1DEnu_nu(nuiskey samplekey) {
  //********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MiniBooNE_CCQE_XSec_1DEnu_nu sample. \n"
                        "Target: CH \n"
                        "Flux: MiniBooNE Forward Horn Current nue + nuebar \n"
                        "Signal: Any event with 1 muon, any nucleons, and no "
                        "other FS particles \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("E_{#nu} (GeV)");
  fSettings.SetYTitle("#sigma(E_{#nu}) (cm^{2}/CH_{2})");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/FULL,DIAG/NORM/MASK", "FIX/FULL");
  fSettings.SetEnuRange(0.5, 2.0);
  fSettings.DefineAllowedTargets("C,H");

  // CCQELike plot information
  fSettings.SetTitle("MiniBooNE_CCQE_XSec_1DEnu_nu");
  fSettings.SetDataInput(FitPar::GetDataBase() +
                         "MiniBooNE/CCQE/totalxsec_edit.txt");
  fSettings.SetCovarInput(FitPar::GetDataBase() +
                          "MiniBooNE/CCQE/totalxsec_covar.txt");
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width") * double(1E-38) /
                 double(fNEvents) * (14.08);

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile(fSettings.GetDataInput());
  SetCorrelationFromTextFile(fSettings.GetCovarInput());
  SetShapeCovar();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

void MiniBooNE_CCQE_XSec_1DEnu_nu::FillEventVariables(FitEvent *event) {

  if (SignalDef::isCCQELike(event, 14, EnuMin, EnuMax)) {
    fXVar = event->GetNeutrinoIn()->fP.E();
  }

};

bool MiniBooNE_CCQE_XSec_1DEnu_nu::isSignal(FitEvent *event) {
  return SignalDef::isCCQELike(event, 14, EnuMin, EnuMax);
}
