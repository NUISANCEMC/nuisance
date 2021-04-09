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
#include "ArgoNeuT_CCInc_XSec_1Dpmu_nu.h"


//********************************************************************
ArgoNeuT_CCInc_XSec_1Dpmu_nu::ArgoNeuT_CCInc_XSec_1Dpmu_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "ArgoNeuT_CCInc_XSec_1Dpmu_nu sample. \n" \
                        "Target: Ar40 \n" \
                        "Flux:  \n" \
                        "Signal:  \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("p_{#mu} (GeV)");
  fSettings.SetYTitle("d#sigma/dp_{#mu} (cm^{2} Ar^{-1} GeV^{-1})");
  fSettings.SetAllowedTypes("FIX/DIAG", "FIX/DIAG");
  fSettings.SetEnuRange(0.0, 50.0);
  fSettings.DefineAllowedTargets("Ar");

  // CCQELike plot information
  fSettings.SetTitle("ArgoNeut CC-INC numu 1Dp_{#mu}");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "/ArgoNeuT/CCInc_dsig_dmumom_nu.dat" );
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width") * double(1E-38) / double(fNEvents) *
                 (40.0 /*Data is /Ar */) / TotalIntegratedFlux("width");

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  ScaleData(1E-38);
  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};

void ArgoNeuT_CCInc_XSec_1Dpmu_nu::FillEventVariables(FitEvent *event) {
  FitParticle* pmu = event->GetHMFSParticle(13);
  if (pmu) fXVar = pmu->fP.Vect().Mag()/1000.0;
  return;
};

//********************************************************************
bool ArgoNeuT_CCInc_XSec_1Dpmu_nu::isSignal(FitEvent *event)
//********************************************************************
{
  return SignalDef::isCCINC(event, 14, EnuMin, EnuMax);
}
