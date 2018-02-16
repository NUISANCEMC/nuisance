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

#include "SciBooNE_CCCOH_MuPiVA_1Dthetamu_nu.h"

SciBooNE_CCCOH_MuPiVA_1Dthetamu_nu::SciBooNE_CCCOH_MuPiVA_1Dthetamu_nu(nuiskey samplekey){

  // Sample overview
  std::string descrip = "SciBooNE CC-coherent 1pion no VA.\n" \
    "Target: CH \n"                                                     \
    "Flux: SciBooNE FHC numu \n";

  // Common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("cos#theta_{#mu}");
  fSettings.SetYTitle("Entries");
  this->SetFitOptions("NOWIDTH");
  fSettings.SetEnuRange(0.0, 10.0);
  fSettings.DefineAllowedTargets("C,H");

  fSettings.SetTitle("SciBooNE CCCOH proton");
  fSettings.SetDataInput(  FitPar::GetDataBase()+"/SciBooNE/SB_COH_Fig7.30c_thetamu.csv");
  fSettings.SetHasExtraHistograms(true);
  fSettings.DefineAllowedSpecies("numu");

  SetDataFromTextFile(fSettings.GetDataInput());
  FinaliseSampleSettings();

  // Setup Plots
  if (SciBooNEUtils::GetUseZackEff()) 
    this->muonStopEff = PlotUtils::GetTH2DFromRootFile(FitPar::GetDataBase()+"/SciBooNE/SciBooNE_stopped_muon_eff_nu_ZACK.root", "Ratio2DBSCC");
  else this->muonStopEff = PlotUtils::GetTH2DFromRootFile(FitPar::GetDataBase()+"/SciBooNE/SciBooNE_stopped_muon_eff_nu.root", "stopped_muon_eff");
  this->protonEff   = PlotUtils::GetTH2DFromRootFile(FitPar::GetDataBase()+"/SciBooNE/SciBooNE_proton_nu.root", "Ratio2DRS");

  this->fMCStack  = new SciBooNEUtils::ModeStack2(fSettings.Name() + "_Stack",
                                                 "Mode breakdown" + fSettings.PlotTitles(),
                                                 PlotUtils::GetTH1DFromFile(fSettings.GetDataInput(), fSettings.GetName()));
  this->fPIDStack  = new SciBooNEUtils::MainPIDStack(fSettings.Name() + "_MainPID",
						     "Main PID" + fSettings.PlotTitles(),
						     PlotUtils::GetTH1DFromFile(fSettings.GetDataInput(), fSettings.GetName()));
  SetAutoProcessTH1(fMCStack);
  SetAutoProcessTH1(fPIDStack);

  double nTargets = 10.6E6/13.*6.022E23;
  this->fScaleFactor = GetEventHistogram()->Integral()*13.*1E-38/double(fNEvents)*nTargets;

  FinaliseMeasurement();

};

void SciBooNE_CCCOH_MuPiVA_1Dthetamu_nu::FillEventVariables(FitEvent *event){

  thetamu = 0;
  this->mainIndex = SciBooNEUtils::GetMainTrack(event, this->muonStopEff, this->protonEff, this->mainTrack, this->Weight);
  SciBooNEUtils::GetOtherTrackInfo(event, this->mainIndex, this->nProtons, this->nPiMus, this->nVertex, this->secondTrack);
  FitParticle *nu   = event->GetNeutrinoIn();

  if (this->mainTrack){
    thetamu = cos(SciBooNEUtils::smear_th(nu,this->mainTrack));
  }

  if (thetamu < 0) thetamu = 0;//return;
  // Set X Variables
  fXVar = thetamu;
  return;
};


bool SciBooNE_CCCOH_MuPiVA_1Dthetamu_nu::isSignal(FitEvent *event){

  if (!this->mainTrack || !this->secondTrack) return false;
  if (this->nPiMus + this->nProtons != 1) return false;
  if (this->nVertex == 0) return false;

  double misIDProb = SciBooNEUtils::ProtonMisIDProb(FitUtils::p(this->secondTrack));

  if (SciBooNEUtils::isProton(this->mainTrack)) this->Weight *= 0.1;
  if (this->nProtons == 1) this->Weight *= misIDProb;
  if (this->nPiMus == 1)   this->Weight *= (1 - misIDProb);
  return true;
};


void SciBooNE_CCCOH_MuPiVA_1Dthetamu_nu::FillExtraHistograms(MeasurementVariableBox* vars, double weight){

  if (Signal){
    fMCStack->Fill(Mode, fXVar, weight);
    fPIDStack->Fill(this->mainTrack->fPID, fXVar, weight);
  }
  return;
};
