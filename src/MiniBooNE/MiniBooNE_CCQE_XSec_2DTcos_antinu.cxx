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

#include "MiniBooNE_CCQE_XSec_2DTcos_antinu.h"



//********************************************************************
MiniBooNE_CCQE_XSec_2DTcos_antinu::MiniBooNE_CCQE_XSec_2DTcos_antinu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MiniBooNE_CCQE_XSec_2DTcos_antinu sample. \n" \
                        "Target: CH2 \n" \
                        "Flux: MiniBooNE Forward Horn Current numu + numubar \n" \
                        "Signal: Any event with 1 anti-muon, any nucleons, and no other FS particles \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("T_{#mu} (GeV)");
  fSettings.SetYTitle("cos#theta_{#mu}");
  fSettings.SetZTitle("d^{2}#sigma/dT_{#mu}dcos#theta_{#mu} (cm^{2}/GeV)");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG/NORM/MASK", "FIX/DIAG");
  fSettings.SetEnuRange(0.0, 3.0);
  fSettings.SetNormError(0.130);
  fSettings.DefineAllowedTargets("C,H");

  // CCQELike plot information
  fSettings.SetTitle("MiniBooNE_CCQE_XSec_2DTcos_antinu");

  ccqelike = fSettings.Found("name", "CCQELike");
  if (!ccqelike) {
    fSettings.SetDataInput(  FitPar::GetDataBase() + "/MiniBooNE/anti-ccqe/aski_like.txt" );
    fSettings.SetErrorInput( FitPar::GetDataBase() + "/MiniBooNE/anti-ccqe/aski_err.txt" );
    fSettings.DefineAllowedSpecies("numub");

  } else {
    fSettings.SetDataInput(  FitPar::GetDataBase() + "/MiniBooNE/anti-ccqe/aski_like.txt" );
    fSettings.SetErrorInput( FitPar::GetDataBase() + "/MiniBooNE/anti-ccqe/aski_err.txt" );
    fMeasurementSpeciesType = kNumuWithWrongSignMeasurement;
    fSettings.DefineAllowedSpecies("numu,numub");
  }

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = ((GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.)) * (14.08 / 8.) / TotalIntegratedFlux());

  // Plot Setup -------------------------------------------------------
  Double_t binx[19] = { 0.2,  0.3,  0.4,  0.5,  0.6,  0.7,  0.8,  0.9,  1.0,  1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0};
  Double_t biny[21] = { -1.0, -0.9, -0.8, -0.7, -0.6, -0.5, -0.4, -0.3, -0.2, -0.1, 0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
  CreateDataHistogram(19, binx, 21, biny);

  SetDataValuesFromTextFile( fSettings.GetDataInput() );
  ScaleData(1E-41);

  SetDataErrorsFromTextFile( fSettings.GetErrorInput() );
  ScaleDataErrors(1E-41);

  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};

//********************************************************************
void  MiniBooNE_CCQE_XSec_2DTcos_antinu::FillEventVariables(FitEvent *event) {
//********************************************************************

  if (event->NumFSParticle(PhysConst::pdg_muons) == 0)
    return;

  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;

  // The highest momentum mu+/mu-. The isSignal definition should make sure we only
  // accept events we want, so no need to do an additional check here.
  TLorentzVector Pmu = event->GetHMFSParticle(PhysConst::pdg_muons)->fP;

  // Now find the kinematic values and fill the histogram
  Ekmu     = Pmu.E() / 1000.0 - PhysConst::mass_muon;
  costheta = cos(Pnu.Vect().Angle(Pmu.Vect()));

  // Set X Variables
  fXVar = Ekmu;
  fYVar = costheta;

  return;
};

//********************************************************************
bool MiniBooNE_CCQE_XSec_2DTcos_antinu::isSignal(FitEvent *event) {
//********************************************************************

  // If CC0pi, include both charges
  if (ccqelike) {
    if (SignalDef::isCC0pi(event, 14, EnuMin, EnuMax) ||
        SignalDef::isCC0pi(event, -14, EnuMin, EnuMax))
      return true;
  } else {
    if (SignalDef::isCCQELike(event, -14, EnuMin, EnuMax))
      return true;
  }

  return false;

};

