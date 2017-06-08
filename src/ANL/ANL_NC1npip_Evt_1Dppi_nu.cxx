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
#include "ANL_NC1npip_Evt_1Dppi_nu.h"


/**
  * M. Derrick et al., "Study of single-pion production by weak neutral currents in low-energy \nu d interactions", Physical Review D, Volume 23, Number 3, 569, 1 February 1981
*/
//********************************************************************
ANL_NC1npip_Evt_1Dppi_nu::ANL_NC1npip_Evt_1Dppi_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "ANL_NC1npip_Evt_1Dppi_nu sample. \n" \
                        "Target: D2 \n" \
                        "Flux:  \n" \
                        "Signal:  \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("p_{#pi} (MeV)");
  fSettings.SetYTitle("Number of events");
  fSettings.SetAllowedTypes("EVT/SHAPE/DIAG", "EVT/SHAPE/DIAG");
  fSettings.SetEnuRange(0.0, 100.0); // No Enu Range given in v1r0p1
  fSettings.DefineAllowedTargets("D,H");

  // CCQELike plot information
  fSettings.SetTitle("ANL #nu_mu NC1n#pi^{+}");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "/ANL/NC1npip/ANL_ppi_NC1npip_weight.csv" );
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  this->fScaleFactor = GetEventHistogram()->Integral("width")/((fNEvents+0.)*GetFluxHistogram()->Integral("width"))*(16./8.);

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetPoissonErrors();
  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};

void ANL_NC1npip_Evt_1Dppi_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(2112) == 0 || event->NumFSParticle(211) == 0) return;

  TLorentzVector Pn   = event->GetHMFSParticle(2112)->fP;;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;;

  double hadMass = FitUtils::MpPi(Pn, Ppip);
  double ppip    = -1.0;

  // ANL has a M(pi, p) < 1.4 GeV cut imposed
  if (hadMass < 1400) ppip = FitUtils::p(Ppip)*1000.;

  fXVar = ppip;

  return;
};

bool ANL_NC1npip_Evt_1Dppi_nu::isSignal(FitEvent *event) {
  return SignalDef::isNC1pi3Prong(event, 14, 211, 2112, EnuMin, EnuMax);
}

