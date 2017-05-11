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

#include <string>
#include <sstream>

#include "MINERvA_SignalDef.h"
#include "MINERvA_CC0pi_XSec_1DQ2_Tgt_nu.h"

//********************************************************************
MINERvA_CC0pi_XSec_1DQ2_Tgt_nu::MINERvA_CC0pi_XSec_1DQ2_Tgt_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MINERvA_CC0pi_XSec_1DQ2_Tgt_nu sample. \n" \
    "Target: Either C, CH, Fe, Pb \n"						    \
    "Flux: MINERvA Forward Horn numu \n";
    "Signal: Any event with 1 muon, 1 proton p>450, no pions";


  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("Q^{2}_{QE} (GeV^{2})");
  fSettings.SetYTitle("d#sigma/dQ^{2} (cm^{2}/GeV^{2})");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG/NORM", "FIX/DIAG");
  fSettings.SetEnuRange(0.0, 100.0);
  fSettings.DefineAllowedTargets("C,H");

  // CCQELike plot information
  fSettings.SetTitle("MINERvA_CC0pi_XSec_1DQ2_Tgt_nu");
  fSettings.DefineAllowedSpecies("numu");


  // Set Target From name
  if (fSettings.Found("name","_CH")){
    fTarget = 1; // kTargetCH;
    fSettings.SetDataInput(  FitPar::GetDataBase() + "/MINERvA/CC0pi/Q2_Tgt_CH_data.txt");
  } else if (fSettings.Found("name","TgtC")){
    fTarget = 2; //kTargetC;
    fSettings.SetDataInput(  FitPar::GetDataBase() + "/MINERvA/CC0pi/Q2_Tgt_C_data.txt");
    fSettings.SetCovarInput(  FitPar::GetDataBase() + "/MINERvA/CC0pi/Q2_Tgt_C_covar.txt");
  }  else if (fSettings.Found("name","TgtFe")){
    fTarget = 3; //kTargetFe;
    fSettings.SetDataInput(  FitPar::GetDataBase() + "/MINERvA/CC0pi/Q2_Tgt_Fe_data.txt");
    fSettings.SetCovarInput(  FitPar::GetDataBase() + "/MINERvA/CC0pi/Q2_Tgt_Fe_covar.txt");
  } else if (fSettings.Found("name","TgtPb")){
    fTarget = 4; //kTargetPb;
    fSettings.SetDataInput(  FitPar::GetDataBase() + "/MINERvA/CC0pi/Q2_Tgt_Pb_data.txt");
    fSettings.SetCovarInput(  FitPar::GetDataBase() + "/MINERvA/CC0pi/Q2_Tgt_Pb_covar.txt");
  } else {
    ERR(FTL) << "Target not found in name! " << std::endl;
    throw;
  }


  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = (GetEventHistogram()->Integral("width")*1E-38/(fNEvents+0.))/TotalIntegratedFlux();

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  if (fTarget == 1){
    SetCovarFromDiagonal(fDataHist);
  } else {
    SetCorrelationFromTextFile(fSettings.GetCovarInput() );
  }

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};

void MINERvA_CC0pi_XSec_1DQ2_Tgt_nu::FillEventVariables(FitEvent *event){

  // Has NuMuCC1p
  if (event->HasISNuMuon() &&
      event->HasFSMuon() &&
      event->HasFSProton()){

    TLorentzVector pnu    = event->GetHMISNuMuon()->fP;
    TLorentzVector pprot  = event->GetHMFSProton()->fP;
    TLorentzVector pmu    = event->GetHMFSMuon()->fP;

    // Q2QE rec from leading proton assuming 34 MeV Eb
    double protmax = pprot.E();
    double q2qe    = FitUtils::ProtonQ2QErec(protmax, 34.);

    // Coplanar is angle between muon and proton plane
    TVector3 plnprotnu = pprot.Vect().Cross(pnu.Vect());
    TVector3 plnmunu   = pmu.Vect().Cross(pnu.Vect());
    double copl        = plnprotnu.Angle(plnmunu);

    // Fill X Variables
    fXVar = q2qe;

    // Save Coplanar into spare y variable
    fYVar = copl;
  }

  return;
};


bool MINERvA_CC0pi_XSec_1DQ2_Tgt_nu::isSignal(FitEvent *event){

  // IS NuMu + FS Muon
  if (!event->HasISNuMuon()) return false;
  if (!event->HasFSMuon()) return false;

  // No Pions
  if (!SignalDef::isCC0pi(event, 14, EnuMin, EnuMax)) return false;

  // Proton Threshold
  if (!SignalDef::HasProtonMomAboveThreshold(event, 450.0)){
    return false;
  }

  return true;
};
