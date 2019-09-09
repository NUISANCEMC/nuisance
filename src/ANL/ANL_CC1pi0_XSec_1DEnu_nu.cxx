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

#include "ANL_CC1pi0_XSec_1DEnu_nu.h"

//********************************************************************
ANL_CC1pi0_XSec_1DEnu_nu::ANL_CC1pi0_XSec_1DEnu_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "ANL CC1pi0 Event Rate 1DQ2 nu sample. \n" \
                        "Target: D2 \n" \
                        "Flux:  \n" \
                        "Signal:  \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("E_{#nu} (GeV)");
  fSettings.SetYTitle("#sigma (cm^{2}/neutron)");
  fSettings.SetAllowedTypes("FIX/DIAG", "FIX,FREE,SHAPE/DIAG");
  fSettings.SetEnuRange(0.0, 1.5);
  fSettings.SetS("norm_error", "0.20");
  fSettings.DefineAllowedTargets("D,H");

  // plot information
  fSettings.SetTitle("ANL #nu_mu CC1#pi^{0}");
  fSettings.DefineAllowedSpecies("numu");

  // User can specifiy to use uncorrected data
  UseCorrectedData = !fSettings.Found("name", "Uncorr");

  // User can specify "W14" for W < 1.4 GeV cut
  //                  "W16" for W < 1.6 GeV cut
  //                  The default is W < 2.0
  if (fSettings.Found("name", "W14Cut")) wTrueCut = 1.4;
  else if (fSettings.Found("name", "W16Cut")) wTrueCut = 1.6;
  else wTrueCut = 10.0;


  // Flag for bad combo
  if (UseCorrectedData && wTrueCut == 1.6) {
    QERROR(WRN, "Can not run ANL CC1pi0 W < 1.6 GeV with CORRECTION, because "
                "the data DOES NOT EXIST");
    QERROR(WRN, "Correction exists for W < 1.4 GeV and no W cut data ONLY");
    QERROR(WRN, "Reverting to using uncorrected data!");
    UseCorrectedData = false;
  }

  // Now read in different data depending on what the user has specified
  std::string DataLocation = GeneralUtils::GetTopLevelDir() + "/data/ANL/CC1pi0_on_n/";

  // If we're using corrected data
  if (UseCorrectedData) {
    if (wTrueCut == 1.4) {
      DataLocation += "anl82corr-numu-n-to-mu-p-pi0-lowW_edges.txt";
    } else if (wTrueCut == 10.0) {
      DataLocation += "anl82corr-numu-n-to-mu-p-pi0-noW_edges.txt";
    }

  // If we're using raw uncorrected data
  } else {

    if (wTrueCut == 1.4) {
      DataLocation += "anl82-numu-cc1pi0-14Wcut.txt";
    } else if (wTrueCut == 1.6) {
      DataLocation += "anl82-numu-cc1pi0-16Wcut.txt";
    } else if (wTrueCut == 10.0) {
      DataLocation += "anl82-numu-cc1pi0-noWcut.txt";
    }
  }
  fSettings.SetDataInput(DataLocation);

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = (GetEventHistogram()->Integral("width") * 1E-38 * 2.0 / 1.0 / (fNEvents + 0.));

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};


void ANL_CC1pi0_XSec_1DEnu_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(2212) == 0 || event->NumFSParticle(111) == 0 || event->NumFSParticle(13) == 0) {
    return;
  }

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pp   = event->GetHMFSParticle(2212)->fP;
  TLorentzVector Ppi0 = event->GetHMFSParticle(111)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double hadMass = FitUtils::MpPi(Pp, Ppi0);
  double Enu     = -1.0;

  if (hadMass/1000. < wTrueCut) {
    Enu = Pnu.E()/1.E3;
  }

  fXVar = Enu;

  return;
};


bool ANL_CC1pi0_XSec_1DEnu_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 111, 2212, EnuMin, EnuMax);
}

