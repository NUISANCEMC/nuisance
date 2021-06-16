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

#include "SciBooNE_CCCOH_MuPiNoVA_1Dthetapr_nu.h"

SciBooNE_CCCOH_MuPiNoVA_1Dthetapr_nu::SciBooNE_CCCOH_MuPiNoVA_1Dthetapr_nu(nuiskey samplekey){

  // Sample overview
  std::string descrip = "SciBooNE CC-coherent 1 pion no VA #theta_{pr}.\n" \
    "Target: CH \n"                                                     \
    "Flux: SciBooNE FHC numu \n";

  // Common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("#Delta #theta_{p} (degrees)");
  fSettings.SetYTitle("Entries/5 degrees");
  this->SetFitOptions("NOWIDTH");
  fSettings.SetEnuRange(0.0, 10.0);
  fSettings.DefineAllowedTargets("C,H");

  fSettings.SetTitle("SciBooNE CCCOH pion no VA #theta_{pr}");
  fSettings.SetDataInput(  FitPar::GetDataBase()+"/SciBooNE/SB_COH_Fig11_CVs.csv");
  fSettings.SetHasExtraHistograms(true);
  fSettings.DefineAllowedSpecies("numu");

  SetDataFromTextFile(fSettings.GetDataInput());
  FinaliseSampleSettings();

  // Setup Plots
  if (SciBooNEUtils::GetUseZackEff())
    this->muonStopEff = PlotUtils::GetTH2DFromRootFile(FitPar::GetDataBase()+"/SciBooNE/SciBooNE_stopped_muon_eff_nu_ZACK.root", "Ratio2DBSCC");
  else this->muonStopEff = PlotUtils::GetTH2DFromRootFile(FitPar::GetDataBase()+"/SciBooNE/SciBooNE_stopped_muon_eff_nu.root", "stopped_muon_eff");
  this->protonEff   = PlotUtils::GetTH2DFromRootFile(FitPar::GetDataBase()+"/SciBooNE/SciBooNE_proton_nu.root", "Ratio2DRS");

  this->fMCStack  = new SciBooNEUtils::ModeStack(fSettings.Name() + "_Stack",
                                                 "Mode breakdown" + fSettings.PlotTitles(),
                                                 PlotUtils::GetTH1DFromFile(fSettings.GetDataInput(), fSettings.GetName()));
  this->fPIDStack  = new SciBooNEUtils::MainPIDStack(fSettings.Name() + "_MainPID",
						     "Main PID" + fSettings.PlotTitles(),
						     PlotUtils::GetTH1DFromFile(fSettings.GetDataInput(), fSettings.GetName()+"_MainPID"));
  SetAutoProcessTH1(fMCStack);
  SetAutoProcessTH1(fPIDStack);

  double nTargets = 10.6E6/1.6749E-27*1E-3;
  double pot = FitPar::Config().GetParD("SciBooNEScale");
  this->fScaleFactor = GetEventHistogram()->Integral()*1E-38/double(fNEvents)*nTargets*pot;

  FinaliseMeasurement();

};

void SciBooNE_CCCOH_MuPiNoVA_1Dthetapr_nu::FillEventVariables(FitEvent *event){

  thetapr = -999;
  this->mainIndex = SciBooNEUtils::GetMainTrack(event, this->muonStopEff, this->protonEff, this->mainTrack, this->Weight);
  SciBooNEUtils::GetOtherTrackInfo(event, this->mainIndex, this->nProtons, this->nPiMus, this->nVertex, this->secondTrack);

  thetapr = SciBooNEUtils::CalcThetaPr(event, this->mainTrack, this->secondTrack);

  if (thetapr < 0) {
    //std::cout << "Error: theta_pr = " << thetapr << std::endl;
    return;
  }
  // Set X Variables
  fXVar = thetapr;
  return;
};


bool SciBooNE_CCCOH_MuPiNoVA_1Dthetapr_nu::isSignal(FitEvent *event){
  
  if (!this->mainTrack || !this->secondTrack) return false;
  if (this->nPiMus + this->nProtons != 1) return false;
  if (this->nVertex != 0) return false;

  double misIDProb = SciBooNEUtils::ProtonMisIDProb(FitUtils::p(this->secondTrack));

  if (SciBooNEUtils::isProton(this->mainTrack)) this->Weight *= SciBooNEUtils::ProtonMisIDProb(FitUtils::p(this->mainTrack));
  if (this->nProtons == 1) this->Weight *= misIDProb;
  if (this->nPiMus == 1)   this->Weight *= (1 - misIDProb);
  return true;
};


void SciBooNE_CCCOH_MuPiNoVA_1Dthetapr_nu::FillExtraHistograms(MeasurementVariableBox* vars, double weight){

  if (Signal){
    fMCStack->Fill(Mode, fXVar, weight);
    fPIDStack->Fill(this->mainTrack->fPID, fXVar, weight);
  }
  return;
};


