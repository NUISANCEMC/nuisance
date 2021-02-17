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

#include "T2K_SignalDef.h"

#include "T2K_CC0pinp_ifk_XSec_3Dinfp_nu.h"



//********************************************************************
T2K_CC0pinp_ifk_XSec_3Dinfp_nu::T2K_CC0pinp_ifk_XSec_3Dinfp_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "T2K_CC0pinp_ifk_XSec_3Dinfp_nu sample. \n" \
                        "Target: CH \n" \
                        "Flux: T2K 2.5 degree off-axis (ND280)  \n" \
                        "Signal: CC0piNp (N>=1) with p_p>450MeV and cthp>0.4 \n"
                        "https://doi.org/10.1103/PhysRevD.98.032003 \n";
  
  // This sample corresponds to the #Delta p variable
  // #Delta p = |p_{p}^{measured}| - |p_{p}^{inferred}|

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("#Delta p-p_{#mu}-cos#theta_{#mu}");
  fSettings.SetYTitle("d^{2}#sigma/d#Delta pdp_{#mu}dcos#theta_{#mu} (cm^{2}/GeV^{2})");
  fSettings.SetAllowedTypes("FULL,DIAG/FREE,SHAPE,FIX/SYSTCOV/STATCOV","FIX/FULL");
  fSettings.SetEnuRange(0.0, 10.0);
  fSettings.DefineAllowedTargets("C,H");

  fAnalysis = 1;

  outOfBoundsMC = 0.0;

  // CCQELike plot information
  fSettings.SetTitle("T2K_CC0pinp_ifk_XSec_3Dinfp_nu");
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = ((GetEventHistogram()->Integral("width")/(fNEvents+0.)) / (TotalIntegratedFlux()));

  fSettings.SetDataInput(  FitPar::GetDataBase() + "/T2K/CC0pi/STV/infkResults_origBin.root;result_p" );
  SetDataFromRootFile( fSettings.GetDataInput() );

  fSettings.SetCovarInput( FitPar::GetDataBase() + "/T2K/CC0pi/STV/infkResults_origBin.root;cor_p" );
  SetCorrelationFromRootFile(fSettings.GetCovarInput() );

  // Setup Histograms
  SetHistograms();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

  fSaveFine = false;
};


bool T2K_CC0pinp_ifk_XSec_3Dinfp_nu::isSignal(FitEvent *event){
  return SignalDef::isT2K_CC0pi_ifk(event, EnuMin, EnuMax);
};

void T2K_CC0pinp_ifk_XSec_3Dinfp_nu::FillEventVariables(FitEvent* event){

  if (event->NumFSParticle(13) == 0 || event->NumFSParticle(2212) == 0)
    return;

  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector Pp  = event->GetHMFSParticle(2212)->fP;

  double pmu = Pmu.Vect().Mag()/1000.;
  double CosThetaMu = cos(Pnu.Vect().Angle(Pmu.Vect()));
  double delta_p = Pp.Vect().Mag()/1000. - FitUtils::ppInfK(Pmu, CosThetaMu, 25, true);

  fXVar = delta_p;
  fYVar = pmu;
  fZVar = CosThetaMu;

  return;
};

void T2K_CC0pinp_ifk_XSec_3Dinfp_nu::FillHistograms(){

  Measurement1D::FillHistograms();
  if (Signal){
    FillMCSlice( fXVar, fYVar, fZVar, Weight );
  }

}


void T2K_CC0pinp_ifk_XSec_3Dinfp_nu::ConvertEventRates(){

  for (int i = 0; i < 7; i++){
    fMCHist_Slices[i]->GetSumw2();
  }

  // Do standard conversion.
  Measurement1D::ConvertEventRates();

  // First scale MC slices also by their width in Y and Z
  //MCHist_Slices[0]->Scale(1.0 / 1.00);
  //MCHist_Slices[1]->Scale(1.0 / 0.60);
  //MCHist_Slices[2]->Scale(1.0 / 0.10);
  //MCHist_Slices[3]->Scale(1.0 / 0.10);


  // Now Convert into 1D list
  fMCHist->Reset();
  //The first bin in the histogram in underflow, so set this and start bincount at 1
  fMCHist->SetBinContent(1, outOfBoundsMC);
  int bincount = 1; 
  for (int i = 0; i < 7; i++){
    for (int j = 0; j < fDataHist_Slices[i]->GetNbinsX(); j++){
      fMCHist->SetBinContent(bincount+1, fMCHist_Slices[i]->GetBinContent(j+1));
      //fMCHist->SetBinError(bincount+1, fMCHist_Slices[i]->GetBinError(j+1));
      bincount++;
    }
  }

  return;
}

void T2K_CC0pinp_ifk_XSec_3Dinfp_nu::FillMCSlice(double x, double y, double z, double w){
  // x is delta_p
  // y is pmu
  // z is CosThetaMu

  if      (z <= -0.6) fMCHist_Slices[0]->Fill(x,w);
  else if (z >= -0.6 and z < 0.0 and y < 0.25)  fMCHist_Slices[1]->Fill(x,w);
  else if (z >= -0.6 and z < 0.0 and y > 0.25)  fMCHist_Slices[2]->Fill(x,w);
  else if (z >= 0.0 and y < 0.25)  fMCHist_Slices[3]->Fill(x,w);
  else if (z >= 0.0 and z < 0.8 and y >= 0.25)  fMCHist_Slices[4]->Fill(x,w);
  else if (z >= 0.8 and z < 1.0 and y >= 0.25 and y < 0.75)  fMCHist_Slices[5]->Fill(x,w);
  else if (z >= 0.8 and z < 1.0 and y >= 0.75)  fMCHist_Slices[6]->Fill(x,w);
  else outOfBoundsMC += w;

}


void T2K_CC0pinp_ifk_XSec_3Dinfp_nu::SetHistograms(){

  std::string name = fSettings.GetName();
  
  // Read in 1D Data Histograms
  fInputFile = new TFile( (FitPar::GetDataBase() + "/T2K/CC0pi/STV/infkResults_origBin.root").c_str(),"READ");
  
  // Read in 2D Data Slices and Make MC Slices
  for (int i = 0; i < 7; i++){ //both y and z slices  
    // Get Data Histogram
    fDataHist_Slices.push_back((TH1D*)fInputFile->Get(Form("resultBin%i_p",i))->Clone());
    fDataHist_Slices[i]->SetNameTitle(Form("%s_data_Slice%i", name.c_str(), i), 
				      Form("%s_data_Slice%i;#Delta p (GeV);%s", 
					   name.c_str(), i, fSettings.GetYTitle().c_str()));
    // Make MC Clones
    fMCHist_Slices.push_back((TH1D*) fDataHist_Slices[i]->Clone());
    fMCHist_Slices[i]->SetNameTitle(Form("%s_MC_Slice%i",name.c_str(), i), 
				    Form("%s_MC_Slice%i;#Delta p (GeV);%s", 
					 name.c_str(), i, fSettings.GetYTitle().c_str()));

    SetAutoProcessTH1(fDataHist_Slices[i],kCMD_Write);
    SetAutoProcessTH1(fMCHist_Slices[i]);
    fMCHist_Slices[i]->Reset();
  }
  return;
};
