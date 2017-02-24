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

#include "MiniBooNE_CC1pi0_XSec_1Dppi0_nu.h"

MiniBooNE_CC1pi0_XSec_1Dppi0_nu::MiniBooNE_CC1pi0_XSec_1Dppi0_nu(nuiskey confkey) {

  // 1. Initalise sample Settings (all overrideable in cardfile) --------------
  fSettings = LoadSampleSettings(confkey); // Must go first
  fSettings.SetDescription("");
  fSettings.SetXTitle("p_{#pi^{0}} (GeV/c)");
  fSettings.SetYTitle("d#sigma/dp_{#pi^{0}} (cm^{2}/GeV/CH_{2})");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/FULL");
  fSettings.SetEnuRange(0.5, 2.0);
  fSettings.SetSuggestedFlux( FitPar::GetDataBase() + "/MiniBooNE/ccqe/mb_fhc_flux.root");

  fSettings.SetTitle("MiniBooNE #nu_#mu CC1#pi^{0} on CH");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "/MiniBooNE/CC1pi0/dxsecdppi_edit.txt" );
  fSettings.SetCovarInput( FitPar::GetDataBase() + "/MiniBooNE/CC1pi0/dxsecdppi_covar.txt" );
  fSettings.DefineAllowedSpecies("numu");
  fSettings.DefineAllowedTargets("C,H");
  FinaliseSampleSettings(); // Must go after all settings

  // 2. Set Scale Factor -------------------------------------------------------
  fScaleFactor = GetEventHistogram()->Integral("width")
                 * double(1E-38) / double(fNEvents)
                 * (14.08) / TotalIntegratedFlux("width");

  // 3. Plot Setup -------------------------------------------------------
  SetDataValues( fSettings.GetDataInput() );
  SetCovarMatrixFromCorrText( fSettings.GetCovarInput(), fDataHist->GetNbinsX() );

  // Create a Target Species Stack copying data
  fTargetStack = new TargetTypeStack( fSettings.Name() + "_TGT", 
                                         "Target Contributions" + fSettings.PlotTitles(), 
                                         fDataHist);
  SetAutoProcessTH1(fTargetStack);

  // Final MC Setup
   // Must go last
  FinaliseMeasurement();
};

void MiniBooNE_CC1pi0_XSec_1Dppi0_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(111) == 0 ||
      event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppi0 = event->GetHMFSParticle(111)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double p_pi0 = FitUtils::p(Ppi0);

  fXVar = p_pi0;
  fTargetPDG = event->fTargetPDG;

  return;
};

//********************************************************************
bool MiniBooNE_CC1pi0_XSec_1Dppi0_nu::isSignal(FitEvent *event) {
//********************************************************************
  return SignalDef::isCC1pi(event, 14, 111, EnuMin, EnuMax);
}

//********************************************************************
void MiniBooNE_CC1pi0_XSec_1Dppi0_nu::FillExtraHistograms(MeasurementVariableBox* box, double weight){
//********************************************************************
  if (!Signal) return;
  fTargetStack->Fill( fTargetPDG, fXVar, weight );
}

