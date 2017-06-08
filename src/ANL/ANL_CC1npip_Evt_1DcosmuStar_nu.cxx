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

#include "ANL_CC1npip_Evt_1DcosmuStar_nu.h"

//********************************************************************
ANL_CC1npip_Evt_1DcosmuStar_nu::ANL_CC1npip_Evt_1DcosmuStar_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "ANL CC1npip Event Rate 1DcosmuStar nu sample. \n" \
                        "Target: D2 \n" \
                        "Flux: ANL fhc numu \n" \
                        "Signal: CC1pi 3 Prong (SignalDef::isCC1pi3Prong) \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);

  fSettings.SetTitle("ANL #nu_mu CC1n#pi^{+}");
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("cos(#theta*)");
  fSettings.SetYTitle("Number of events");
  fSettings.SetEnuRange(0.0, 1.5);
  fSettings.SetAllowedTypes("EVT/SHAPE/DIAG", "EVT/SHAPE/DIAG");
  fSettings.DefineAllowedTargets("D,H");
  fSettings.DefineAllowedSpecies("numu");
  fSettings.SetDataInput( FitPar::GetDataBase() + "/ANL/CC1pip_on_n/ANL_CC1npip_cosmuStar.csv" );

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width")/(fNEvents+0.)*2./1.;

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetPoissonErrors();
  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};


void ANL_CC1npip_Evt_1DcosmuStar_nu::FillEventVariables(FitEvent *event) {

  if (event->NumISParticle(2112) == 0 || // Initial state particles
      event->NumFSParticle(2112) == 0 || event->NumFSParticle(211) == 0 || event->NumFSParticle(13) == 0) { // Final state particles

    return;
  }

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pin  = event->GetHMISParticle(2112)->fP;
  TLorentzVector Pn   = event->GetHMFSParticle(2112)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double hadMass = FitUtils::MpPi(Pn, Ppip);
  double cosmuStar = -999;

  // Now need to boost into center-of-mass frame
  TLorentzVector CMS = Pnu + Pin;
  // Boost the muon backwards
  Pmu.Boost(-CMS.BoostVector());
  // Boost the neutrino forwards
  Pnu.Boost(CMS.BoostVector());

  // ANL has a M(pi, p) < 1.4 GeV cut imposed
  // Find angle in CMS frame
  if (hadMass < 1400) cosmuStar = cos(FitUtils::th(Pmu, Pnu));

  fXVar = cosmuStar;

  return;
};

bool ANL_CC1npip_Evt_1DcosmuStar_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 211, 2112, EnuMin, EnuMax);
}
