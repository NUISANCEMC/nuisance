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
T2K_CC0pi_XSec_2DPcos_nu::T2K_CC0pi_XSec_2DPcos_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){

  // NEEDS UPDATING

  // // Get Fit Options
  // this->SetFitOptions(type);
  // this->forwardgoing = (type.find("REST") != std::string::npos);

  // // Set pointer to the reweighting engine
  // rw_engine = rw;

  // // Define the energy region
  // this->EnuMin = 0;
  // this->EnuMax = 10.0;

  // this->exp_distance = 0.280;

  // this->data_points_x = 12;
  // this->data_points_y = 10;
  // Double_t tempx[12] = {0.2, 0.35, 0.5, 0.65, 0.8, 0.95, 1.1, 1.25, 1.5, 2.0, 3.0, 5.0};
  // Double_t tempy[10] = {-1.0, 0.0, 0.6, 0.7, 0.8, 0.85, 0.9, 0.94, 0.98, 1.0};
  // this->xBins = tempx;
  // this->yBins = tempy;

  // this->inFile = inputfile;
  // this->measurementName = "T2K_CC0pi_XSec_2DPcos_nu";
  // this->plotTitles = "; P_{#mu} (GeV); cos#theta_{#mu}; d^{2}#sigma/dP_{#mu}dcos#theta_{#mu} (cm^{2}/GeV)";


  // // Fill just the map histogram first
  // this->dataHist = new TH2D((this->measurementName+"_data").c_str(),(this->measurementName+"_data"+this->plotTitles).c_str(),
  // 			    this->data_points_x-1, this->xBins, this->data_points_y-1, this->yBins);

  // this->SetHistograms(std::string(std::getenv("NIWG_DATA"))+"/T2K/T2K_CC0PI_TN167_data.root");

  // // Diagonal covar setup
  // if (!this->isShape) this->addNormPenalty = true;
  // this->normError = 0.08559;

  // this->mcHist = (TH2D*) (this->dataHist)->Clone((this->measurementName+"_MC").c_str());
  // this->mcFine = new TH2D((this->measurementName+"_data").c_str(),(this->measurementName+"_data"+this->plotTitles).c_str(),
  // 			  25, 0.2, 5.0, 25, -1.0, 1.0);
  // PlotUtils::CreateNeutModeArray(this->mcHist, (TH1**)this->mcHist_PDG);


  // // Setup Neut Events from inFile
  // this->ReadEventFile();

  // // Different generators require slightly different rescaling factors.
  // if    (this->eventType == 0) this->scaleFactor = (this->eventHist->Integral("width")/(nevents+0.))*13.0/6.0/this->TotalIntegratedFlux(); // NEUT

};


bool T2K_CC0pi_XSec_2DPcos_nu::isSignal(FitEvent *event){

  if (!only_allowed_particles) return false;
  if (!numu_event) return false;
  if (numu_energy < this->EnuMin || numu_energy > this->EnuMax) return false;

  // restricted phase space
  if (this->forwardgoing and CosThetaMu < 0.0) return false;

  return true;
};

void T2K_CC0pi_XSec_2DPcos_nu::FillEventVariables(FitEvent* event){

  numu_event =  ((event->PartInfo(0))->fPID == 14);
  only_allowed_particles = true;

  numu_energy = ((event->PartInfo(0))->fP.E()/1000.0);

  // Loop over all particles
  for (int j = 2; j < event->Npart(); ++j){

    // Muon section
    if ((event->PartInfo(j))->fPID == 13){

      // Now find the kinematic values and fill the histogram
      pmu     = (event->PartInfo(j))->fP.Vect().Mag()/1000;
      CosThetaMu = cos(((event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect())));

      continue;
    }

     // Only interested in final state particles...
    if (!(event->PartInfo(j))->fIsAlive or (event->PartInfo(j))->fStatus != 0) continue;

    // Restrict allowed particles
    particle_pdg = (event->PartInfo(j))->fPID;

    if (particle_pdg != 22 && //should be photon
        particle_pdg != 2212 &&
        particle_pdg != 2112 &&
        particle_pdg != 13) only_allowed_particles = false;
  }

  this->X_VAR = pmu;
  this->Y_VAR = CosThetaMu;

  return;
};

// Modification is needed after the full reconfigure to move bins around
// Otherwise this would need to be replaced by a TH2Poly which is too awkward.
void T2K_CC0pi_XSec_2DPcos_nu::Reconfigure(double norm, bool fullconfig){

  MeasurementBase::Reconfigure();

  //  InputHandler::Reconfigure(norm, fullconfig);

  // Y = 0
  //  TH2D* tempMC = (TH2D*)this->mcHist->Clone();
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

  //  this->mcHist = tempMC->Clone();
  //  delete tempMC;

  return;
}


void T2K_CC0pi_XSec_2DPcos_nu::SetHistograms(std::string infile){

  std::cout<<"Reading data from "<<infile<<std::endl;
  TFile* rootfile = new TFile(infile.c_str(),"READ");

  this->dataHist = (TH2D*) rootfile->Get("dataPlot")->Clone((this->measurementName+"_data").c_str());
  this->mapHist = (TH2I*) rootfile->Get("mapPlot")->Clone((this->measurementName+"_MAP").c_str());

  TH2D* covplot = (TH2D*) rootfile->Get("covPlot");

  int nbins = covplot->GetNbinsX();
  this->covar = new TMatrixDSym(nbins);

  for (int i = 0; i < nbins; i++){
    for (int j = 0; j < nbins; j++){
      (*this->covar)(i,j) = covplot->GetBinContent(i+1,j+1) * 1E92 ;
    }
  }

  // Robust matrix inversion method
  TDecompSVD LU = TDecompSVD(*this->covar);
  this->covar = new TMatrixDSym(nbins, LU .Invert().GetMatrixArray(), "");

  std::cout<<"Got dataHist "<<this->dataHist->GetTitle()<<std::endl;
  int nBins = 67;


  return;
};
