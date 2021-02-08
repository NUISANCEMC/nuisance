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

#include "GGM_CC1ppip_Evt_1DQ2_nu.h"



//********************************************************************
GGM_CC1ppip_Evt_1DQ2_nu::GGM_CC1ppip_Evt_1DQ2_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "GGM_CC1ppip_Evt_1DQ2_nu sample. \n" \
                        "Target: D2 \n" \
                        "Flux:  \n" \
                        "Signal:  \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("Q^{2} (GeV^{2})");
  fSettings.SetYTitle("Number of events");
  // fSettings.SetAllowedTypes("EVT/SHAPE/DIAG", "EVT/SHAPE/DIAG");
  fSettings.SetEnuRange(1.0, 10.0);
  fSettings.DefineAllowedTargets("D,H");
 fIsDiag = true;
  fIsRawEvents = true;

  // plot information
  fSettings.SetTitle("GGM_CC1ppip_Evt_1DQ2_nu");
  fSettings.DefineAllowedSpecies("numu");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "GGM/CC1pip_on_p/GGM_CC1ppip_Q2_events_bin_edit.txt");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor for shape
  fScaleFactor = GetEventHistogram()->Integral("width")*double(1E-38)/double(fNEvents+0.)*(2./1.);

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetPoissonErrors();
  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

}

void GGM_CC1ppip_Evt_1DQ2_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(2212) == 0 ||
      event->NumFSParticle(211) == 0 ||
      event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pp   = event->GetHMFSParticle(2212)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double hadMass = FitUtils::MpPi(Pp, Ppip);
  double q2CCpip = -1.0;

  // GGM has a M(pi, p) < 1.4 GeV cut imposed only on this channel
  if (hadMass < 1400) q2CCpip = -1*(Pnu-Pmu).Mag2()/1.E6;
  fXVar = q2CCpip;

  return;
}

bool GGM_CC1ppip_Evt_1DQ2_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 211, 2212, EnuMin, EnuMax);
}
