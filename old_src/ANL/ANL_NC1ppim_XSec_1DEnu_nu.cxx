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
#include "ANL_NC1ppim_XSec_1DEnu_nu.h"

/**
  * M. Derrick et al., "Study of the reaction \nu n \rightarrow \nu p \pi^-", Physics Letters, Volume 92B, Number 3,4, 363, 19 May 1980
*/

//********************************************************************
ANL_NC1ppim_XSec_1DEnu_nu::ANL_NC1ppim_XSec_1DEnu_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "ANL_NC1ppim_XSec_1DEnu_nu sample. \n" \
                        "Target: D2 \n" \
                        "Flux:  \n" \
                        "Signal:  \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("E_{#nu} (GeV)");
  fSettings.SetYTitle("#sigma(E_{#nu}) (cm^{2}/nucleon)");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG", "FIX/DIAG");
  fSettings.SetEnuRange(0.3, 1.5);
  fSettings.DefineAllowedTargets("D,H");

  // CCQELike plot information
  fSettings.SetTitle("ANL #nu_mu NC1n#pi^{+}");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "/ANL/NC1ppim/ANL_NC1ppim_Enu_xsec.csv" );
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width") * 1E-38 /(fNEvents+0)*(16./8.);

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};

void ANL_NC1ppim_XSec_1DEnu_nu::FillEventVariables(FitEvent *event) {

  // Very simple here!
  double Enu = event->GetNeutrinoIn()->fP.E()/1000.;
  this->fXVar = Enu;

  return;
};

bool ANL_NC1ppim_XSec_1DEnu_nu::isSignal(FitEvent *event) {
  return SignalDef::isNC1pi3Prong(event, 14, -211, 2212, EnuMin, EnuMax);
}

