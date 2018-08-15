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

/**
 * D.Day et al, "Study of \nud charged-current two-pion production in the threshold region", Physical Review D, Volume 28, Number 11, 1 December 1983 \n
 * Derrick, Musgrave, Ammar, Day, Kafka and Mann, "Two- and three-pion productin by \nu\mud recations nears threshold: The implication for nucleon-decay experiments", Physical Review D, Vol 30, Number 7, 1 October 1984
*/

#include "ANL_CC2pi_1pim1pip_Evt_1Dppip_nu.h"


//********************************************************************
ANL_CC2pi_1pim1pip_Evt_1Dppip_nu::ANL_CC2pi_1pim1pip_Evt_1Dppip_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "ANL_CC2pi_1pim1pip_Evt_1Dppip_nu sample. \n" \
                        "Target: D2 \n" \
                        "Flux:  \n" \
                        "Signal:  \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("p_{#pi+} (GeV)");
  fSettings.SetYTitle("Number of events (area norm.)");
  fSettings.SetAllowedTypes("EVT/SHAPE/DIAG", "EVT/SHAPE/DIAG");
  fSettings.SetEnuRange(0.0, 6.0);
  fSettings.DefineAllowedTargets("D,H");

  // CCQELike plot information
  fSettings.SetTitle("ANL #nu_mu CC2#pi^{+-}");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "/ANL/CC2pi/1pim1pip/CC2pi_1pim1pip_ppip_weight.csv" );
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width")*double(1E-38)/double(fNEvents)*(2./1.);

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetPoissonErrors();
  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};

void ANL_CC2pi_1pim1pip_Evt_1Dppip_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(211) == 0)
    return;

  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  double ppip = FitUtils::p(Ppip);

  this->fXVar = ppip;

  return;
}

// Signal asks for 1pi-, 1pi+, 1mu-, 1p
bool ANL_CC2pi_1pim1pip_Evt_1Dppip_nu::isSignal(FitEvent *event) {
  int pdgs[] = {13, 211, -211, 2212};
  return SignalDef::isCCWithFS(event, 14, pdgs,
                               EnuMin, EnuMax);
}

