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

#include "parameter_pulls.h"

// The constructor
parameter_pulls::parameter_pulls(std::string name, std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  this->inFile = inputfile;
  this->measurementName = name;
  this->plotTitles = "; Dials; Reweight Values";
  this->rw_engine = rw;

  if (type.find("FRAC") != std::string::npos) dialoption = "FRAC";
  else if (type.find("ABS") != std::string::npos) dialoption = "ABS";

  this->SetDataValues(this->inFile); // Set the matrix and parameters from a text file.;
  this->mcHist = (TH1D*) this->dataHist->Clone();
  this->mcHist->Reset();
  this->mcHist->SetNameTitle((this->measurementName+"_FitVals").c_str(), (this->measurementName+this->plotTitles).c_str());
    
};


void parameter_pulls::Reconfigure(double norm, bool fullconfig){

  // Get Dial Names that are valid
  std::vector<std::string> svec = this->rw_engine->GetDialNames();

  // Set Bin Values
  for (int i = 0; i < svec.size(); i++){
    std::string systString = svec.at(i);

    for (int j = 0; j < mcHist->GetNbinsX(); j++){

      if (systString.compare( mcHist->GetXaxis()->GetBinLabel(j+1) ) == 0){
	double curVal =  this->rw_engine->GetDialValue(systString, dialoption);
	this->mcHist->SetBinContent(j+1,curVal);
      }
      
    }
  }

  return;

};

void parameter_pulls::Renormalise(double norm){
  return;
};


double parameter_pulls::GetChi2(){

  int nbins = this->dataHist->GetNbinsX();
  double chi2 = 0.0;

  for (int i = 0; i < nbins; i++){
    for (int j = 0; j < nbins; j++){
      
      double idif = this->dataHist->GetBinContent(i+1) - this->mcHist->GetBinContent(i+1);
      double jdif = this->dataHist->GetBinContent(j+1) - this->mcHist->GetBinContent(j+1);

      chi2 += idif*jdif *(*this->covar)(i, j);
    }
  }
 
  return chi2;

};

void parameter_pulls::ThrowCovariance(){

  std::vector<double> randvals;
  for (int i = 0; i < this->dataHist->GetNbinsX(); i++){
    randvals.push_back(gRandom->Gaus(0.0,1.0));
  }

  for (int i = 0; i < this->dataHist->GetNbinsX(); i++){
    
    double binmod  = 0.0;

    for (int j = 0; j < this->dataHist->GetNbinsX(); j++){
      
      binmod += (*this->decomp)(j,i) * randvals.at(j);

    }

    this->dataHist->SetBinContent(i+1,this->dataTrue->GetBinContent(i+1) + binmod);

    LOG(REC)<<"New value for "<<this->dataHist->GetXaxis()->GetBinLabel(i+1)<< " = "<<this->dataHist->GetBinContent(i+1)<<std::endl;
  }

  return;

};

//**********************************************
void parameter_pulls::SetDataValues(std::string dataFile){
//**********************************************

  // --- Try and read ROOT file ------------------------  
  cout<<"Finding parameter pull from root file: "<<dataFile<<std::endl;
  
  // Pares input file incase names provided
  std::vector<std::string> inputs = PlotUtils::FillVectorSFromString(dataFile, ";");
  
  // If no extra inputs provided use standard
  if (inputs.size() == 1){
    inputs.push_back("fit_dials");
    inputs.push_back("covariance_free");
  }
  
  // Open Input
  TFile* input = new TFile(inputs.at(0).c_str(),"READ");
  if (!input){
    std:: cerr<<" No input file found for parameter pulls! "<<std::endl;
    exit(-1);
  }
  
    // Get covariance plot
    TH2D* covarplot = (TH2D*) input->Get( inputs.at(2).c_str() );
    TH1D* fitplot   = (TH1D*) input->Get( inputs.at(1).c_str() );

    if (!covarplot or !fitplot){
      std::cerr<<"No covariance or dials plot!"<<std::endl;
      exit(-1);
    }


    int dim = covarplot->GetNbinsX();
    this->data_points = dim+1;
    
    this->dataHist = new TH1D((this->measurementName + "_CentralVals").c_str(),(this->measurementName + "_centralvals").c_str(),dim,0,dim);

    for (int i = 0; i < dim; i++){
      this->dataHist->GetXaxis()->SetBinLabel(i+1,covarplot->GetXaxis()->GetBinLabel(i+1));
      this->dataHist->SetBinError(i+1,sqrt(covarplot->GetBinContent(i+1,i+1)));
    }

    // Fill Central Values
    for (int i = 0; i < dim; i++){
      for (int j = 0; j < fitplot->GetNbinsX(); j++){

	std::string parname = std::string(fitplot->GetXaxis()->GetBinLabel(j+1));

	if (!parname.compare(dataHist->GetXaxis()->GetBinLabel(i+1)))
	  dataHist->SetBinContent(i+1, fitplot->GetBinContent(j+1));
      }
    }


    this->fullcovar = new TMatrixDSym(dim);
    for (int i = 0; i < dim; i++){
      for (int j = 0; j < dim; j++){
	(*(this->fullcovar))(i,j) = covarplot->GetBinContent(i+1,j+1);
	//	std::cout<<i<<" "<<j<<" = "<<covarplot->GetBinContent(i+1,j+1)<<std::endl;
      }
    }

    this->covar = StatUtils::GetInvert(fullcovar);
    this->decomp = StatUtils::GetDecomp(fullcovar);
    
  
  return;
};


void parameter_pulls::Write(std::string drawOpt){

 
  TH2D* temp = new TH2D(*this->covar);
  
  temp->SetName((this->measurementName + "_INVCOVAR").c_str());
  temp->SetTitle((this->measurementName + "_INVCOVAR; Dials; Dials; Covariance").c_str());

  for(std::map<std::string,int>::iterator binIter = this->binMap.begin(); binIter != this->binMap.end(); binIter++){
    
    this->mcHist->GetXaxis()->SetBinLabel(binIter->second, binIter->first.c_str());
    this->dataHist->GetXaxis()->SetBinLabel(binIter->second, binIter->first.c_str());

    temp->GetXaxis()->SetBinLabel(binIter->second, binIter->first.c_str());
    temp->GetYaxis()->SetBinLabel(binIter->second, binIter->first.c_str());

  }
  
  this->mcHist->Write();
  this->dataHist->Write();  
  temp->Write();


  return;

};


TH2D parameter_pulls::GetFullCovarMatrix(){

  TH2D tempCov = TH2D(*fullcovar);
  for (int i = 0; i < tempCov.GetNbinsX(); i++){
    tempCov.GetXaxis()->SetBinLabel(i+1,dataHist->GetXaxis()->GetBinLabel(i+1));
    tempCov.GetYaxis()->SetBinLabel(i+1,dataHist->GetXaxis()->GetBinLabel(i+1));
  }

  return tempCov;
};
