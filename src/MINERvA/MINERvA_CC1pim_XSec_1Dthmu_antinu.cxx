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

#include "MINERvA_SignalDef.h"

#include "MINERvA_CC1pim_XSec_1Dthmu_antinu.h"


MINERvA_CC1pim_XSec_1Dthmu_antinu::MINERvA_CC1pim_XSec_1Dthmu_antinu(
  nuiskey samplekey )
{
  // Sample overview ---------------------------------------------------
  std::string descrip = "MINERvA_CC1pim_XSec_1Dthmu_antinu sample. \n"
    "Target: CH \n"
    "Flux: MINERvA Reverse Horn Current numubar \n"
    "Signal: Any event with 1 positive muon, 1 negative pion, and no other"
    " mesons. Any number of FS nucleons is allowed.\n";

  // Setup common settings
  fSettings = LoadSampleSettings( samplekey );
  fSettings.SetDescription( descrip );
  fSettings.SetXTitle( "#theta_{#mu} (degree)" );
  fSettings.SetYTitle( "d#sigma/d#theta_{#mu} (cm^{2}/degree/nucleon)" );
  fSettings.SetAllowedTypes( "FIX,FREE/DIAG,FULL/NORM/MASK", "FIX/FULL" );
  fSettings.SetEnuRange( 1.5, 10.0 );
  fSettings.DefineAllowedTargets( "C,H" );
  fSettings.DefineAllowedSpecies( "numub" );

  fSettings.SetTitle( "MINERvA_CC1pim_XSec_1Dthmu_antinu" );

  fSettings.SetDataInput( GeneralUtils::GetTopLevelDir()
    + "/data/MINERvA/CC1pim/minerva_cc1pim_data.root" );

  fSettings.SetCovarInput( GeneralUtils::GetTopLevelDir()
    + "/data/MINERvA/CC1pim/minerva_cc1pim_data.root" );

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width") * double(1E-38)
    / double(fNEvents) / TotalIntegratedFlux("width");

  // Plot Setup -------------------------------------------------------
  SetDataFromRootFile( fSettings.GetDataInput(), "mutheta_data" );
  SetCovarFromRootFile( fSettings.GetCovarInput(), "mutheta_covariance" );

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

}

void MINERvA_CC1pim_XSec_1Dthmu_antinu::FillEventVariables( FitEvent* event ) {

  const int ANTI_NUMU = -14;
  if ( event->NumISParticle(ANTI_NUMU) <= 0 ) return;
  FitParticle* numubar = event->GetHMISParticle( ANTI_NUMU );

  const int MU_PLUS = -13;
  if ( event->NumFSParticle(MU_PLUS) <= 0 ) return;
  FitParticle* antimuon = event->GetHMFSParticle( MU_PLUS );

  // Muon scattering angle (relative to the incident neutrino) in degrees
  double theta_mu = FitUtils::th( numubar, antimuon ) / TMath::Pi() * 180.;

  fXVar = theta_mu;

}

bool MINERvA_CC1pim_XSec_1Dthmu_antinu::isSignal( FitEvent* event ) {
  return SignalDef::isCC1pim_MINERvA( event, EnuMin, EnuMax );
}
