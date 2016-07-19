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

#include "MiniBooNE_NCEL_XSec_Treco_nu.h"
#include "TLorentzVector.h"

/// ENTIRE CLASS NEEDS FIXING


// The constructor
MiniBooNE_NCEL_XSec_Treco_nu::MiniBooNE_NCEL_XSec_Treco_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){
  
  

  // // Check if this is a shape only fit - for now, there is no shape option.
  // if (!type.compare("SHAPE")){
  //   std::cout << "MiniBooNE NCEL is not available as a shape only fit... ignoring..." << std::endl;
  // }

  // // Set pointer to the reweighting engine
  // rw_engine = rw;
  
  // // Define the energy of the signal
  // this->EnuMin = 0.;
  // this->EnuMax = 10.;

  // // In future read most of these from a card file
  // this->inFile = inputfile;
  // this->measurementName = "MB_NCEL_XSec_Treco_nu";
  // this->plotTitles = "; T_{reco} (MeV); Events/(12 MeV)";

  // // Because the binning is in Treco is fairly esoteric, hardcode here
  // this->arr_treco = {40.0, 52.0, 63.9, 75.9, 87.8, 99.8, 111.8, 123.7, 135.7, 147.6, 159.6, 171.6, 183.5, 195.5,   
  // 		     207.5, 219.4, 231.4, 243.3, 255.3, 267.3, 279.2, 291.2, 303.1, 315.1, 327.1, 339.0, 351.0, 362.9, 374.9, 386.9,   
  // 		     398.8, 410.8, 422.7, 434.7, 446.7, 458.6, 470.6, 482.5, 494.5, 506.5, 518.4, 530.4, 542.4, 554.3, 566.3, 578.2,   
  // 		     590.2, 602.2, 614.1, 626.1, 638.0, 650.0};

  // this->SetDataValues(FitPar::GetDataBase()+"/MiniBooNE/ncqe/input_data.txt");
  // this->SetCovarMatrix(FitPar::GetDataBase()+"/MiniBooNE/ncqe/ErrorMatrix.tab", 51);
  // this->SetResponseMatrix(FitPar::GetDataBase()+"/MiniBooNE/ncqe/response_mat.txt", 51);

  // // Check if we're using fake data
  // if (!fakeDataFile.empty()) this->SetFakeDataValues(fakeDataFile);

  // // This will be the final data histogram
  // this->mcHist = new TH1D((this->measurementName+"_MC").c_str(), (this->measurementName+this->plotTitles).c_str(), 51, this->arr_treco);
 
  // // Usually, the mcFine histogram is a finer binned version of mcHist. But as NCEL requires a Ttrue histogram, co-opt mcFine for this purpose. 
  // // Should probably change the naming to reflect the possible other use of this histogram.
  // this->mcFine = new TH1D((this->measurementName+"_Ttrue").c_str(), (this->measurementName+this->plotTitles).c_str(), 50, 0, 900);

  // // Read in the histograms from the NEUT file that are required for normalisation
  // TFile *in = new TFile(this->inFile.c_str());
  // this->fluxHist  = (TH1D*)in->Get((PlotUtils::GetObjectWithName(in, "flux")).c_str());
  // this->fluxHist->SetNameTitle((this->measurementName+"_FLUX").c_str(), (this->measurementName+";E_{#nu} (GeV)").c_str());

  // this->eventHist = (TH1D*)in->Get((PlotUtils::GetObjectWithName(in, "evtrt")).c_str());
  // this->eventHist->SetNameTitle((this->measurementName+"_EVT").c_str(), (this->measurementName+";E_{#nu} (GeV); Event Rate").c_str());

  // // Read in the file once only
  // tn = new TChain("neuttree", "");
  // tn->Add(Form("%s/neuttree", this->inFile.c_str()));
  // nevents = tn->GetEntries();
  // nvect = NULL;
  // tn->SetBranchAddress("vectorbranch", &nvect);

  // // The scale factor is quite complicated because MB didn't divide by number of targets.
  // // nMolMB is the number of CH_2 molecules in the MB FV (610.6 cm radius sphere) and 0.845 is the published density of the mineral oil.
  // double nMolMB = 6.023E+23*0.845*4.0*M_PI*610.6*610.6*610.6/3.0;
  // this->scaleFactor = (this->eventHist->Integral()*1E-38*14.08/(nevents+0.))*nMolMB*0.646165;
};


void MiniBooNE_NCEL_XSec_Treco_nu::Reconfigure(double norm, bool fullconfig){
  
  // // Clear the current histogram before repopulating
  // this->mcHist->Reset();
  // this->mcFine->Reset();
  // this->currentNorm = norm;

  // // Loop over all events at each iteration of the fit
  // for (int i = 0; i < nevents; ++i){
  //   tn->GetEntry(i);

  //   if (!isSignal(nvect)) continue;

  //   // Find the weight
  //   double rw_weight = 1;
  //   rw_weight = rw_engine->CalcWeight(customEvent);
    
  //   // Skip any events with suspiciously large weights...
  //   if (rw_weight > 200) {std::cout << "LARGE WEIGHT: " << rw_weight << std::endl; break;}

  //   // Sum of the true kinetic energies of particles
  //   double t_true = 0.;

  //   // Loop over the particle stack
  //   for (int j = 2; j < nvect->Npart(); ++j){
      
  //     // Add the kinetic energies of any nucleons
  //     if (abs((nvect->PartInfo(j))->fPID) == 2212)
  // 	t_true += (nvect->PartInfo(j))->fP.E()/1000 - 0.93956536;
  //     else if (abs((nvect->PartInfo(j))->fPID) == 2112)
  // 	t_true += (nvect->PartInfo(j))->fP.E()/1000 - 0.93827203;

  //   }
    
  //   // Now fill the Ttrue histogram
  //   this->mcFine->Fill(t_true*1000., rw_weight);
  // }

  // // Now convert Ttrue to Treco...
  // for (int treco = 0; treco < 51; ++treco){
  //   double total = 0.;
  //   for (int ttrue = 0; ttrue < 50; ++ttrue) total += mcFine->GetBinContent(ttrue+1)*this->response_mat->GetBinContent(ttrue+1, treco+1);
  //   this->mcHist->SetBinContent(treco+1, total);
  // }

  // // Scale
  // this->mcHist->Scale(this->scaleFactor, "width");
  // this->mcFine->Scale(this->scaleFactor, "width");

  // // Add in the backgrounds...
  // for (int treco = 0; treco < 51; ++treco){
  //   double total = this->mcHist->GetBinContent(treco+1) + this->BKGD_other->GetBinContent(treco+1) + this->BKGD_irrid->GetBinContent(treco+1);
  //   this->mcHist->SetBinContent(treco+1, total);
  // }

  // // Normalisation factor if one has been provided.
  // if (norm){
  //   this->mcHist->Scale(1.0/norm);
  // } else {
  //   this->mcHist->Scale(0);
  // }
  
  // return;
};


bool MiniBooNE_NCEL_XSec_Treco_nu::isSignal(NeutVect *nvect){
  
  // Only interested in true NCEL events
  // if (nvect->Mode != 51 && nvect->Mode != 52) return false;

  // // Only look at numu events
  // if ((nvect->PartInfo(0))->fPID != 14 && (nvect->PartInfo(0))->fPID != 12) return false;

  // // Restrict energy range
  // if ((nvect->PartInfo(0))->fP.E() < this->EnuMin*1000 || (nvect->PartInfo(0))->fP.E() > this->EnuMax*1000) return false;

  return true;
};


// Read in the covariance matrix from the file specified in the constructor
void MiniBooNE_NCEL_XSec_Treco_nu::SetCovarMatrix(std::string covarFile, int dim){
  
  // // Make a counter to track the line number
  // int row = 0;

  // std::string line;
  // std::ifstream covar(covarFile.c_str(),ifstream::in);
  
  // this->covar = new TMatrixDSym(dim);

  // if(covar.is_open()) std::cout << "Reading covariance matrix from file: " << covarFile << std::endl;

  // while(std::getline(covar, line, '\n')){
  //   std::istringstream stream(line);
  //   double entry;    
  //   int column = 0;

  //   // Loop over entries and insert them into matrix
  //   // Multiply by the errors to get the covariance, rather than the correlation matrix
  //   while(stream >> entry){
  //     (*this->covar)(row, column) = entry;
  //     if (row == column) this->dataHist->SetBinError(row+1, sqrt(entry));
  //     column++;
  //   }    
  //   row++;
  // }
  
  // // Robust matrix inversion method
  // TDecompSVD LU = TDecompSVD(*this->covar);
  // this->covar = new TMatrixDSym(dim, LU .Invert().GetMatrixArray(), "");

  return;
};



// The covariance matrix contains all of the information for the chi2 calculation
double MiniBooNE_NCEL_XSec_Treco_nu::GetChi2(){
  // double chi2 = 0;
  
  // int nBins = this->dataHist->GetNbinsX();

  // for (int i = 0; i < nBins; ++i){
  //   for (int j = 0; j < nBins; ++j){

  //     double iDiff = this->dataHist->GetBinContent(i+1) - this->mcHist->GetBinContent(i+1);
  //     double jDiff = this->dataHist->GetBinContent(j+1) - this->mcHist->GetBinContent(j+1);
  //     chi2 += iDiff*(*this->covar)(i, j)*jDiff;
  //   }
  // }
  // return chi2;

};


// Function to make an Asimov dataset (so don't throw any errors)     
void MiniBooNE_NCEL_XSec_Treco_nu::SetFakeDataValues(std::string fakeDataFile) {

  // // This is the published data
  // TH1D *tempData = (TH1D*)this->dataHist->Clone();
  // TFile *fake    = new TFile(fakeDataFile.c_str());

  // // This is the fake data
  // this->dataHist = (TH1D*)fake->Get((this->measurementName+"_MC").c_str());
  // this->dataHist ->SetNameTitle((this->measurementName+"_FAKE").c_str(), (this->measurementName+this->plotTitles).c_str());

  // for (int xBin = 0; xBin < this->dataHist->GetNbinsX(); ++xBin){

  //   // If the fake data or real didn't didn't fill the bin, can't assign an error
  //   if (!this->dataHist->GetBinContent(xBin+1) || !tempData->GetBinContent(xBin+1)){
  //     this->dataHist->SetBinError(xBin+1, 0);
  //     continue;
  //   }

  //   double err = tempData->GetBinError(xBin+1)*
  //     this->dataHist->GetBinContent(xBin+1)/(tempData->GetBinContent(xBin+1)+0.);
  //   this->dataHist->SetBinError(xBin+1, err);
  // }

  // delete tempData;
  // return;
};



// Override the usual function in the base class because this is more complicated for the NCEL sample...
void MiniBooNE_NCEL_XSec_Treco_nu::SetDataValues(std::string inputFile){

  // std::string line;
  // std::ifstream input(inputFile.c_str(),ifstream::in);

  // if(input.is_open()) std::cout << "Reading data from file: " << inputFile << std::endl;
  
  // this->dataHist   = new TH1D((this->measurementName+"_data").c_str(), (this->measurementName+this->plotTitles).c_str(), 
  // 			      51, this->arr_treco);
  // this->BKGD_other = new TH1D((this->measurementName+"_BKGD_other").c_str(), (this->measurementName+this->plotTitles).c_str(), 
  // 			      51, arr_treco);
  // this->BKGD_irrid = new TH1D((this->measurementName+"_BKGD_irrid").c_str(), (this->measurementName+this->plotTitles).c_str(), 
  // 			      51, arr_treco);
  // // To get the nDOF correct...
  // this->data_points= 52;

  // double entry = 0;
  // int xBin     = 0;

  // // First line is the MB data
  // std::getline(input, line, '\n');
  // std::istringstream stream1(line);
  
  // while(stream1 >> entry){
  //   this->dataHist->SetBinContent(xBin+1, entry);
  //   xBin++;
  // }
 
  // // Second line is "other" backgrounds
  // std::getline(input, line, '\n');
  // std::istringstream stream2(line);
  // entry = 0;
  // xBin  = 0;  
  // while(stream2 >> entry){
  //   this->BKGD_other->SetBinContent(xBin+1, entry);
  //   xBin++;
  // } 

  // // Third line is the irreducible background
  // std::getline(input, line, '\n');
  // std::istringstream stream3(line);
  // entry = 0;
  // xBin  = 0;  
  // while(stream3 >> entry){
  //   this->BKGD_irrid->SetBinContent(xBin+1, entry);
  //   xBin++;
  // } 
};

// Read in the response matrix -- thus far, a response matrix is unique to the NCEL sample
void MiniBooNE_NCEL_XSec_Treco_nu::SetResponseMatrix(std::string responseFile, int dim){
  // Make a counter to track the line number
  // int xBin = 0;

  // std::string line;
  // std::ifstream response(responseFile.c_str(),ifstream::in);
  
  // // Response matrix: x axis is Ttrue, y axis is Treco
  // this->response_mat = new TH2D((this->measurementName+"_RESPONSE_MATRIX").c_str(), (this->measurementName+this->plotTitles).c_str(),
  // 				50, 0, 900, 51, this->arr_treco);

  // if(response.is_open()) std::cout << "Reading in the response matrix from file: " << responseFile << std::endl;

  // while(std::getline(response, line, '\n')){
  //   std::istringstream stream(line);
  //   double entry;    
  //   int yBin = 0;

  //   // Loop over entries and insert them into matrix
  //   // Multiply by the errors to get the covariance, rather than the correlation matrix
  //   while(stream >> entry){
  //     this->response_mat->SetBinContent(xBin+1, yBin+1, entry);
  //     yBin++;
  //   }    
  //   xBin++;
  // }
};
