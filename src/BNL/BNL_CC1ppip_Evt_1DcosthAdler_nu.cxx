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

#include "BNL_CC1ppip_Evt_1DcosthAdler_nu.h"

// T. Kitagaki et al. "Charged-current exclusive pion production in neutrino-deuterium interactions"
// Physical Review D, Volume 34, Number 9, 1 Nov 1986
// Figure 12 and related
//
// Also see 
// N. J. Baker et al. "Study of the isospin structure of single-pion production in charged-current neutrino interactions"
// Physical Review D, Volume 23, Number 11, 1 June 1981
//
// K. Furuno et al. NuInt02 proceedings, (supposedly published in Nucl. Phys. B but I never found it), Retreieved from KEK preprints.
// KEK Preprint 2003-48, RCNS-03-01, September 2003

//********************************************************************
BNL_CC1ppip_Evt_1DcosthAdler_nu::BNL_CC1ppip_Evt_1DcosthAdler_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "BNL_CC1ppip_Evt_1DcosthAdler_nu sample. \n" \
                        "Target: D2 \n" \
                        "Flux:  \n" \
                        "Signal:  \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("cos#theta_{Adler}");
  fSettings.SetYTitle("Number of events");
  fSettings.SetAllowedTypes("EVT/SHAPE/DIAG", "EVT/SHAPE/DIAG");
  fSettings.SetEnuRange(0.5, 6.0);
  fSettings.DefineAllowedTargets("D,H");

  // CCQELike plot information
  fSettings.SetTitle("BNL_CC1ppip_Evt_1DcosthAdler_nu");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "/BNL/CC1pip_on_p/BNL_CC1ppip_W14_cosThAdler.csv" );
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor =GetEventHistogram()->Integral("width")/(fNEvents+0.)*2./1.;

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetPoissonErrors();
  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};


void BNL_CC1ppip_Evt_1DcosthAdler_nu::FillEventVariables(FitEvent *event) {

  fXVar = -999.99;

  if (event->NumFSParticle(2212) == 0 ||
      event->NumFSParticle(211) == 0 ||
      event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pp   = event->GetHMFSParticle(2212)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  // Get the hadronic mass
  double hadMass = FitUtils::MpPi(Pp, Ppip);
  if (hadMass > 1400) return;

  // Get Adler cos theta
  double cosThAdler = FitUtils::CosThAdler(Pnu, Pmu, Ppip, Pp);
  fXVar = cosThAdler;
};

bool BNL_CC1ppip_Evt_1DcosthAdler_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 211, 2212,EnuMin,EnuMax);
}

