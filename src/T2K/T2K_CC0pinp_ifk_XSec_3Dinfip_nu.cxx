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

#include "T2K_SignalDef.h"

#include "T2K_CC0pinp_ifk_XSec_3Dinfip_nu.h"



//********************************************************************
T2K_CC0pinp_ifk_XSec_3Dinfip_nu::T2K_CC0pinp_ifk_XSec_3Dinfip_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "T2K_CC0pinp_ifk_XSec_3Dinfip_nu sample. \n" \
                        "Target: CH \n" \
                        "Flux: T2K 2.5 degree off-axis (ND280)  \n" \
                        "Signal: CC0piNp (N>=1) with p_p>450MeV and cthp>0.4 \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("|#Delta p|");
  fSettings.SetYTitle("p_#mu");
  fSettings.SetZTitle("cos#theta_{#mu}");
  //fSettings.SetZTitle("d^{2}#sigma/dP_{#mu}dcos#theta_{#mu} (cm^{2}/GeV)");
  fSettings.SetAllowedTypes("FULL,DIAG/FREE,SHAPE,FIX/SYSTCOV/STATCOV","FIX/FULL");
  fSettings.SetEnuRange(0.0, 10.0);
  fSettings.DefineAllowedTargets("C,H");

  fAnalysis = 1;
 
  outOfBoundsMC = 0.0;

  // CCQELike plot information
  fSettings.SetTitle("T2K_CC0pinp_ifk_XSec_3Dinfip_nu");
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = ((GetEventHistogram()->Integral("width")/(fNEvents+0.)) / (TotalIntegratedFlux()));
  //fScaleFactor = ((GetEventHistogram()->Integral("width")/(fNEvents+0.)) * 10 / (TotalIntegratedFlux()));

  fSettings.SetDataInput(  FitPar::GetDataBase() + "/T2K/CC0pi/infkResults_origBin.root;result_tp" );
  SetDataFromRootFile( fSettings.GetDataInput() );

  fSettings.SetCovarInput( FitPar::GetDataBase() + "/T2K/CC0pi/infkResults_origBin.root;cor_tp" );
  SetCorrelationFromRootFile(fSettings.GetCovarInput() );
  //SetCovarFromRootFile(FitPar::GetDataBase() + "/T2K/CC0pi/infkResults_origBin.root", "cov_tp" );              

  // Setup Histograms
  SetHistograms();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};


bool T2K_CC0pinp_ifk_XSec_3Dinfip_nu::isSignal(FitEvent *event){
  return SignalDef::isT2K_CC0pi_ifk(event, EnuMin, EnuMax);
};

void T2K_CC0pinp_ifk_XSec_3Dinfip_nu::FillEventVariables(FitEvent* event){

  if (event->NumFSParticle(13) == 0 || event->NumFSParticle(2212) == 0)
    return;

  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector Pp  = event->GetHMFSParticle(2212)->fP;

  double pmu = Pmu.Vect().Mag()/1000.;
  double pp = Pp.Vect().Mag()/1000.;
  double CosThetaMu = cos(Pnu.Vect().Angle(Pmu.Vect()));
  TVector3 tp_inf = FitUtils::tppInfK(Pmu, CosThetaMu, 25, true);

  TVector3 Pp_mev(Pp.X()/1000,Pp.Y()/1000,Pp.Z()/1000);

  TVector3 delta_tp = tp_inf-Pp_mev;

  //std::cout << "Proton 3 mom is: " << std::endl;
  //(Pp.Vect()).Print("all");
  //std::cout << "Inferred Proton 3 mom is: " << std::endl;
  //tp_inf.Print("all");
  //std::cout << " " << std::endl;

  fXVar = delta_tp.Mag();
  fYVar = pmu;
  fZVar = CosThetaMu;

  return;
};

void T2K_CC0pinp_ifk_XSec_3Dinfip_nu::FillHistograms(){

  Measurement1D::FillHistograms();
  if (Signal){
    FillMCSlice( fXVar, fYVar, fZVar, Weight );
  }

}


void T2K_CC0pinp_ifk_XSec_3Dinfip_nu::ConvertEventRates(){

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

void T2K_CC0pinp_ifk_XSec_3Dinfip_nu::FillMCSlice(double x, double y, double z, double w){
  // x is delta_tp
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


void T2K_CC0pinp_ifk_XSec_3Dinfip_nu::SetHistograms(){

  // Read in 1D Data Histograms
  fInputFile = new TFile( (FitPar::GetDataBase() + "/T2K/CC0pi/infkResults_origBin.root").c_str(),"READ");
  fInputFile->ls();
  
  // Read in 1D Data
  fDataHist = (TH1D*) fInputFile->Get("result_tp");
  fDataHist->SetNameTitle("T2K_CC0pinp_ifk_XSec_3Dinfip_nu_data", "T2K_CC0pinp_ifk_XSec_3Dinfip_nu_data");
  SetAutoProcessTH1(fDataHist,kCMD_Write);
  
  // Read in 2D Data Slices and Make MC Slices
  for (int i = 0; i < 7; i++){ //both y and z slices  
    // Get Data Histogram
    fInputFile->ls();
    fDataHist_Slices.push_back((TH1D*)fInputFile->Get(Form("resultBin%i_tp",i))->Clone());
    fDataHist_Slices[i]->SetNameTitle(Form("T2K_CC0pinp_ifk_XSec_3Dinfip_nu_data_Slice%i",i), (Form("T2K_CC0pinp_ifk_XSec_3Dinfip_nu_data_Slice%i",i)));
    // Make MC Clones
    fMCHist_Slices.push_back((TH1D*) fDataHist_Slices[i]->Clone());
    fMCHist_Slices[i]->SetNameTitle(Form("T2K_CC0pinp_ifk_XSec_3Dinfip_nu_MC_Slice%i",i), (Form("T2K_CC0pinp_ifk_XSec_3Dinfip_nu_MC_Slice%i",i)));

    SetAutoProcessTH1(fDataHist_Slices[i],kCMD_Write);
    SetAutoProcessTH1(fMCHist_Slices[i]);
    fMCHist_Slices[i]->Reset();
  }
  return;
};
