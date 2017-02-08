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

#include "SciBooNE_CCCOH_MuPiNoVA_1Dthetapr_nu.h"

SciBooNE_CCCOH_MuPiNoVA_1Dthetapr_nu::SciBooNE_CCCOH_MuPiNoVA_1Dthetapr_nu(std::string name, std::string inputfile,
							 FitWeight *rw, std::string type, 
							 std::string fakeDataFile){
  // Measurement Details
  fName = name;
  fDefaultTypes = "DIAG";
  fAllowedTypes += "EVT";
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Setup Plots
  this->fPlotTitles = "; #Delta #theta_{p} (degrees); Entries/5 degrees";
  this->SetDataValues(FitPar::GetDataBase()+"/SciBooNE/SB_COH_Fig11_CVs.csv");
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

void SciBooNE_CCCOH_MuPiNoVA_1Dthetapr_nu::FillEventVariables(FitEvent *event){

  if (event->NumFSParticle(PhysConst::pdg_muons) == 0) return;

  FitParticle *muon = event->GetHMFSParticle(PhysConst::pdg_muons);
  FitParticle *nu   = event->GetNeutrinoIn();

  
  thetapr = SciBooNEUtils::CalcThetaPr(event);

  if (thetapr < 0) return;

  // Need to figure out if this is the best place to set weights long term... but...
  this->Weight *= SciBooNEUtils::CalcEfficiency(this->muonStopEff, nu, muon);
  
  // Set X Variables
  fXVar = thetapr;
  return;
};


bool SciBooNE_CCCOH_MuPiNoVA_1Dthetapr_nu::isSignal(FitEvent *event){

  if (SciBooNEUtils::isMuPiSignal(event, false)) return true;

  // Also include 10% of protons
  if (SciBooNEUtils::isMuPrSignal(event, false)){
    this->Weight*=0.1;
    return true;
  }

  return false;
};


void SciBooNE_CCCOH_MuPiNoVA_1Dthetapr_nu::ScaleEvents(){

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

void SciBooNE_CCCOH_MuPiNoVA_1Dthetapr_nu::FillHistograms(){

  // This was annoying for a while...
  if (Signal)
    SciBooNEUtils::FillModeArray((TH1**)fMCHist_modes, Mode, fXVar, this->Weight);
  Measurement1D::FillHistograms();

  return;
}

void SciBooNE_CCCOH_MuPiNoVA_1Dthetapr_nu::Write(std::string drawOpt){

  SciBooNEUtils::WriteModeArray((TH1**)fMCHist_modes);
  Measurement1D::Write(drawOpt);

  return;
}

void SciBooNE_CCCOH_MuPiNoVA_1Dthetapr_nu::ApplyNormScale(double norm){

  Measurement1D::ApplyNormScale(norm);
  SciBooNEUtils::ScaleModeArray((TH1**)fMCHist_modes, 1.0/norm, "");

  return;
}

void SciBooNE_CCCOH_MuPiNoVA_1Dthetapr_nu::ResetAll(){

  Measurement1D::ResetAll();
  SciBooNEUtils::ResetModeArray((TH1**)fMCHist_modes);
  return;
}

