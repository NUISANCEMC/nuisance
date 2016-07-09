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

// Author: Callum Wilkinson    07/2014  
/**
   Usage: ./reweightArbitraryFlux.exe -c card file, where samples and parameters are defined
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

#include "testFCN.h"

void readParameters(std::string cardFile, std::vector<std::string>& name, std::map<std::string, double>& start, 
		    std::map<std::string, double>& min, std::map<std::string, double>& max, std::map<std::string, bool>& fix);

void readSamples(std::string cardFile, std::vector<std::string>& name, std::vector<bool>& fix, std::vector<double>& normVal);

void saveOutput(TFitterMinuit *minuit, TFile *outFile);

// Command line inputs
int testMinimize(std::string cardFile, TFile *outFile, std::string fitStrategy) { 

  std::vector<std::string>           params;
  std::map<std::string, double>      startVals, minValues, maxValues;
  std::map<std::string, bool>        fixValues;
  std::vector<std::string>           nSampleNames;
  std::vector<bool>                  nSampleShape;
  std::vector<double>                nSampleVal;

  // Read the card file for this fit
  readParameters(cardFile, params, startVals, minValues, maxValues, fixValues);
  readSamples(cardFile, nSampleNames, nSampleShape, nSampleVal);

  gSystem->Load("libMinuit2");

  t2krew::T2KReWeight *rw          = new t2krew::T2KReWeight();
  t2krew::T2KNeutReWeight *neut_rw = new t2krew::T2KNeutReWeight();
  t2krew::T2KNIWGReWeight *niwg_rw = new t2krew::T2KNIWGReWeight();

  rw->AdoptWghtEngine("neut_rw", neut_rw);
  rw->AdoptWghtEngine("niwg_rw", niwg_rw);

  // Decide which parameters are included in T2KReWeight
  for(unsigned int i = 0; i < params.size(); ++i){
    rw->Systematics().Include(t2krew::T2KSyst::FromString(params[i])); 
  }
  
  // Make minuit object and pass it the function to minimise
  TFitterMinuit *minuit = new TFitterMinuit();
  testFCN *thisFCN = new testFCN(cardFile, outFile, rw, 1);
  minuit->SetMinuitFCN(thisFCN);

  // Use low precision when finding minimum (speed)
  minuit->SetStrategy(0);
  // Use low precision in case the first derivatives are not to be relied upon
  minuit->SetPrecision(1E-5);
 
  std::cout << std::endl << "Using the T2KReWeight parameters: " << std::endl;

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

    // Check if fixed        
    if (fixValues.at(systString) == 1) {
      minuit->SetParameter(ipar, systString.c_str(), vstart, 1., vlow, vhigh);
      minuit->FixParameter(ipar);
      std::cout << "Setting: " << systString.c_str() << " to " << vstart << std::endl;
    }
    ipar++;
  }

  // Now deal with the normalisation parameters
  for (int norm = 0; norm < int(nSampleNames.size()); ++norm){
    minuit->SetParameter(ipar, (nSampleNames[norm]+"_norm").c_str(), nSampleVal[norm], 0.05, 0.3, 1.7);

    // Fix any which shouldn't have a normalisation parameter
    if (!nSampleShape[norm]) minuit->FixParameter(ipar);
    ipar++;
  }

  // Minimal MINUIT output
  minuit->SetPrintLevel(1);

  // kCombined uses MIGRAD, reverting to SIMPLEX if the fitter fails.  
  minuit->CreateMinimizer(TFitterMinuit::kCombined);
  minuit->Minimize();

  // Now store the required output in the root file
  saveOutput(minuit, outFile);
  
  delete neut_rw;
  delete niwg_rw;
  delete thisFCN;
  return 0; 
}



int main(int argc, char* argv[]){ 

  std::string cardFile;
  std::string outFile;
  std::string fitStrategy;

  for (int i = 1; i< argc; ++i){
    if (i+1 != argc){
      // Cardfile
      if (!std::strcmp(argv[i], "-c")) {cardFile=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-o")) {outFile=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-f")) {fitStrategy=argv[i+1]; ++i;}
      else std::cerr << "ERROR: unknown command line option given!" << std::endl;
    }
  }

  // Open the ouput root file
  TFile *out = new TFile(outFile.c_str(),"RECREATE");

  int iret = testMinimize(cardFile, out, fitStrategy);
  if (iret != 0) { 
    std::cerr << "ERROR: Minimize test failed !" << std::endl;
    return iret;
  }

  // Close the output file
  out->Close();

  delete out;
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




void readSamples(std::string cardFile, std::vector<std::string>& name, std::vector<bool>& fix, std::vector<double>& normVal){

  std::string line;
  std::ifstream card(cardFile.c_str(), ifstream::in);

  while(std::getline(card, line, '\n')){
    std::istringstream stream(line);
    std::string token;
    int val = 0;

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
      } else if (val == 2) {
	if (token.compare("FREE") == 0) fix.push_back(1);
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




void saveOutput(TFitterMinuit *minuit, TFile *outFile){
  // This function saves all of the interesting output in the output ROOT file

  // Create vectors to store useful information in
  std::vector<std::string> nameVect;
  std::vector<double>      valVect;

  // A fixed length character array seems to be the only way to get the name from minuit. Disgusting stuff!
  char *name = new char[50];
  double val, err, min, max;

  // Grab best fit parameters
  for (int npar = 0; npar < minuit->GetNumberTotalParameters(); ++npar) {
    for (int c = 0; c < 50; ++c) name[c] = '\0';
    minuit->GetParameter(npar, name, val, err, min, max);

    nameVect.push_back(std::string(name, strlen(name)));    
    valVect .push_back(val);
  }
  delete name;

  double amin, edm, errdef;
  int nvpar, nparx;
  minuit->GetStats(amin, edm, errdef, nvpar, nparx);

  outFile  ->cd();

  // Create a TTree and save the output of the fit
  TTree *tree = new TTree("fit_results", "fit_results");
  tree ->Branch("parameter_names",  &nameVect);
  tree ->Branch("parameter_values", &valVect);

  tree ->Fill();
  tree ->Write();
  
  delete tree;
  return;
}
