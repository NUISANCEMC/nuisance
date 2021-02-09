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
#include "ArgoNeuT_CC1Pi_XSec_1Dthetamupi_nu.h"


//********************************************************************
ArgoNeuT_CC1Pi_XSec_1Dthetamupi_nu::ArgoNeuT_CC1Pi_XSec_1Dthetamupi_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "ArgoNeuT_CC1Pi_XSec_1Dthetamupi_nu sample. \n" \
                        "Target: Ar40 \n" \
                        "Flux:  \n" \
                        "Signal: CC, single charged pion > 0.1 GeV/c, no other mesons. \n";
  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("#theta_{#mu#pi} (GeV)");
  fSettings.SetYTitle("d#sigma/d#theta_{#mu#pi} (cm^{2} nucleon^{-1} degree^{-1})");
  fSettings.SetAllowedTypes("FIX/DIAG", "FIX/DIAG");
  fSettings.SetEnuRange(0.0, 100.0);
  fSettings.DefineAllowedTargets("Ar");

  // CC1Pi plot information
  fSettings.SetTitle("ArgoNeut CC1#pi^{#pm} #nu_{#mu} 1D#theta_{#mu#pi}");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "/ArgoNeuT/CC1Pi_dsig_dthetamupi_nu.dat" );
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width") * double(1E-38) / double(fNEvents) /
                 TotalIntegratedFlux("width");

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  ScaleData(1E-38);
  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};

void ArgoNeuT_CC1Pi_XSec_1Dthetamupi_nu::FillEventVariables(FitEvent *event) {
  FitParticle* pmu = event->GetHMFSParticle(13);
  FitParticle* ppi = event->GetHMFSChargePions();
  if (pmu && ppi) fXVar = pmu->fP.Vect().Angle(ppi->fP.Vect()) * 180.0 / TMath::Pi();
  return;
};

//********************************************************************
bool ArgoNeuT_CC1Pi_XSec_1Dthetamupi_nu::isSignal(FitEvent *event)
//********************************************************************
{
  if (!SignalDef::isCCINC(event, 14, EnuMin, EnuMax)) return false;

  int nMesons   = event->NumFSMesons();
  int nLeptons  = event->NumFSLeptons();
  int nPiCharge = event->NumFSChargePions();
  int nPiZero   = event->NumFSPiZero();

  if (nPiCharge != 1 || nMesons != 1 || nLeptons != 1 || nPiZero > 0) {
    return false;
  }

  if (event->GetHMFSChargePions()->fP.Vect().Mag() < 100) return false;

  return true;
}

