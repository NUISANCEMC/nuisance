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

// T. Kitagaki et al. "Charged-current exclusive pion production in neutrino-deuterium interactions"
// Physical Review D, Volume 34, Number 9, 1 Nov 1986
// Figure 3
//
// K. Furuno et al. NuInt02 proceedings, (supposedly published in Nucl. Phys. B but I never found it), Retreieved from KEK preprints.
// KEK Preprint 2003-48, RCNS-03-01, September 2003
// Figure 8
//
// Also see 
// N. J. Baker et al. "Study of the isospin structure of single-pion production in charged-current neutrino interactions"
// Physical Review D, Volume 23, Number 11, 1 June 1981

#include "BNL_CC1ppip_Evt_1DQ2_nu.h"

//********************************************************************
BNL_CC1ppip_Evt_1DQ2_nu::BNL_CC1ppip_Evt_1DQ2_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "BNL_CC1ppip_Evt_1DQ2_nu sample. \n" \
                        "Target: D2 \n" \
                        "Flux:  \n" \
                        "Signal:  \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("Q^{2}_{CC#pi} (GeV^{2})");
  fSettings.SetYTitle("Number of events");
  fSettings.SetAllowedTypes("EVT/SHAPE/DIAG", "EVT/SHAPE/DIAG");
  fSettings.SetEnuRange(0.5, 6.0);
  fSettings.DefineAllowedTargets("D,H");

  // plot information
  fSettings.SetTitle("BNL #nu_mu CC1n#pi^{+}");
  fSettings.DefineAllowedSpecies("numu");

  // Hadronic Cut Info
  HadCut = fSettings.Found("name", "W14Cut") ? 1.4 : 10.0;
  if (HadCut == 1.4) {
    fSettings.SetDataInput(  FitPar::GetDataBase()
                             + "/BNL/CC1pip_on_p/BNL_CC1pip_on_p_noEvents_q2_w14_enu05to6_firstQ2rem.txt" );
  } else {
    fSettings.SetDataInput(  FitPar::GetDataBase()
                             + "/BNL/CC1pip_on_p/BNL_CC1pip_on_p_noEvents_q2_noWcut_HighQ2Gone.txt");
    fSettings.SetEnuRange(0.0, 10.0);
  }

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width") / (fNEvents + 0.);

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetPoissonErrors();
  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};

void BNL_CC1ppip_Evt_1DQ2_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(2212) == 0 || event->NumFSParticle(211) == 0 || event->NumFSParticle(13) == 0) {
    return;
  }

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pp   = event->GetHMFSParticle(2212)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double hadMass = FitUtils::MpPi(Pp, Ppip);
  double q2CCpip = -1.0;

  // BNL has a M(pi, p) < 1.4 GeV cut and no W cut.
  // This should be specified by user in "type" field
  // Reverts to 10 GeV (essentially no W cut!)
  if (hadMass < HadCut * 1000.) {
    q2CCpip = -1 * (Pnu - Pmu).Mag2() / 1.E6;
  }

  fXVar = q2CCpip;

  return;
};


bool BNL_CC1ppip_Evt_1DQ2_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 211, 2212, EnuMin, EnuMax);
}

