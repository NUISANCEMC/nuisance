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
#include "ANL_NC1ppim_Evt_1DcosmuStar_nu.h"

/**
  * M. Derrick et al., "Study of the reaction \nu n \rightarrow \nu p \pi^-", Physics Letters, Volume 92B, Number 3,4, 363, 19 May 1980
*/


//********************************************************************
ANL_NC1ppim_Evt_1DcosmuStar_nu::ANL_NC1ppim_Evt_1DcosmuStar_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "ANL_NC1ppim_Evt_1DcosmuStar_nu sample. \n" \
                        "Target: D2 \n" \
                        "Flux:  \n" \
                        "Signal:  \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("cos#theta*_{#mu}");
  fSettings.SetYTitle("Number of events");
  fSettings.SetAllowedTypes("EVT/SHAPE/DIAG", "EVT/SHAPE/DIAG");
  fSettings.SetEnuRange(0.3, 1.5);
  fSettings.DefineAllowedTargets("D,H");

  // CCQELike plot information
  fSettings.SetTitle("ANL #nu_mu CC1n#pi^{+}");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "/ANL/NC1ppim/ANL_NC1ppim_cosMuStar.csv" );
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width")/((fNEvents+0.)*GetFluxHistogram()->Integral("width"))*(2./1.);

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetPoissonErrors();
  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};


void ANL_NC1ppim_Evt_1DcosmuStar_nu::FillEventVariables(FitEvent *event) {

  if (event->NumISParticle(2112) == 0 || event->NumFSParticle(2212) == 0 || event->NumFSParticle(-211) == 0 || event->NumFSParticle(14) == 0) return;

  TLorentzVector Pnu    = event->GetNeutrinoIn()->fP;
  TLorentzVector Pin    = event->GetHMISParticle(2112)->fP;
  TLorentzVector Pp     = event->GetHMFSParticle(2212)->fP;
  TLorentzVector Ppim   = event->GetHMFSParticle(-211)->fP;
  TLorentzVector PnuOut = event->GetHMFSParticle(14)->fP;

  // Boost into centre of mass frame
  TLorentzVector CMS = Pnu + Pin;
  // Boost outgoing neutrino backwards CMS
  PnuOut.Boost(-CMS.BoostVector());
  // Boost incoming neutrino forwards by CMS
  Pnu.Boost(CMS.BoostVector());

  double cosmuStar = cos(FitUtils::th(PnuOut, Pnu));

  this->fXVar = cosmuStar;

  return;
};

bool ANL_NC1ppim_Evt_1DcosmuStar_nu::isSignal(FitEvent *event) {
  return SignalDef::isNC1pi3Prong(event, 14, -211, 2212, EnuMin, EnuMax);
}

