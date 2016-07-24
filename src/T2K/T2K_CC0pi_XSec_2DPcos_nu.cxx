// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NuFiX.
*
*    NuFiX is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NuFiX is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include "T2K_CC0pi_XSec_2DPcos_nu.h"

// The constructor
T2K_CC0pi_XSec_2DPcos_nu::T2K_CC0pi_XSec_2DPcos_nu(std::string name,
						   std::string inputfile,
						   FitWeight *rw,
						   std::string type,
						   std::string fakeDataFile){


  measurementName = name;
  analysis = measurementName.find("_II") != std::string::npos ? 2 : 1;
  forwardgoing = (type.find("REST") != std::string::npos);
  EnuMin = 0;
  EnuMax = 10.0;
  exp_distance = 0.280;
  default_types = "DIAG/FIX";
  allowed_types = "DIAG/FIX";
  data_points_x = 12;
  data_points_y = 10;
  Measurement2D::SetupMeasurement(inputfile, type, rw, fakeDataFile);
  
  Double_t tempx[12] = {0.2, 0.35, 0.5, 0.65, 0.8, 0.95, 1.1, 1.25, 1.5, 2.0, 3.0, 5.0};
  Double_t tempy[10] = {-1.0, 0.0, 0.6, 0.7, 0.8, 0.85, 0.9, 0.94, 0.98, 1.0};
  xBins = tempx;
  yBins = tempy;
  plotTitles = "; P_{#mu} (GeV); cos#theta_{#mu}; d^{2}#sigma/dP_{#mu}dcos#theta_{#mu} (cm^{2}/GeV)";
  
  dataHist = new TH2D((measurementName+"_data").c_str(),
		      (measurementName+"_data"+plotTitles).c_str(),
		      data_points_x-1, xBins,
		      data_points_y-1, yBins);
  
  SetHistograms(FitPar::GetDataBase()+"/T2K/CC0pi/data_release.root");
  SetupDefaultHist();
  
  // Diagonal covar setup
  if (!isShape) addNormPenalty = true;
  normError = 0.089; // Set from covar mat instead...

  cout << " Inputs = "<<this->GetInput()<<std::endl;
  
  // Get Scaling
  scaleFactor = (eventHist->Integral("width")/(nevents+0.))*13.0/6.0/TotalIntegratedFlux(); // NEUT

};


bool T2K_CC0pi_XSec_2DPcos_nu::isSignal(FitEvent *event){
  return SignalDef::isT2K_CC0pi(event, EnuMin, EnuMax, forwardgoing);
};

void T2K_CC0pi_XSec_2DPcos_nu::FillEventVariables(FitEvent* event){

  double pmu = -999.9;
  double CosThetaMu = -999.9;
  
  // Loop over all particles
  for (UInt_t j = 2; j < event->Npart(); ++j){

    // Muon section
    if ((event->PartInfo(j))->fPID == 13){

      // Now find the kinematic values and fill the histogram
      pmu     = (event->PartInfo(j))->fP.Vect().Mag()/1000;
      CosThetaMu = cos(((event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect())));

      continue;
    }
  }
  
  this->X_VAR = pmu;
  this->Y_VAR = CosThetaMu;

  return;
};

// Modification is needed after the full reconfigure to move bins around
// Otherwise this would need to be replaced by a TH2Poly which is too awkward.



void T2K_CC0pi_XSec_2DPcos_nu::ConvertEventRates(){

  // Do standard conversion.
  Measurement2D::ConvertEventRates();

  if (analysis == 1){
  
    // Following code handles weird ND280 Binning
    int nbins = this->mcHist->GetNbinsX() + 1;
    double total = 0.0;

    // Y = 1
    total = 0.0;
    for (int i = 3; i < nbins; i++){

      double width = this->mcHist->GetXaxis()->GetBinWidth(i) * this->mcHist->GetYaxis()->GetBinWidth(1);
      total += this->mcHist->GetBinContent(i, 1) * width;
      this->mcHist->SetBinContent(i,1,0);
    }
    this->mcHist->SetBinContent(3, 1, total / (1.0 * 29.6));

    // Y = 2
    total = 0.0;
    for (int i = 5; i < nbins; i++){
      double width = this->mcHist->GetXaxis()->GetBinWidth(i) * this->mcHist->GetYaxis()->GetBinWidth(2);
      total += this->mcHist->GetBinContent(i, 2)* width;
      this->mcHist->SetBinContent(i,2,0);
    }
    this->mcHist->SetBinContent(5, 2, total / (0.6 *29.4));

    // Y = 3
    total = 0.0;
    for (int i = 7; i < nbins; i++){
      double width = this->mcHist->GetXaxis()->GetBinWidth(i) * this->mcHist->GetYaxis()->GetBinWidth(3);
      total += this->mcHist->GetBinContent(i, 3)* width;
      this->mcHist->SetBinContent(i, 3,0);
    }
    this->mcHist->SetBinContent(7, 3, total/ (0.1 * 29.2));

    // Y = 4
    total = 0.0;
    for (int i = 7; i < nbins; i++){
      double width = this->mcHist->GetXaxis()->GetBinWidth(i) * this->mcHist->GetYaxis()->GetBinWidth(4);
      total += this->mcHist->GetBinContent(i, 4)* width;
      this->mcHist->SetBinContent(i, 4,0);
    }
    this->mcHist->SetBinContent(7, 4, total / (0.1 * 29.2));

    // Y = 5
    total = 0.0;
    for (int i = 8; i < nbins; i++){
      double width = this->mcHist->GetXaxis()->GetBinWidth(i) * this->mcHist->GetYaxis()->GetBinWidth(5);
      total += this->mcHist->GetBinContent(i, 5)* width;
      this->mcHist->SetBinContent(i,5,0);
    }
    this->mcHist->SetBinContent(8, 5, total / (0.05 * 29.0));

    // Y = 6
    total = 0.0;
    for (int i = 9; i < nbins; i++){
      double width = this->mcHist->GetXaxis()->GetBinWidth(i) * this->mcHist->GetYaxis()->GetBinWidth(6);
      total += this->mcHist->GetBinContent(i, 6)* width;
      this->mcHist->SetBinContent(i, 6,0);
    }
    this->mcHist->SetBinContent(9, 6, total / (0.05 * 28.5));

    // Y = 7
    total = 0.0;
    for (int i = 8; i < nbins; i++){
      double width = this->mcHist->GetXaxis()->GetBinWidth(i) * this->mcHist->GetYaxis()->GetBinWidth(7);
      total += this->mcHist->GetBinContent(i, 7)* width;
      this->mcHist->SetBinContent(i, 7,0);
    }
    this->mcHist->SetBinContent(8, 7, total/ (0.04 * 28.0));

    // Y = 8
    total = 0.0;
    for (int i = 11; i < nbins; i++){
      double width = this->mcHist->GetXaxis()->GetBinWidth(i) * this->mcHist->GetYaxis()->GetBinWidth(8);
      total += this->mcHist->GetBinContent(i, 8)* width;
      this->mcHist->SetBinContent(i, 8,0);
    }
    this->mcHist->SetBinContent(11, 8, total / (0.4 * 27.0));

    // Y = 9
    total = 0.0;
    for (int i = 9; i < nbins; i++){
      double width = this->mcHist->GetXaxis()->GetBinWidth(i) * this->mcHist->GetYaxis()->GetBinWidth(9);
      total += this->mcHist->GetBinContent(i, 9)* width;
      this->mcHist->SetBinContent(i,9,0);
    }
    this->mcHist->SetBinContent(9, 9, total / (0.02 * 25.0));
  }
  
  return;
}


void T2K_CC0pi_XSec_2DPcos_nu::SetHistograms(std::string infile){

  LOG(SAM)<<"Reading data from "<<infile<<std::endl;
  TFile* rootfile = new TFile(infile.c_str(),"READ");

  if (analysis == 1){

  } else {

    rootfile->ls();
    
    this->dataHist = (TH2D*) rootfile->Get("data_analysis2")->Clone((this->measurementName+"_data").c_str());
    this->mapHist = (TH2I*) rootfile->Get("map_analysis2")->Clone((this->measurementName+"_MAP").c_str());

    TMatrixDSym* covmat_stat = (TMatrixDSym*) rootfile->Get("analysis2_statcov");
    TMatrixDSym* covmat_flux = (TMatrixDSym*) rootfile->Get("analysis2_fluxcov");
    TMatrixDSym* covmat_syst = (TMatrixDSym*) rootfile->Get("analysis2_systcov");

    // Get flags
    this->fullcovar = new TMatrixDSym(67);

    for (int i = 0; i < 67; i++){
      for (int j = 0; j < 67; j++){
	(*fullcovar)(i,j) = (*fullcovar)(i,j) + (*covmat_stat)(i,j);
	(*fullcovar)(i,j) = (*fullcovar)(i,j) + (*covmat_flux)(i,j);
	(*fullcovar)(i,j) = (*fullcovar)(i,j) + (*covmat_syst)(i,j);
      }
    }
    
    this->covar = StatUtils::GetInvert(fullcovar);
    this->decomp = StatUtils::GetDecomp(covar);
  }
  
  return;
};
