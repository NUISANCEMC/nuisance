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

#include "SciBooNE_CCCOH_1TRK_1DQ2_nu.h"

SciBooNE_CCCOH_1TRK_1DQ2_nu::SciBooNE_CCCOH_1TRK_1DQ2_nu(nuiskey samplekey){
  
  // Sample overview
  std::string descrip = "SciBooNE CC-coherent 1 track sample.\n" \
    "Target: CH \n" \
    "Flux: SciBooNE FHC numu \n";

  // Common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("Q^{2} (GeV^{2})");
  fSettings.SetYTitle("Entries/0.05 (GeV^{2})");
  fSettings.SetAllowedTypes("EVT");
  fSettings.SetEnuRange(0.0, 10.0);
  fSettings.DefineAllowedTargets("C,H");

  fSettings.SetTitle("SciBooNE CCCOH 1TRK");
  fSettings.SetDataInput(  FitPar::GetDataBase()+"/SciBooNE/SB_COH_Fig10a_CVs.csv");
  fSettings.SetHasExtraHistograms(true);
  fSettings.DefineAllowedSpecies("numu");

  SetDataFromTextFile(fSettings.GetDataInput());
  FinaliseSampleSettings();

  // Setup Plots
  this->muonStopEff = PlotUtils::GetTH2DFromRootFile(
		    FitPar::GetDataBase()+"/SciBooNE/SciBooNE_stopped_muon_eff_nu.root", "stopped_muon_eff");

  this->fMCStack  = new SciBooNEUtils::ModeStack(fSettings.Name() + "_Stack",
						 "Mode breakdown" + fSettings.PlotTitles(),
						 PlotUtils::GetTH1DFromFile(fSettings.GetDataInput(), fSettings.GetName()));  
  SetAutoProcessTH1(fMCStack);
  
  // Estimate the number of CH molecules in SciBooNE...
  double nTargets = 10.6E6/13.*6.022E23;
  this->fScaleFactor = GetEventHistogram()->Integral()*1E-38*13./double(fNEvents)*nTargets;
  FinaliseMeasurement();

};

void SciBooNE_CCCOH_1TRK_1DQ2_nu::FillEventVariables(FitEvent *event){

  if (event->NumFSParticle(PhysConst::pdg_muons) == 0) return;

  FitParticle *muon = event->GetHMFSParticle(PhysConst::pdg_muons);
  FitParticle *nu   = event->GetNeutrinoIn();

  // Need to figure out if this is the best place to set weights long term... but...
  if (SciBooNEUtils::StoppedEfficiency(this->muonStopEff, nu, muon) > 
      SciBooNEUtils::PenetratedEfficiency(nu, muon)){
    this->Weight *= SciBooNEUtils::StoppedEfficiency(this->muonStopEff, nu, muon);
    q2qe = FitUtils::Q2QErec(FitUtils::p(muon),cos(FitUtils::th(nu,muon)), 27., true);
  } 

  if (q2qe < 0) return;
  
  // Set X Variables
  fXVar = q2qe;
  return;
};


bool SciBooNE_CCCOH_1TRK_1DQ2_nu::isSignal(FitEvent *event){
  if (fXVar == 0) return false;
  return SciBooNEUtils::is1TRK(event);
};

void SciBooNE_CCCOH_1TRK_1DQ2_nu::FillExtraHistograms(MeasurementVariableBox* vars, double weight){
  
  if (Signal) fMCStack->Fill(Mode, fXVar, weight);
  return;
}


// void SciBooNE_CCCOH_1TRK_1DQ2_nu::ScaleEvents(){

//   if (fScaleFactor < 0) {
//     ERR(FTL) << "I found a negative fScaleFactor in " << __FILE__ << ":" << __LINE__ << std::endl;
//     ERR(FTL) << "fScaleFactor = " << fScaleFactor << std::endl;
//     ERR(FTL) << "EXITING" << std::endl;
//     exit(-1);
//   }
  
//   LOG(REC) << std::setw(10) << std::right << NSignal << "/"
// 	   << fNEvents << " events passed selection + binning after reweight"
// 	   << std::endl;

//   fMCHist->Scale(fScaleFactor);
//   fMCFine->Scale(fMCHist->Integral("width")/double(fMCFine->Integral()), "width");
//   PlotUtils::ScaleNeutModeArray((TH1**)fMCHist_PDG, fScaleFactor);
//   SciBooNEUtils::ScaleModeArray((TH1**)fMCHist_modes, fScaleFactor);

//   return;
// }

// void SciBooNE_CCCOH_1TRK_1DQ2_nu::FillHistograms(){
  
//   // This was annoying for a while...
//   if (Signal)
//     SciBooNEUtils::FillModeArray((TH1**)fMCHist_modes, Mode, fXVar, this->Weight);
//   Measurement1D::FillHistograms();

//   return;
// }

// void SciBooNE_CCCOH_1TRK_1DQ2_nu::Write(std::string drawOpt){

//   SciBooNEUtils::WriteModeArray((TH1**)fMCHist_modes);
//   Measurement1D::Write(drawOpt);

//   return;
// }

// void SciBooNE_CCCOH_1TRK_1DQ2_nu::ApplyNormScale(double norm){

//   Measurement1D::ApplyNormScale(norm);
//   SciBooNEUtils::ScaleModeArray((TH1**)fMCHist_modes, 1.0/norm, "");

//   return;
// }

// void SciBooNE_CCCOH_1TRK_1DQ2_nu::ResetAll(){
  
//   Measurement1D::ResetAll();
//   SciBooNEUtils::ResetModeArray((TH1**)fMCHist_modes);
//   return;
// }
    


