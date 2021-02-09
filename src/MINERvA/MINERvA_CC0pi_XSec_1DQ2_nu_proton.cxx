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

#include <string>
#include <sstream>

#include "MINERvA_SignalDef.h"
#include "MINERvA_CC0pi_XSec_1DQ2_nu_proton.h"

// https://arxiv.org/abs/1409.4497
// DOI: 10.1103/PhysRevD.91.071301

//********************************************************************
MINERvA_CC0pi_XSec_1DQ2_nu_proton::MINERvA_CC0pi_XSec_1DQ2_nu_proton(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MINERvA_CC0pi_XSec_1DQ2_nu_proton sample. \n" \
                        "Target: CH \n" \
                        "Flux: MINERvA Forward Horn Current numu \n" \
                        "Signal: Any event with 1 muon, at least one proton in range, and no other FS particles \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("Q^{2}_{QE} (GeV^{2})");
  fSettings.SetYTitle("d#sigma/dQ^{2} (cm^{2}/GeV^{2})");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/FULL");
  fSettings.SetEnuRange(0.0, 100.0);
  fSettings.DefineAllowedTargets("C,H");

  // CCQELike plot information
  fSettings.SetTitle("MINERvA_CC0pi_XSec_1DQ2_nu_proton");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "/MINERvA/CC0pi/proton_2014/proton_Q2QE_nu_data.txt" );
  // Have a shape covariance
  fIsShape = fSettings.Found("type","SHAPE");
  std::string covid = FitPar::GetDataBase() + "/MINERvA/CC0pi/proton_2014/proton_Q2QE_nu_covar";
  if (fIsShape) covid += "_shape";
  covid += ".txt";
  fSettings.SetCovarInput(covid);
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = (GetEventHistogram()->Integral("width")*1E-38/(fNEvents+0.))/TotalIntegratedFlux();

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetCorrelationFromTextFile(fSettings.GetCovarInput() );

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

void MINERvA_CC0pi_XSec_1DQ2_nu_proton::FillEventVariables(FitEvent *event){

  // Has NuMuCC1p
  if (event->HasISNuMuon() &&
      event->HasFSMuon() &&
      event->HasFSProton()) {

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


bool MINERvA_CC0pi_XSec_1DQ2_nu_proton::isSignal(FitEvent *event){
  return SignalDef::isCC0pi1p_MINERvA(event, EnuMin*1.E3, EnuMax*1.E3);
};
