//Adrian Orea
//I used the file MINERvA_CCinc_XSec_2DEavq3_nu.cxx as a template
//Also, I am fully aware of the naming typo (should be ptpz), but Everything is already named the same way so...

//Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

/*
  Author : Adrian Orea
 */


#include "MINERvA_SignalDef.h"
#include "MINERvA_CC0pi_XSec_2Dptpx_nu.h"

//********************************************************************
MINERvA_CC0pi_XSec_2Dptpx_nu::MINERvA_CC0pi_XSec_2Dptpx_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MINERvA_CC0pi_XSec_2Dptpx_nu sample. \n" \
                        "Target: CH \n" \
                        "Flux: MINERvA Medium Energy FHC numu  \n" \
                        "Signal: CC-0pi \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetYTitle("p_{z} (GeV)");
  fSettings.SetZTitle("p_{t} (GeV)");
  fSettings.SetZTitle("d^{2}#sigma/dP_{t}dP_{z} (cm^{2}/GeV^{2}/nucleon)");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/FULL,DIAG/MASK", "FIX/FULL");
  fSettings.SetEnuRange(0.0, 100.0);
  fSettings.DefineAllowedTargets("C,H");

  // CCQELike plot information
  fSettings.SetTitle("MINERvA_CC0pi_XSec_2Dptpx_nu");

  fSettings.SetDataInput(  FitPar::GetDataBase() + "MINERvA/CC0pi_ptpz_nu/data2D.txt");
  fSettings.SetCovarInput( FitPar::GetDataBase() + "MINERvA/CC0pi_ptpz_nu/covar.txt");
  fSettings.SetMapInput( FitPar::GetDataBase() + "MINERvA/CC0pi_ptpz_nu/map2D.txt" );
  fSettings.DefineAllowedSpecies("numu");
  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  //  fScaleFactor = (GetEventHistogram()->Integral("width") * 1E-37 / (fNEvents + 0.)) / this->TotalIntegratedFlux();
  fScaleFactor = (GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.)) / this->TotalIntegratedFlux();

  // Plot Setup -------------------------------------------------------
  Double_t P_t[14] = {0,0.075,0.15,0.25,0.325,0.4,0.475,0.55,0.7,0.85,1.0,1.25,1.5,2.5};
  Double_t P_z[13] = {1.5,2.0,2.5,3.0,3.5,4.0,4.5,5.0,6.0,8.0,10.0,15.0,20.0};
  CreateDataHistogram(14, P_t, 13, P_z);
  
  SetDataValuesFromTextFile( fSettings.GetDataInput() );
  //  fDataHist->Scale(1.0, "width");

  SetMapValuesFromText( fSettings.GetMapInput() );
  SetCovarFromTextFile( fSettings.GetCovarInput(), FitPar::Config().GetParI("CC0piNBINS") );
  
  StatUtils::SetDataErrorFromCov(fDataHist, fFullCovar, fMapHist, 1E-38);

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};

//********************************************************************
void MINERvA_CC0pi_XSec_2Dptpx_nu::FillEventVariables(FitEvent *event) {
//********************************************************************

// Checking to see if there is a Muon
  if (event->NumFSParticle(13) == 0) 
	return;

  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP; //I Added this part
  Double_t px = Pmu.X()/1000;
  Double_t py = Pmu.Y()/1000;
  Double_t pz = Pmu.Z()/1000;
  Double_t pt = sqrt(px*px+py*py);

  // Set Hist Variables
  fYVar = pz;
  fXVar = pt;

  return;
};

//********************************************************************
bool MINERvA_CC0pi_XSec_2Dptpx_nu::isSignal(FitEvent *event) {
//********************************************************************
  return SignalDef::isCC0pi_MINERvAPTPZ(event, 14, EnuMin, EnuMax);
  
};
