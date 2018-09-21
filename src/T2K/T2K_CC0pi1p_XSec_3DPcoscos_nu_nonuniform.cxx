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

#include "T2K_CC0pi1p_XSec_3DPcoscos_nu_nonuniform.h"



//********************************************************************
T2K_CC0pi1p_XSec_3DPcoscos_nu_nonuniform::T2K_CC0pi1p_XSec_3DPcoscos_nu_nonuniform(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "T2K_CC0pi1p_XSec_3DPcoscos_nu_nonuniform sample. \n" \
                        "Target: CH \n" \
                        "Flux: T2K 2.5 degree off-axis (ND280)  \n" \
                        "Signal: CC0piNp (N>=1) with p_p>500MeV \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("P_{p} (GeV)");
  fSettings.SetYTitle("cos#theta_{p}");
  fSettings.SetZTitle("cos#theta_{#mu}");
  //fSettings.SetZTitle("d^{2}#sigma/dP_{#mu}dcos#theta_{#mu} (cm^{2}/GeV)");
  fSettings.SetAllowedTypes("FULL,DIAG/FREE,SHAPE,FIX/SYSTCOV/STATCOV","FIX/FULL");
  fSettings.SetEnuRange(0.0, 10.0);
  fSettings.DefineAllowedTargets("C,H");

  fAnalysis = 1;
 

  // CCQELike plot information
  fSettings.SetTitle("T2K_CC0pi1p_XSec_3DPcoscos_nu_nonuniform");
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  //fScaleFactor = ((GetEventHistogram()->Integral("width")/(fNEvents+0.)) * 1E-38 / (TotalIntegratedFlux()));
  fScaleFactor = ((GetEventHistogram()->Integral("width")/(fNEvents+0.)) * 10 / (TotalIntegratedFlux()));

  // Setup Histograms
  SetHistograms();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};


bool T2K_CC0pi1p_XSec_3DPcoscos_nu_nonuniform::isSignal(FitEvent *event){
  return SignalDef::isT2K_CC0pi1p(event, EnuMin, EnuMax);
};

void T2K_CC0pi1p_XSec_3DPcoscos_nu_nonuniform::FillEventVariables(FitEvent* event){

  if (event->NumFSParticle(13) == 0 || event->NumFSParticle(2212) == 0)
    return;

  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector Pp  = event->GetHMFSParticle(2212)->fP;

  double pmu = Pmu.Vect().Mag()/1000.;
  double pp = Pp.Vect().Mag()/1000.;
  double CosThetaMu = cos(Pnu.Vect().Angle(Pmu.Vect()));
  double CosThetaP = cos(Pnu.Vect().Angle(Pp.Vect()));

  fXVar = pp;
  fYVar = CosThetaP;
  fZVar = CosThetaMu;

  return;
};

void T2K_CC0pi1p_XSec_3DPcoscos_nu_nonuniform::FillHistograms(){

  Measurement1D::FillHistograms();
  if (Signal){
    fMCHist_Fine2D->Fill( fXVar, fYVar, Weight );
    FillMCSlice( fXVar, fYVar, fZVar, Weight );

  }

}


// Modification is needed after the full reconfigure to move bins around
// Otherwise this would need to be replaced by a TH2Poly which is too awkward.
void T2K_CC0pi1p_XSec_3DPcoscos_nu_nonuniform::ConvertEventRates(){

  for (int i = 0; i < 4; i++){
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
  int bincount = 0;
  for (int i = 0; i < 4; i++){
    for (int j = 0; j < fDataHist_Slices[i]->GetNumberOfBins(); j++){
      fMCHist->SetBinContent(bincount+1, fMCHist_Slices[i]->GetBinContent(j+1));
      //fMCHist->SetBinError(bincount+1, fMCHist_Slices[i]->GetBinError(j+1));
      bincount++;
    }
  }

  return;
}

void T2K_CC0pi1p_XSec_3DPcoscos_nu_nonuniform::FillMCSlice(double x, double y, double z, double w){

  if      (z >= -1.0 and z < -0.3) fMCHist_Slices[0]->Fill(y,x,w);
  else if (z >= -0.3 and z < 0.3)  fMCHist_Slices[1]->Fill(y,x,w);
  else if (z >= 0.3  and z < 0.8)  fMCHist_Slices[2]->Fill(y,x,w);
  else if (z >= 0.8  and z < 1.0)  fMCHist_Slices[3]->Fill(y,x,w);
}


void T2K_CC0pi1p_XSec_3DPcoscos_nu_nonuniform::SetHistograms(){

  // Read in 1D Data Histograms
  fInputFile = new TFile( (FitPar::GetDataBase() + "/T2K/CC0pi/multidif_3D_pcoscos.root").c_str(),"READ");
  fInputFile->ls();
  
  // Read in 1D Data
  fDataHist = (TH1D*) fInputFile->Get("LinResult");

  fMCHist_Fine2D = new TH2D("T2K_CC0pi1p_XSec_3DPcoscos_nu_nonuniform_Fine2D","T2K_CC0pi1p_XSec_3DPcoscos_nu_nonuniform_Fine2D", 400, 0.0,30.0,100,-1.0,1.0);
  SetAutoProcessTH1(fMCHist_Fine2D);
  
  TH2D* tempcov = (TH2D*) fInputFile->Get("CovMatrix");

  fFullCovar = new TMatrixDSym(fDataHist->GetNbinsX());
  for (int i = 0; i < fDataHist->GetNbinsX(); i++){
    for (int j = 0; j < fDataHist->GetNbinsX(); j++){
      //(*fFullCovar)(i,j) = tempcov->GetBinContent(i+1, j+1);
      (*fFullCovar)(i,j) = tempcov->GetBinContent(i+1,j+1)*fDataHist->GetBinContent(i+1)*fDataHist->GetBinContent(j+1);
      if(i==j) fDataHist->SetBinError(i+1,sqrt((*fFullCovar)(i,j)));
      if(i==j) std::cout << "For bin " << i+1 << ", relative covariance was " << tempcov->GetBinContent(i+1,j+1);
      if(i==j) std::cout << ". Absolute covariance is now " << (*fFullCovar)(i,j) << ", linear xsec is: " << fDataHist->GetBinContent(i+1) << std::endl;    }
  }
  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);

  TH1D* linearResult = new TH1D(*fDataHist);
  linearResult->SetNameTitle("T2K_CC0pi1p_XSec_3DPcoscos_nu_nonuniform_data" ,"T2K_CC0pi1p_XSec_3DPcoscos_nu_nonuniform_data");
  SetAutoProcessTH1(linearResult, kCMD_Write);
  
  // Read in 3D Data
//  for (int c = 0; c < 4; c++){
//    fDataPoly[c] = (TH2Poly*) fInputFile->Get(Form("dataslice_%i",c));
//    fDataPoly[c]->SetNameTitle(Form("T2K_CC0pi1p_XSec_3DPcoscos_nu_nonuniform_datapoly_%i",c),Form("T2K_CC0pi1p_XSec_3DPcoscos_nu_nonuniform_datapoly_%i",c));
//    SetAutoProcessTH1(fDataPoly[c], kCMD_Write);
//    fDataHist->Reset();
//  }
  
  // Read in 2D Data Slices and Make MC Slices
  fDataHist->Reset();
  int bincount = 0;
  for (int i = 0; i < 4; i++){ //both y and z slices
  
    // Get Data Histogram
    fInputFile->ls();
    fDataHist_Slices.push_back((TH2Poly*)fInputFile->Get(Form("dataslice_%i",i))->Clone());
    fDataHist_Slices[i]->SetNameTitle(Form("T2K_CC0pi1p_XSec_3DPcoscos_nu_nonuniform_data_Slice%i",i), (Form("T2K_CC0pi1p_XSec_3DPcoscos_nu_nonuniform_data_Slice%i",i)));

    for (int j = 0; j < fDataHist_Slices[i]->GetNumberOfBins(); j++){
      fDataHist_Slices[i]->SetBinError(j, sqrt(tempcov->GetBinContent(bincount,bincount)));
      fDataHist->SetBinContent(bincount+1, fDataHist_Slices[i]->GetBinContent(j+1));
      fDataHist->SetBinError(bincount+1, fDataHist_Slices[i]->GetBinError(j+1));
      bincount++;
    }

    // Loop over nbins and set errors from covar
//    for (int j = 0; j < fDataHist_Slices[i]->GetNbinsX(); j++){
//      for (int k = 0; k < fDataHist_Slices[i]->GetNbinsY(); k++){
//        fDataHist_Slices[i]->SetBinError(j+1, k+1, sqrt((*fFullCovar)(bincount,bincount)) * 1E-38);
//
//        std::cout << "Setting data hist " <<  fDataHist_Slices[i]->GetBinContent(j+1,k+1) << " " << fDataHist_Slices[i]->GetBinError(j+1,k+1) << std::endl;
//        fDataHist->SetBinContent(bincount+1, fDataHist_Slices[i]->GetBinContent(j+1,k+1) );
//        fDataHist->SetBinError(bincount+1, fDataHist_Slices[i]->GetBinError(j+1,k+1) );
//
//        bincount++;
//      }
//    }

    // Make MC Clones
    fMCHist_Slices.push_back((TH2Poly*) fDataHist_Slices[i]->Clone());
    fMCHist_Slices[i]->SetNameTitle(Form("T2K_CC0pi1p_XSec_3DPcoscos_nu_nonuniform_MC_Slice%i",i), (Form("T2K_CC0pi1p_XSec_3DPcoscos_nu_nonuniform_MC_Slice%i",i)));

    SetAutoProcessTH1(fDataHist_Slices[i],kCMD_Write);
    SetAutoProcessTH1(fMCHist_Slices[i]);
    fMCHist_Slices[i]->ClearBinContents();
  }
  return;
};
