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
 * Radecky et al. Phys Rev D, 3rd series, volume 25, number 5, 1 March 1982, p 1161-1173
*/
#include "ANL_CC1ppip_XSec_1DQ2_nu.h"


//********************************************************************
ANL_CC1ppip_XSec_1DQ2_nu::ANL_CC1ppip_XSec_1DQ2_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "ANL CC1ppip XSec 1DQ2 nu sample. \n" \
                        "Target: D2 \n" \
                        "Flux:  \n" \
                        "Signal:  \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("Q^{2}_{CC#pi} (GeV^{2})");
  fSettings.SetYTitle("d#sigma/dQ^{2}_{CC#pi} (GeV^{2})");
  fSettings.SetAllowedTypes("FIX/FREE,SHAPE/DIAG", "FIX/DIAG");
  fSettings.SetEnuRange(0.5, 6.0);
  fSettings.DefineAllowedTargets("D,H");

  // plot information
  fSettings.SetTitle("ANL #nu_mu CC1n#pi^{+}");
  fSettings.DefineAllowedSpecies("numu");
  fSettings.SetDataInput(  FitPar::GetDataBase()
                           + "/ANL/CC1pip_on_p/ANL_CC1pip_on_p_dSigdQ2_W14_1982.txt" );

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
      // this->fScaleFactor = (GetEventHistogram()->Integral("width")*1E-38)/((fNEvents+0.)*TotalIntegratedFlux("width"))*16./8.;
  fScaleFactor = (GetEventHistogram()->Integral("width") * 1E-38) / ((fNEvents + 0.) * TotalIntegratedFlux("width")) * 2. / 1.;

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};

void ANL_CC1ppip_XSec_1DQ2_nu::FillEventVariables(FitEvent *event) {

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

  // I use the W < 1.4GeV cut ANL data to isolate single pion
  // there is also a W < 1.6 GeV and an uncut spectrum ANL 1982
  if (hadMass < 1400) q2CCpip = -1 * (Pnu - Pmu).Mag2() / 1.E6;

  fXVar = q2CCpip;

  return;
};

bool ANL_CC1ppip_XSec_1DQ2_nu::isSignal(FitEvent *event) {
  // std::cout << "CC1ppip Enu " << EnuMin << " " << EnuMax << std::endl;
  return SignalDef::isCC1pi3Prong(event, 14, 211, 2212, EnuMin, EnuMax);
}

// void ANL_CC1ppip_XSec_1DQ2_nu::FillEventVariables(FitEvent *event) {
// 46  
// 47    if (event->NumFSParticle(2212) == 0 ||
// 48        event->NumFSParticle(211) == 0 ||
// 49        event->NumFSParticle(13) == 0)
// 50      return;
// 51  
// 52    TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
// 53    TLorentzVector Pp   = event->GetHMFSParticle(2212)->fP;
// 54    TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
// 55    TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;
// 56  
// 57    double hadMass = FitUtils::MpPi(Pp, Ppip);
// 58    double q2CCpip = -1.0;
// 59  
// 60    // I use the W < 1.4GeV cut ANL data to isolate single pion
// 61    // there is also a W < 1.6 GeV and an uncut spectrum ANL 1982
// 62    if (hadMass < 1400) q2CCpip = -1*(Pnu-Pmu).Mag2()/1.E6;
// 63  
// 64    fXVar = q2CCpip;
// 65  
// 66    return;
// 67  };
// 68  
// 69  bool ANL_CC1ppip_XSec_1DQ2_nu::isSignal(FitEvent *event) {
// 70    return SignalDef::isCC1pi3Prong(event, 14, 211, 2212, EnuMin, EnuMax);
// 71  }
