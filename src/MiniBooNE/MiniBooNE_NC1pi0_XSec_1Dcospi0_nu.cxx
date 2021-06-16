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

#include "MiniBooNE_NC1pi0_XSec_1Dcospi0_nu.h"


//********************************************************************
MiniBooNE_NC1pi0_XSec_1Dcospi0_nu::MiniBooNE_NC1pi0_XSec_1Dcospi0_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MiniBooNE_NC1pi0_XSec_1Dcospi0_nu sample. \n" \
                        "Target: CH \n" \
                        "Flux: MiniBooNE Forward Horn Current nue + nuebar \n" \
                        "Signal: Any event with 1 muon, any nucleons, and no other FS particles \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("cos#theta_{#pi^{0}}");
  fSettings.SetYTitle("d#sigma/dcos#theta_{#pi^{0}} (cm^{2}/nucleon)");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/FULL,DIAG/NORM/MASK", "FIX/FULL");
  fSettings.SetEnuRange(0.0, 5.0);
  fSettings.DefineAllowedTargets("C,H");

  fSettings.SetTitle("MiniBooNE_NC1pi0_XSec_1Dcospi0_nu");
  nunubarmode = fSettings.Found("name", "fhc");
  if (!nunubarmode) {
    fSettings.SetDataInput(  FitPar::GetDataBase() + "MiniBooNE/NC1pi0/nucosthetapi0xsec_edit.txt" );
    fSettings.SetCovarInput( FitPar::GetDataBase() + "MiniBooNE/NC1pi0/nucosthetapi0xsecerrormatrix.txt" );
    fSettings.DefineAllowedSpecies("numu");

  } else {
    fSettings.SetDataInput(  FitPar::GetDataBase() + "MiniBooNE/NC1pi0/combinedsignnumodecosthetapi0xsec_edit.txt");
    fSettings.SetCovarInput( FitPar::GetDataBase() + "MiniBooNE/NC1pi0/combinedsignnumodecosthetapi0xsecerrormatrix.txt");
    fSettings.DefineAllowedSpecies("numu,numub");

  }

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = (GetEventHistogram()->Integral("width") * 1E-38) / double(fNEvents) / TotalIntegratedFlux("width");

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetCovarFromTextFile( fSettings.GetCovarInput() );
  ScaleCovar(1.E-5);
  StatUtils::SetDataErrorFromCov(fDataHist, fFullCovar, 1E-38);
  SetShapeCovar();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};


void MiniBooNE_NC1pi0_XSec_1Dcospi0_nu::FillEventVariables(FitEvent *event) {

  // (CP) require pi0 in final state (this makes some assumptions about how the
  // generator treats the pi0 after it is produced in the nucleus.
  // MB required 2 photons to make a pion signal, so check for those later
  if (event->NumFSParticle(111) == 0 ) {
    return;
  }

  if (abs(event->NumFSParticle(13)) == 1 || abs(event->NumFSParticle(11)) == 1 || abs(event->NumFSParticle(15)) == 1 || abs(event->NumFSParticle(17)) == 1) return; 

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppi0 = event->GetHMFSParticle(111)->fP;

  double CosPi0 = cos(FitUtils::th(Pnu, Ppi0));

  fXVar = CosPi0;

  return;
};

//********************************************************************
bool MiniBooNE_NC1pi0_XSec_1Dcospi0_nu::isSignal(FitEvent *event) {
//********************************************************************
  if (nunubarmode){
    return (SignalDef::isNC1pi(event, 14, 111, EnuMin, EnuMax) ||
	    SignalDef::isNC1pi(event, -14, 111, EnuMin, EnuMax));
  } else {
    return SignalDef::isNC1pi(event, 14, 111, EnuMin, EnuMax);
  }
}
