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

#include "SciBooNE_CCCOH_STOP_NTrks_nu.h"

SciBooNE_CCCOH_STOP_NTrks_nu::SciBooNE_CCCOH_STOP_NTrks_nu(std::string name, std::string inputfile,
							   FitWeight *rw, std::string type, 
							   std::string fakeDataFile){
  // Measurement Details
  fName = name;
  fDefaultTypes = "DIAG";
  fAllowedTypes += "EVT";
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Setup Plots
  this->fPlotTitles = "; Q^{2} (GeV^{2}); Entries/0.05 (GeV^{2})";
  this->SetDataValues(FitPar::GetDataBase()+"/SciBooNE/SB_COH_Fig7_CVs.csv");
  this->muonStopEff = (TH2D*)PlotUtils::GetHistFromRootFile(
		    FitPar::GetDataBase()+"/SciBooNE/SciBooNE_stopped_muon_eff_nu.root", "stopped_muon_eff");
  this->SetupDefaultHist();

  PlotUtils::CreateNeutModeArray((TH1D*)this->fMCHist,(TH1**)this->fMCHist_PDG);
  PlotUtils::ResetNeutModeArray((TH1**)this->fMCHist_PDG);
  
  SciBooNEUtils::CreateModeArray((TH1D*)this->fMCHist,(TH1**)this->fMCHist_modes);
  SciBooNEUtils::ResetModeArray((TH1**)this->fMCHist_modes);

  // Estimate the number of CH molecules in SciBooNE...
  double nTargets = 10.6E6/13.*6.022E23;
  this->fScaleFactor = GetEventHistogram()->Integral()*1E-38*13./double(fNEvents)*nTargets;

};

void SciBooNE_CCCOH_STOP_NTrks_nu::FillEventVariables(FitEvent *event){

  if (event->NumFSParticle(PhysConst::pdg_muons) == 0) return;

  FitParticle *muon = event->GetHMFSParticle(PhysConst::pdg_muons);
  FitParticle *nu   = event->GetNeutrinoIn();

  this->Weight *= SciBooNEUtils::StoppedEfficiency(this->muonStopEff, nu, muon);
  nTrks = SciBooNEUtils::GetNTracks(event);

  // Set X Variables
  fXVar = nTrks;
  return;
};


bool SciBooNE_CCCOH_STOP_NTrks_nu::isSignal(FitEvent *event){

  // Just require that there is a final state muon
  if (event->NumFSParticle(PhysConst::pdg_muons) != 1)
    return false;
  return true;
};

void SciBooNE_CCCOH_STOP_NTrks_nu::ScaleEvents(){

  if (fScaleFactor < 0) {
    ERR(FTL) << "I found a negative fScaleFactor in " << __FILE__ << ":" << __LINE__ << std::endl;
    ERR(FTL) << "fScaleFactor = " << fScaleFactor << std::endl;
    ERR(FTL) << "EXITING" << std::endl;
    exit(-1);
  }
  
  LOG(REC) << std::setw(10) << std::right << NSignal << "/"
	   << fNEvents << " events passed selection + binning after reweight"
	   << std::endl;

  fMCHist->Scale(fScaleFactor);
  fMCFine->Scale(fMCHist->Integral("width")/double(fMCFine->Integral()), "width");
  PlotUtils::ScaleNeutModeArray((TH1**)fMCHist_PDG, fScaleFactor);
  SciBooNEUtils::ScaleModeArray((TH1**)fMCHist_modes, fScaleFactor);

  return;
}

void SciBooNE_CCCOH_STOP_NTrks_nu::FillHistograms(){
  
  // This was annoying for a while...
  if (Signal)
    SciBooNEUtils::FillModeArray((TH1**)fMCHist_modes, Mode, fXVar, this->Weight);
  Measurement1D::FillHistograms();

  return;
}

void SciBooNE_CCCOH_STOP_NTrks_nu::Write(std::string drawOpt){

  SciBooNEUtils::WriteModeArray((TH1**)fMCHist_modes);
  Measurement1D::Write(drawOpt);

  return;
}

void SciBooNE_CCCOH_STOP_NTrks_nu::ApplyNormScale(double norm){

  Measurement1D::ApplyNormScale(norm);
  SciBooNEUtils::ScaleModeArray((TH1**)fMCHist_modes, 1.0/norm, "");

  return;
}

void SciBooNE_CCCOH_STOP_NTrks_nu::ResetAll(){
  
  Measurement1D::ResetAll();
  SciBooNEUtils::ResetModeArray((TH1**)fMCHist_modes);
  return;
}
    


