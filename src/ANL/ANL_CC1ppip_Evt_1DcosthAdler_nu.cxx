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

/**
 * Radecky et al. Phys Rev D, 3rd series, Vol 25, No 5, 1 March 1982, p 1161-1173
*/

#include "ANL_CC1ppip_Evt_1DcosthAdler_nu.h"


//********************************************************************
ANL_CC1ppip_Evt_1DcosthAdler_nu::ANL_CC1ppip_Evt_1DcosthAdler_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "ANL CC1ppip Event Rate 1DcosmuStar nu sample. \n" \
                        "Target: D2 \n" \
                        "Flux:  \n" \
                        "Signal:  \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("cos#theta_{Adler}");
  fSettings.SetYTitle("Number of events");
  fSettings.SetAllowedTypes("EVT/SHAPE/DIAG", "EVT/SHAPE/DIAG");
  fSettings.SetEnuRange(0.0, 6.0);
  fSettings.DefineAllowedTargets("D,H");

  // CCQELike plot information
  fSettings.SetTitle("ANL #nu_mu CC1n#pi^{+}");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "/ANL/CC1pip_on_p/ANL_CC1pip_on_p_noEvents_costhAdler_1982.csv" );
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width") / (fNEvents + 0.) * 2. / 1.;

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetPoissonErrors();
  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};



void ANL_CC1ppip_Evt_1DcosthAdler_nu::FillEventVariables(FitEvent *event) {

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

bool ANL_CC1ppip_Evt_1DcosthAdler_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 211, 2212, EnuMin, EnuMax);
}

/*
void ANL_CC1ppip_Evt_1DcosthAdler_nu::FillHistograms() {

  if (makeHadronicMassHist) {
    hadMassHist->Fill(hadMass);
  }

  Measurement1D::FillHistograms();

}


void ANL_CC1ppip_Evt_1DcosthAdler_nu::ScaleEvents() {

  PlotUtils::FluxUnfoldedScaling(fMCHist, GetFluxHistogram());
  PlotUtils::FluxUnfoldedScaling(fMCFine, GetFluxHistogram());

  fMCHist->Scale(fScaleFactor);
  fMCFine->Scale(fScaleFactor);

  return;
}
*/
