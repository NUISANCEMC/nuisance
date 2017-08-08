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

#include "MINERvA_CC0pi_XSec_1DThetae_nue.h"


//********************************************************************
MINERvA_CC0pi_XSec_1DThetae_nue::MINERvA_CC0pi_XSec_1DThetae_nue(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MINERvA_CC0pi_XSec_1DThetae_nue sample. \n" \
                        "Target: CH \n" \
                        "Flux: MINERvA Forward Horn Current nue + nuebar \n" \
                        "Signal: Any event with 1 electron, any nucleons, and no other FS particles \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("#theta_{e}");
  fSettings.SetYTitle("d#sigma/d#theta_{e} (cm^{2})");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/FULL");
  fSettings.SetEnuRange(0.0, 10.0);
  fSettings.DefineAllowedTargets("C,H");

  // CCQELike plot information
  fSettings.SetTitle("MINERvA #nu_e CC0#pi");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "/MINERvA/CC0pi/MINERvA_CC0pi_nue_Data_ARX1509_05729.root;Data_1DThetae" );
  fSettings.SetCovarInput( FitPar::GetDataBase() + "/MINERvA/CC0pi/MINERvA_CC0pi_nue_Data_ARX1509_05729.root;Covar_1DThetae" );
  fSettings.DefineAllowedSpecies("nue,nueb");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = (GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.)) / TotalIntegratedFlux();

  // Plot Setup -------------------------------------------------------
  SetDataFromRootFile( fSettings.GetDataInput() );
  SetCovarFromRootFile(fSettings.GetCovarInput() );
  ScaleCovar(1.0 / 1000.0);

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};

//********************************************************************
void MINERvA_CC0pi_XSec_1DThetae_nue::FillEventVariables(FitEvent *event){
//********************************************************************

  int PDGnu = event->GetNeutrinoIn()->fPID;
  int PDGe = 0;
  if (PDGnu == 12) PDGe= 11;
  else if (PDGnu == -12) PDGe = -11;

  if (event->NumFSParticle(PDGe) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pe   = event->GetHMFSParticle(PDGe)->fP;

  Thetae   = Pnu.Vect().Angle(Pe.Vect());
  Q2QEe    = FitUtils::Q2QErec(Pe, cos(Thetae), 34., true);
  Ee       = Pe.E() / 1000.0;

  fXVar = Thetae * 180. / TMath::Pi();
  return;
}



//********************************************************************
bool MINERvA_CC0pi_XSec_1DThetae_nue::isSignal(FitEvent *event){
//*******************************************************************

  // Check this is a nue CC0pi event
  if (!(SignalDef::isCC0pi(event, 12, EnuMin, EnuMax)) and
      !(SignalDef::isCC0pi(event, -12, EnuMin, EnuMax))) return false;

  // Restrict EE
  if (Ee < 0.5) return false;

  return true;
};

