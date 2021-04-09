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

#include "BEBC_CC1ppip_XSec_1DQ2_nu.h"


//********************************************************************
BEBC_CC1ppip_XSec_1DQ2_nu::BEBC_CC1ppip_XSec_1DQ2_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "BEBC_CC1ppip_XSec_1DQ2_nu sample. \n" \
                        "Target: D2 \n" \
                        "Flux:  \n" \
                        "Signal:  \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("Q^{2} (GeV^{2})");
  fSettings.SetYTitle("d#sigma/dQ^{2} (cm^{2}/GeV^{2}/proton)");
  fSettings.SetAllowedTypes("FIX/FREE,SHAPE/DIAG", "FIX/DIAG");
  fSettings.SetEnuRange(5.0, 200.0);
  fSettings.DefineAllowedTargets("D,H");

  // plot information
  fSettings.SetTitle("BEBC_CC1ppip_XSec_1DQ2_nu");
  fSettings.DefineAllowedSpecies("numu");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "/BEBC/Dfill/BEBC_Dfill_CC1pi+_on_p_W14_edit.txt" );

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = (GetEventHistogram()->Integral("width")*1E-38)/((fNEvents+0.)*TotalIntegratedFlux("width"))*2./1.;

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetCovarFromDiagonal();

  hadMassHist = new TH1D((fName+"_Wrec").c_str(),(fName+"_Wrec").c_str(), 100, 1000, 2000);
  hadMassHist->SetTitle((fName+"; W_{rec} (GeV/c^{2}); Area norm. # of events").c_str());

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};



void BEBC_CC1ppip_XSec_1DQ2_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(2212) == 0 ||
      event->NumFSParticle(211) == 0 ||
      event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pp   = event->GetHMFSParticle(2212)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  hadMass = FitUtils::MpPi(Pp, Ppip);
  double q2CCpip = -1.0;

  // BEBC has a M(pi, p) < 1.4 GeV cut imposed only on this channel
  if (hadMass < 1400) q2CCpip = -1*(Pnu-Pmu).Mag2()/1.E6;

  fXVar = q2CCpip;

  return;
};



bool BEBC_CC1ppip_XSec_1DQ2_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 211, 2212, EnuMin, EnuMax);
}

void BEBC_CC1ppip_XSec_1DQ2_nu::FillHistograms() {

  Measurement1D::FillHistograms();

  hadMassHist->Fill(hadMass);

  return;
}

void BEBC_CC1ppip_XSec_1DQ2_nu::Write(std::string drawOpt) {

  Measurement1D::Write(drawOpt);
  hadMassHist->Scale(1/hadMassHist->Integral());
  hadMassHist->Write();

  return;
}
