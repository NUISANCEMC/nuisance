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

#include "MINERvA_SignalDef.h"
#include "MINERvA_CC0pi_XSec_1DEe_nue.h"

//********************************************************************
MINERvA_CC0pi_XSec_1DEe_nue::MINERvA_CC0pi_XSec_1DEe_nue(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MINERvA CC0pi nue Ee sample. \n" \
                        "Target: CH \n" \
                        "Flux: MINERvA Forward Horn Current nue + nuebar \n" \
                        "Signal: Any event with 1 electron, any nucleons, and no other FS particles \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("E_{e} (GeV)");
  fSettings.SetYTitle("d#sigma/dE_{e} (cm^{2}/GeV)");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/FULL");
  fSettings.SetEnuRange(0.0, 20.0);
  fSettings.DefineAllowedTargets("C,H");
  // fSettings.SetSuggestedFlux( FitPar::GetDataBase() + "/MiniBooNE/ccqe/mb_ccqe_flux.root");

  // CCQELike plot information
  fSettings.SetTitle("MINERvA #nu_e CC0#pi");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "/MINERvA/CC0pi/MINERvA_CC0pi_nue_Data_ARX1509_05729.root" );
  fSettings.SetCovarInput( FitPar::GetDataBase() + "/MINERvA/CC0pi/MINERvA_CC0pi_nue_Data_ARX1509_05729.root" );
  fSettings.DefineAllowedSpecies("nue,nueb");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = (GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.)) / TotalIntegratedFlux();

  // Plot Setup -------------------------------------------------------
  SetDataFromFile( fSettings.GetDataInput(), "Data_1DEe" );
  SetCovarFromDataFile(fSettings.GetCovarInput(), "Covar_1DEe");

  // Extra Convert covar from 1E-40 to 1E-38
  *fDecomp *= (1.0 / 10.0);
  *fFullCovar *= (1.0 / 100.0);
  *covar *= (100.0);

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};

//********************************************************************
void MINERvA_CC0pi_XSec_1DEe_nue::FillEventVariables(FitEvent *event) {
//********************************************************************

  if (event->NumFSParticle(11) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pe   = event->GetHMFSParticle(11)->fP;

  Thetae   = Pnu.Vect().Angle(Pe.Vect());
  Enu_rec  = FitUtils::EnuQErec(Pe, cos(Thetae), 34., true);
  Q2QEe    = FitUtils::Q2QErec(Pe, cos(Thetae), 34., true);
  Ee       = Pe.E() / 1000.0;

  fXVar = Ee;
  return;
}



//********************************************************************
bool MINERvA_CC0pi_XSec_1DEe_nue::isSignal(FitEvent *event) {
//*******************************************************************

  // Check that this is a nue CC0pi event
  if (!SignalDef::isCC0pi(event, 12, EnuMin, EnuMax)) return false;

  // Electron Enrgy
  if (Ee < 0.5) return false;

  return true;
};

