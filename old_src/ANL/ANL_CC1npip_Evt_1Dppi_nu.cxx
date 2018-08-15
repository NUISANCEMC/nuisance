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

#include "ANL_CC1npip_Evt_1Dppi_nu.h"

//********************************************************************
ANL_CC1npip_Evt_1Dppi_nu::ANL_CC1npip_Evt_1Dppi_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "ANL_CC1npip_Evt_1Dppi_nu sample. \n" \
                        "Dist   : Pion Momentum : p_{#pi} \n" \
                        "Signal : numuCC1pi+1p : #nu_{#mu}-CC-1#pi^{+}-1p \n" \
                        "Target : Deuterium : D2 \n" \
                        "Flux   : ANL FHC numu \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetTitle("ANL #nu_mu CC1p#pi^{+}");
  fSettings.SetXTitle("p_{#pi} (MeV)");
  fSettings.SetYTitle("Number of events");
  fSettings.SetAllowedTypes("EVT/SHAPE/DIAG","EVT/SHAPE/DIAG");
  fSettings.DefineAllowedTargets("D,H");
  fSettings.DefineAllowedSpecies("numu");
  fSettings.SetEnuRange(0.0, 1.5);
  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width") / (fNEvents + 0.) * 2. / 1.;

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( FitPar::GetDataBase() + "/ANL/CC1pip_on_n/ANL_ppi_CC1npip.csv" );
  SetPoissonErrors();
  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};

void ANL_CC1npip_Evt_1Dppi_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(2112) == 0 ||
      event->NumFSParticle(211) == 0 ||
      event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pn   = event->GetHMFSParticle(2112)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double hadMass = FitUtils::MpPi(Pn, Ppip);
  double ppip;

  // This measurement has a 1.4 GeV M(Npi) constraint
  if (hadMass < 1400) ppip = FitUtils::p(Ppip) * 1000.;
  else ppip = -1.0;

  fXVar = ppip;

  return;
}

bool ANL_CC1npip_Evt_1Dppi_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 211, 2112, EnuMin, EnuMax);
}

