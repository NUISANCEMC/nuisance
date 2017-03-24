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

#include "ANL_CC1npip_Evt_1DQ2_nu.h"


//********************************************************************
ANL_CC1npip_Evt_1DQ2_nu::ANL_CC1npip_Evt_1DQ2_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "ANL CC1npip Event Rate 1DQ2 nu sample. \n" \
                        "Target: D2 \n" \
                        "Flux:  \n" \
                        "Signal:  \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("Q^{2}_{CC#pi} (GeV^{2})");
  fSettings.SetYTitle("Number of events");
  fSettings.SetAllowedTypes("EVT/SHAPE/DIAG", "EVT/SHAPE/DIAG");
  fSettings.SetEnuRange(0.0, 1.5);
  fSettings.DefineAllowedTargets("D,H");

  // plot information
  fSettings.SetTitle("ANL #nu_mu CC1n#pi^{+}");
  fSettings.DefineAllowedSpecies("numu");

  // Hadronic Cut Info
  HadCut = fSettings.Found("name", "W14Cut") ? 1.4 : 10.0;
  if (HadCut == 1.4) {
    fSettings.SetDataInput(  FitPar::GetDataBase()
                             + "/data/ANL/CC1pip_on_n/ANL_CC1pip_on_n_noEvents_Q2_W14GeV_rebin_firstQ2rem.txt" );
  } else {
    fSettings.SetDataInput(  FitPar::GetDataBase()
                             + "/data/ANL/CC1pip_on_n/ANL_CC1pip_on_n_noEvents_Q2_noWcut_HighQ2Gone.txt" );
  }

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = (GetEventHistogram()->Integral() / double(fNEvents));

  // Plot Setup -------------------------------------------------------
  SetDataValues( fSettings.GetDataInput() );

  // set Poisson errors on fDataHist (scanned does not have this)
  // Simple counting experiment here
  for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
    fDataHist->SetBinError(i + 1, sqrt(fDataHist->GetBinContent(i + 1)));
  }

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};

//********************************************************************
void ANL_CC1npip_Evt_1DQ2_nu::FillEventVariables(FitEvent *event) {
//********************************************************************

  if (event->NumFSParticle(2112) == 0 || event->NumFSParticle(211) == 0 || event->NumFSParticle(13) == 0) {
    return;
  }

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pn   = event->GetHMFSParticle(2112)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double hadMass = FitUtils::MpPi(Pn, Ppip);
  double q2CCpip;

  // ANL has a M(pi, p) < 1.4 GeV cut imposed (also no cut measurement but not useful for delta tuning)
  if (hadMass < HadCut * 1000.) {
    q2CCpip = -1.0 * (Pnu - Pmu).Mag2() / 1.E6;
  } else {
    q2CCpip = -1.0;
  }

  fXVar = q2CCpip;

  return;
};


//********************************************************************
bool ANL_CC1npip_Evt_1DQ2_nu::isSignal(FitEvent *event) {
//********************************************************************

  return SignalDef::isCC1pi3Prong(event, 14, 211, 2112, EnuMin, EnuMax);
}
