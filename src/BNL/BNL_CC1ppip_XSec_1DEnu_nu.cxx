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

#include "BNL_CC1ppip_XSec_1DEnu_nu.h"

// T. Kitagaki et al. "Charged-current exclusive pion production in neutrino-deuterium interactions"
// Physical Review D, Volume 34, Number 9, 1 Nov 1986
// Figure 7
//
// Also see 
// N. J. Baker et al. "Study of the isospin structure of single-pion production in charged-current neutrino interactions"
// Physical Review D, Volume 23, Number 11, 1 June 1981
//
// K. Furuno et al. NuInt02 proceedings, (supposedly published in Nucl. Phys. B but I never found it), Retreieved from KEK preprints.
// KEK Preprint 2003-48, RCNS-03-01, September 2003

//********************************************************************
BNL_CC1ppip_XSec_1DEnu_nu::BNL_CC1ppip_XSec_1DEnu_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "BNL_CC1ppip_XSec_1DEnu_nu sample. \n" \
                        "Target: D2 \n" \
                        "Flux:  \n" \
                        "Signal:  \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("E_{#nu} (GeV)");
  fSettings.SetYTitle("#sigma (cm^{2}/proton)");
  fSettings.SetAllowedTypes("FIX/DIAG", "FIX,FREE,SHAPE/DIAG");
  fSettings.SetEnuRange(0.0, 6.0);
  fSettings.SetS("norm_error", "0.15");
  fSettings.DefineAllowedTargets("D,H");

  // plot information
  fSettings.SetTitle("BNL_CC1ppip_XSec_1DEnu_nu");
  fSettings.DefineAllowedSpecies("numu");

  // User can specifiy to use uncorrected data
  UseCorrectedData = !fSettings.Found("name", "Uncorr");

  // User can specify 1.4 or no W cut
  if (fSettings.Found("name", "W14Cut")) wTrueCut = 1.4;
  else wTrueCut = 10.0;

  // Now read in different data depending on what the user has specified
  std::string DataLocation = GeneralUtils::GetTopLevelDir() + "/data/BNL/CC1pip_on_p/";

  // If we're using corrected data
  if (UseCorrectedData) {
    if (wTrueCut == 1.4) {
      DataLocation += "BNL_CC1pip_on_p_W14_1986_corr.txt";
      fSettings.SetEnuRange(0.5, 3.0);
    } else {
      DataLocation += "BNL_CC1pip_on_p_1986_corr.txt";
      fSettings.SetEnuRange(0.4, 4.0);
    }
  // If we're using raw uncorrected data
  } else {
    if (wTrueCut == 1.4) {
      DataLocation += "BNL_CC1pip_on_p_W14_1986.txt";
      fSettings.SetEnuRange(0.6, 3.0);
    } else {
      DataLocation += "BNL_CC1pip_on_p_1986.txt";
      fSettings.SetEnuRange(0.5, 14.0);
    }
  }

  fSettings.SetDataInput(DataLocation);

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/proton
  fScaleFactor = (GetEventHistogram()->Integral("width") * 1E-38) / ((fNEvents + 0.)) * 2. / 1.;

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};

void BNL_CC1ppip_XSec_1DEnu_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(2212) == 0 || event->NumFSParticle(211) == 0 || event->NumFSParticle(13) == 0) return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pp   = event->GetHMFSParticle(2212)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double hadMass = FitUtils::MpPi(Pp, Ppip);
  double Enu     = -1.0;

  if (hadMass/1000. < wTrueCut) {
    Enu = Pnu.E() / 1000.;
  }

  fXVar = Enu;

  return;
};

bool BNL_CC1ppip_XSec_1DEnu_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 211, 2212, EnuMin, EnuMax);
}

