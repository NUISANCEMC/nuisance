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



//!!!!!!!!!!!!!!!!!
// This script is obsolete, use ExtFit_GLSminimize.cxx from now on.



// Author: Callum Wilkinson    01/2014  
/**
   Usage: ./minimizeCCQE.exe -c card file, where samples and parameters are defined
                             -o output file, where the results of the fit are stored
   where:   
*/

#include "TH1.h"
#include "TF1.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "Minuit2/FCNBase.h"
#include "TFitterMinuit.h"
#include "TSystem.h"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cstring>

#include "T2KSyst.h"
#include "T2KNeutReWeight.h"
#include "T2KNIWGReWeight.h"
#include "FitEvent.h"
#include "fitFCN.h"
#include "FitParameters.h"

void readParameters(std::string cardFile, std::vector<std::string>& name, std::map<std::string, double>& start, 
		    std::map<std::string, double>& min, std::map<std::string, double>& max, std::map<std::string, bool>& fix);

void readFakeDataPars(std::string cardFile, std::vector<std::string>& name, std::map<std::string, double>& start);

void readSamples(std::string cardFile, std::vector<std::string>& name, std::vector<bool>& fix, std::vector<double>& normVal);

void saveOutput(TFitterMinuit *minuit, TFile *outFile, fitFCN *thisFCN);

// Command line inputs
int testMinimize(std::string cardFile, TFile *outFile, std::string fitStrategy, std::string fakeDataFile, int type, std::string infile) { 

  std::vector<std::string>           params;
  std::map<std::string, double>      startVals, minValues, maxValues;
  std::map<std::string, bool>        fixValues;
  std::vector<std::string>           nSampleNames;
  std::vector<bool>                  nSampleShape;
  std::vector<double>                nSampleVal;

  std::vector<std::string>           nFakeParams;
  std::map<std::string,double>       nFakeStarts;

  // Read the card file for this fit
  std::cout<<"Reading Card Files"<<std::endl;
  readParameters(cardFile, params, startVals, minValues, maxValues, fixValues);
  readSamples(cardFile, nSampleNames, nSampleShape, nSampleVal);
  readFakeDataPars(cardFile, nFakeParams, nFakeStarts);

  // Override input parameters using file
  if (!infile.empty()){

      TFile* rootFile = new TFile(infile.c_str(),"READ");
      TTree* fitpars  = (TTree*)rootFile->Get("fit_results");
      fitpars->GetEntry(0);

      std::vector<std::string> *dialNames ;// = fitpars.parameter_names;
      std::vector<double> *dialVal        ;// = fitpars.parameter_values;
      std::vector<double> *dialMin        ;// = fitpars.parameter_low;
      std::vector<double> *dialMax        ;// = fitpars.parameter_high;
      std::vector<double> *dialFix        ;// = fitpars.parameter_fix;
      //      std::vector<double> dialStart      = fitpars->parameter_start;

      fitpars->SetBranchAddress("parameter_names",&dialNames);
      

      for (int i = 0; i < dialNames->size(); i++){
	
	if (startVals.find(dialNames->at(i)) != startVals.end()){
	  startVals[dialNames->at(i)] = dialVal->at(i);
	  minValues[dialNames->at(i)]   = dialMin->at(i);
	  maxValues[dialNames->at(i)]   = dialMax->at(i);
	  if (FitPar::Config().GetParI("FIX_INPUT_DIAL")) fixValues[dialNames->at(i)]   = 1.0;
	  else fixValues[dialNames->at(i)] = dialFix->at(i);
	}
      }
  }
    
  
    
  std::cout<<"---------------------------------------"<<std::endl;
  std::cout<<"Setting up generator libraries"<<std::endl;

  gSystem->Load("event1.so");

  t2krew::T2KReWeight *rw          = new t2krew::T2KReWeight();
  t2krew::T2KNeutReWeight *neut_rw = new t2krew::T2KNeutReWeight();
  t2krew::T2KNIWGReWeight *niwg_rw = new t2krew::T2KNIWGReWeight();

  rw->AdoptWghtEngine("neut_rw", neut_rw);
  rw->AdoptWghtEngine("niwg_rw", niwg_rw);

  // Decide which parameters are included in T2KReWeight
  for(unsigned int i = 0; i < params.size(); ++i){
    rw->Systematics().Include(t2krew::T2KSyst::FromString(params[i])); 
  }
  
 
  std::cout<<"---------------------------------------"<<std::endl;
  std::cout<<"Creating sample instances"<<std::endl;

  fitFCN *thisFCN = new fitFCN(cardFile, fakeDataFile, outFile, rw, 1);
  if (!fakeDataFile.empty() and fakeDataFile.compare("MC") != 0){

    std::cout<<"---------------------------------------"<<std::endl;
    std::cout<<"Creating fake data from external fit file shown in cardfile."<<std::endl;
    
    thisFCN->SetFakeData(fakeDataFile);
    
  }

  thisFCN->SetType(type);

  std::cout<<"---------------------------------------"<<std::endl;
  std::cout<<"Setting up Minuit."<<std::endl;

  gSystem->Load("libMinuit2");
 
  // Make minuit object and pass it the function to minimise
  TFitterMinuit *minuit = new TFitterMinuit();
  minuit->SetMinuitFCN(thisFCN);
  
  // Use low precision when finding minimum (speed)
  minuit->SetStrategy(0);

  // Use low precision in case the first derivatives are not to be relied upon
  minuit->SetPrecision(1E-5);
 
  std::cout << std::endl << "Including the following parameters in MINUIT: " << std::endl;

  // Counter to keep track of the parameter number
  int ipar = 0;
  std::vector<t2krew::T2KSyst_t> svec = rw->Systematics().AllIncluded();

  for(std::vector<t2krew::T2KSyst_t>::const_iterator parm_iter = svec.begin() ; parm_iter != svec.end(); ++parm_iter) {

    t2krew::T2KSyst_t syst = *parm_iter;
    std::string systString = t2krew::T2KSyst::AsString(syst);

    // Parameter values
    double vstart = 0;   // Parameter initial value
    double vlow   = 0;   // Parameter lower bound
    double vhigh  = 0;   // Parameter upper bound    
    
    if (startVals.find(systString) != startVals.end()) vstart = startVals.at(systString);
    if (minValues.find(systString) != minValues.end()) vlow   = minValues.at(systString);
    if (maxValues.find(systString) != maxValues.end()) vhigh  = maxValues.at(systString);
    if (fixValues.find(systString) == fixValues.end()) continue;

    rw->Systematics().SetTwkDial(syst,vstart);

    // Check if fixed        
    if (fixValues.at(systString) == 1) {
      minuit->SetParameter(ipar, systString.c_str(), vstart, 1., vlow, vhigh);
      minuit->FixParameter(ipar);
      std::cout << "Fixed: " << systString.c_str() << " at " << vstart << std::endl;
    } else {
      // Note, starting value is set to 0 (as this is the NEUT output), and the error corresponds to the 1 sigma error in T2KReWeight
      minuit->SetParameter(ipar, systString.c_str(), vstart, 1., vlow, vhigh);
      std::cout << "Free: " << systString.c_str() << " Initial: " << vstart << " Min: " << vlow << " Max: " << vhigh << std::endl;
    }
    ipar++;
  }

  // Now deal with the normalisation parameters
  for (int norm = 0; norm < int(nSampleNames.size()); ++norm){
    minuit->SetParameter(ipar, (nSampleNames[norm]+"_norm").c_str(), nSampleVal[norm], 0.05, 0.3, 1.7);

    std::cout<<"is fixed = "<<nSampleShape[norm]<<std::endl;
    // Fix any which shouldn't have a normalisation parameter
    if (!nSampleShape[norm]) minuit->FixParameter(ipar);
    
    ipar++;
  }

  std::cout << std::endl << "Number of Free Parameters = " << minuit->GetNumberFreeParameters() << std::endl;
  std::cout << "Total number of Parameters = " << minuit->GetNumberTotalParameters() << std::endl;


  if (fakeDataFile.compare("MC") == 0){
    std::cout<<"---------------------------------------"<<std::endl;
    std::cout<<"Setting fake data from MC starting prediction." <<std::endl<<std::endl;
    // Set twk dial values for fake MC
    for(int i = 0; i < nFakeParams.size(); i++){
      
      std::string systName = nFakeParams.at(i);
      std::cout<<"Setting Fake MC Dial "<<systName<<" to "<<nFakeStarts.at(systName)<<std::endl;
      rw->Systematics().Include(t2krew::T2KSyst::FromString(systName)); 
      rw->Systematics().SetTwkDial(t2krew::T2KSyst::FromString(systName), nFakeStarts.at(systName));

    }
   
    rw->Reconfigure();
    thisFCN->ReconfigureAllEvents();
    thisFCN->SetFakeData("MC");

    for(int i = 0; i < nFakeParams.size(); i++){
      rw->Systematics().ResetTwkDial(t2krew::T2KSyst::FromString(nFakeParams.at(i)));
    }
    std::cout<<"Set all data to fake MC predictions."<<std::endl;
  }

  std::cout<<"---------------------------------------"<<std::endl;
  std::cout<<" Beginning fit routine." <<std::endl;

  // Minimal MINUIT output
  minuit->SetPrintLevel(1);

  // If the PRESCAN fit option is given, scan the parameter space before starting the fit
  if (fitStrategy.compare("PRESCAN") == 0){
    std::cout<<"  --- PRESCAN --- "<<std::endl;
    minuit->CreateMinimizer(TFitterMinuit::kScan);
    // Perform the minimization
    minuit->Minimize();
  }

  // If the BRUTEFORCE fit option is given, fit and then scan several times to try and avoid local minima
  if (fitStrategy.compare("BRUTEFORCE") == 0){
    std::cout<<"  --- BRUTEFORCE --- "<<std::endl;
    minuit->CreateMinimizer(TFitterMinuit::kScan);
    minuit->Minimize();
    for (int count = 0; count < 2; ++count){
      minuit->CreateMinimizer(TFitterMinuit::kCombined);
      minuit->Minimize();
      minuit->CreateMinimizer(TFitterMinuit::kScan);
      minuit->Minimize();
    }
  }

  std::cout<<"  --- MINIMIZE --- "<<std::endl;
  // kCombined uses MIGRAD, reverting to SIMPLEX if the fitter fails.  
  minuit->CreateMinimizer(TFitterMinuit::kCombined);
  minuit->Minimize();

  // Now check if any of the parameters are at a parameter limit. If so, fix and re-minimise
  bool atLimit = false;

  char *name = new char[50];
  double val, err, min, max;

  // Grab best fit parameters
  for (int npar = 0; npar < minuit->GetNumberTotalParameters(); ++npar) {
    for (int c = 0; c < 50; ++c) name[c] = '\0';
    minuit->GetParameter(npar, name, val, err, min, max);  
    if (fabs(val - min) < 1E-3) {
      std::cout << "Parameter was at limit (low): " << name << std::endl;
      minuit->FixParameter(npar);
      atLimit = true;
    }
    if (fabs(max - val) < 1E-3) {
      std::cout << "Parameter was at limit (high): " << name << std::endl;
      minuit->FixParameter(npar);
      atLimit = true;
    }     
  }

  if (atLimit) {
    std::cout<<"  --- FIXED PARAMETER RE-MINIMIZE --- "<<std::endl;
    std::cout << "Fixed parameters at limit and now re-minimising..." << std::endl;
    minuit->Minimize();
  }

  std::cout<<"---------------------------------------"<<std::endl;
  std::cout<<"Running Final Reconfigure "<<std::endl;
  // Final reconfigure considering background loops
  thisFCN->ReconfigureAllEvents();

  std::cout<<"---------------------------------------"<<std::endl;
  std::cout<<"Saving Final Output. "<<std::endl;
  // Now store the required output in the root file
  saveOutput(minuit, outFile, thisFCN);
  

  delete thisFCN;
  delete neut_rw;
  delete niwg_rw;
  return 0; 
}



int main(int argc, char* argv[]){ 

  std::string cardFile;
  std::string outFile;
  std::string fitStrategy;
  std::string fakeDataFile;  
  std::string ParsFileOverride;
  std::vector<std::string> ParsOverrides;
  std::string InputFile;
  
  int type = 0;

  for (int i = 1; i< argc; ++i){
    if (i+1 != argc){
      // Cardfile
      if (!std::strcmp(argv[i], "-c")) {cardFile=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-o")) {outFile=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-f")) {fitStrategy=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-d")) {fakeDataFile=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-t")) {type=atoi(argv[i+1]); ++i;}
      else if (!std::strcmp(argv[i], "-p")) { ParsFileOverride=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-q")) { ParsOverrides.push_back(argv[i+1]); ++i;}
      else if (!std::strcmp(argv[i], "-i")) { InputFile=argv[i+1]; ++i;}
      else std::cerr << "ERROR: unknown command line option given!" << std::endl;
    }
  }

  // Set override parameters
  std::string defaultFile = std::string(std::getenv("EXT_FIT"))+"/parameters/fitter.requirements.dat";

  std::cout<<"---------------------------------------"<<std::endl;
  std::cout<<"Reading in requirements from "<<defaultFile<<std::endl;
  FitPar::Config().ReadParamFile(defaultFile);

  if (!ParsFileOverride.empty()){
    std::cout<<"Reading in parameter overrides from "<<ParsFileOverride<<std::endl;
    FitPar::Config().ReadParamFile(defaultFile);
  }

  if (!ParsOverrides.empty()){
    for (unsigned int iter = 0; iter < ParsOverrides.size(); iter++){
      FitPar::Config().ForceParam(ParsOverrides[iter]);
    }
  }
  std::cout<<"---------------------------------------"<<std::endl;
    

  // Open the ouput root file
  if (outFile.empty()) {
    std::cerr << "ERROR: output file not specified."<<std::endl;
  }
  
  TFile *out = new TFile(outFile.c_str(),"RECREATE");

int iret = testMinimize(cardFile, out, fitStrategy, fakeDataFile, type, InputFile);
  if (iret != 0) { 
    std::cerr << "ERROR: Minimize test failed !" << std::endl;
    return iret;
  }

  // Close the output file
  out->Close();

  delete out;
  std::cout<<"---------------------------------------"<<std::endl;
  std::cout<<"Complete."<<std::endl;
  std::cout<<"---------------------------------------"<<std::endl;
  return 0;
}




void readParameters(std::string cardFile, std::vector<std::string>& name, std::map<std::string, double>& start, 
		    std::map<std::string, double>& min, std::map<std::string, double>& max, std::map<std::string, bool>& fix){

  std::string line;
  std::ifstream card(cardFile.c_str(), ifstream::in);

  while(std::getline(card, line, '\n')){
    std::istringstream stream(line);
    
    std::string token;
    int val = 0;  
    std::string thisName;
    double minVal   = 0;
    double maxVal   = 0;
    double startVal = 0;

    // check the type
    while(std::getline(stream, token, ' ')){
      // Strip any leading whitespace from the stream
      stream >> std::ws;

      // Ignore comments
      if (token.c_str()[0] == '#') continue;

      std::istringstream stoken(token);
      
      if (val == 0){
	if (token.compare("parameter") !=0 ) {
	  break;
	}
      } else if (val == 1) {
	name   .push_back(token);
	thisName = token;
      } else if (val == 2) {
	stoken >> startVal;
	start  .insert( map<std::string, double>::value_type(thisName, startVal) );	
      } else if (val == 3) {
	stoken >> minVal;
	min    .insert( map<std::string, double>::value_type(thisName, minVal) );
      } else if (val == 4) {
	stoken >> maxVal;
	max    .insert( map<std::string, double>::value_type(thisName, maxVal) );
      } else if (val == 5){
	if (token.compare("FIX") == 0) {
	  fix  .insert( map<std::string, bool>::value_type(thisName, 1) );
	} else fix .insert( map<std::string, bool>::value_type(thisName, 0) );
      } else break;

      val++;
    }
  }
  card.close();
}


void readFakeDataPars(std::string cardFile, std::vector<std::string>& name, std::map<std::string, double>& start){

  std::string line;
  std::ifstream card(cardFile.c_str(), ifstream::in);

  while(std::getline(card, line, '\n')){
    std::istringstream stream(line);
    
    std::string token;
    int val = 0;  
    std::string thisName;
    double startVal = 0;

    // check the type
    while(std::getline(stream, token, ' ')){
      // Strip any leading whitespace from the stream
      stream >> std::ws;

      // Ignore comments
      if (token.c_str()[0] == '#') continue;

      std::istringstream stoken(token);
      
      if (val == 0){
	if (token.compare("fake_parameter") !=0 ) {
	  break;
	}
      } else if (val == 1) {
	name   .push_back(token);
	thisName = token;
      } else if (val == 2) {
	stoken >> startVal;
	start  .insert( map<std::string, double>::value_type(thisName, startVal) );	
      } else break;

      val++;
    }
  }
  card.close();
};




void readSamples(std::string cardFile, std::vector<std::string>& name, std::vector<bool>& fix, std::vector<double>& normVal){

  std::string line;
  std::ifstream card(cardFile.c_str(), ifstream::in);

  while(std::getline(card, line, '\n')){
    std::istringstream stream(line);
    std::string token;
    int val = 0;
    std::string thisName;

    // check the type
    while(std::getline(stream, token, ' ')){
      // Strip any leading whitespace from the stream
      stream >> std::ws;

      // Ignore comments
      if (token.c_str()[0] == '#') continue;

      std::istringstream stoken(token);
      
      if (val == 0){
	if (token.compare("sample") !=0 ) {
	  break;
	}
      } else if (val == 1) {
	name .push_back(token);
	thisName = token;
      } else if (val == 2) {
	if (token.find("FREE") != std::string::npos) fix.push_back(1); // dont try and vary norm of a ratio.
	else fix.push_back(0);
	normVal.push_back(1.0);
      } else if (val == 3) {
      } else if (val == 4) {
	double tempVal;
	stoken >> tempVal;
	normVal[name.size()-1] = tempVal;
      } else break;

      val++;
    }
  }
  card.close();
}




void saveOutput(TFitterMinuit *minuit, TFile *outFile, fitFCN *thisFCN){

  // This function saves all of the interesting output in the output ROOT file
  Int_t nPars = minuit->GetNumberFreeParameters();
  // Create vectors to store useful information in
  std::vector<std::string> nameVect;
  std::vector<double>      valVect;
  std::vector<double>      errVect;
  std::vector<double>      minVect;
  std::vector<double>      maxVect;
  std::vector<double>      startVect;
  std::vector<double>      fixVect;
  
  // A fixed length character array seems to be the only way to get the name from minuit. Disgusting stuff!
  char *name = new char[50];
  double val, err, min, max;

  // Grab best fit parameters
  for (int npar = 0; npar < minuit->GetNumberTotalParameters(); ++npar) {
    for (int c = 0; c < 50; ++c) name[c] = '\0';
    minuit->GetParameter(npar, name, val, err, min, max);

  

    nameVect.push_back(std::string(name, strlen(name)));    
    valVect .push_back(val);
    minVect .push_back(min);
    maxVect .push_back(max);

    // Skip any fixed parameters
    if (minuit->IsFixed(npar)){
      errVect .push_back(0);
      fixVect .push_back(1);
    }    else {
      errVect .push_back(err);
      fixVect .push_back(0);
    }
  }
  delete name;


  double amin, edm, errdef;
  int nvpar, nparx;
  minuit->GetStats(amin, edm, errdef, nvpar, nparx);
  int NDOF = thisFCN->GetNDOF() - nPars;
  std::cout << "Minimum chi^2 = " << amin << std::endl;
  std::cout << "NDOF = " << NDOF << " (# of free pars already subtracted)" << std::endl;

  outFile  ->cd();
  
  if (minuit->GetCovarianceMatrix() != NULL){
    // Grab parameter covariance matrix  
    TMatrixD FitParCov(nPars, nPars, minuit->GetCovarianceMatrix());
    TH2D covarHist(FitParCov);
    TH2D corrHist (covarHist);
    
    int xBin = 0;
    // Set labels and sort out correlation matrix
    for (int ipar = 0; ipar < minuit->GetNumberTotalParameters(); ++ipar){
      if (minuit->IsFixed(ipar)) continue;
      
      covarHist.GetXaxis()->SetBinLabel(xBin+1, nameVect[ipar].c_str());
      covarHist.GetYaxis()->SetBinLabel(xBin+1, nameVect[ipar].c_str());
      corrHist .GetXaxis()->SetBinLabel(xBin+1, nameVect[ipar].c_str());
      corrHist .GetYaxis()->SetBinLabel(xBin+1, nameVect[ipar].c_str());
      
      int yBin = 0;
      for (int jpar = 0; jpar < minuit->GetNumberTotalParameters(); ++jpar){
	if (minuit->IsFixed(jpar)) continue;	
	corrHist .SetBinContent(xBin+1, yBin+1, corrHist.GetBinContent(xBin+1, yBin+1)/(errVect[ipar]*errVect[jpar] +0.0));
	++yBin;
      }
      ++xBin;
    }
    
    covarHist.Write("covariance");
    corrHist .Write("correlation");
  }

  // Create a TTree and save the output of the fit
  TTree *tree = new TTree("fit_results", "fit_results");
  tree ->Branch("parameter_names",  &nameVect);
  tree ->Branch("parameter_values", &valVect);
  tree ->Branch("parameter_errors", &errVect);
  tree ->Branch("parameter_min",    &minVect);
  tree ->Branch("parameter_max",    &maxVect);
  tree ->Branch("parameter_fix",    &fixVect);
  tree ->Branch("chi2", &amin, "amin/D");
  tree ->Branch("NDOF", &NDOF, "NDOF/I");

  tree ->Fill();
  tree ->Write();
  
  delete tree;
  return;
}
