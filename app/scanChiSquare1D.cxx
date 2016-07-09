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

/**
   Author: Callum Wilkinson   January 2014
   
   Usage: ./scanChiSquare1D.exe -c card file which gives the values of parameters to be fixed, the range and density of parameters to be scanned, 
                                   and the samples the be included in the chi-square calculation.
                                -o output file in which to same the produced TGraphs   
*/
#include "TH1.h"
#include "TF1.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TVirtualFitter.h"
#include "TStyle.h"
#include "Minuit2/FCNBase.h"
#include "TSystem.h"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cstring>

#include "T2KSyst.h"
#include "T2KNeutReWeight.h"
#include "T2KNIWGReWeight.h"

#include "fitFCN.h"

void readParameters(std::string cardFile, std::vector<std::string>& name, std::map<std::string, double>& min, 
		    std::map<std::string, double>& max, std::map<std::string, int>& fix, std::map<std::string, int>& density);

void readSamples(std::string cardFile, std::vector<std::string>& name, std::vector<bool>& fix, std::vector<double>& normVal);

// Command line inputs
int testMinimize(std::string cardFile, TFile *outFile) { 

  std::vector<std::string>       params;
  std::map<std::string, double>  minValues;
  std::map<std::string, double>  maxValues;
  std::map<std::string, int>     fixValues;
  std::map<std::string, int>     denValues;

  std::vector<std::string>       nSampleNames;
  std::vector<bool>              nSampleShape;
  std::vector<double>            nSampleVal;

  // Read the card file for this fit
  readParameters(cardFile, params, minValues, maxValues, fixValues, denValues);
  readSamples(cardFile, nSampleNames, nSampleShape, nSampleVal);

  gSystem->Load("libMinuit2");

  t2krew::T2KReWeight *rw = new t2krew::T2KReWeight(); 
  
  rw->AdoptWghtEngine("neut_rw", new t2krew::T2KNeutReWeight());
  rw->AdoptWghtEngine("niwg_rw", new t2krew::T2KNIWGReWeight());

  // Decide which parameters are included in T2KReWeight
  for(unsigned int i = 0; i < params.size(); ++i){
    rw->Systematics().Include(t2krew::T2KSyst::FromString(params[i])); 
  }

  std::vector<t2krew::T2KSyst_t> svec = rw->Systematics().AllIncluded();

  // Make minuit object and pass it the function to minimise
  TFitterMinuit * minuit = new TFitterMinuit();

  // Don't save any best fit distributions
  fitFCN thisFCN(cardFile, outFile, rw, 0);
  minuit->SetMinuitFCN(&thisFCN);

  // Create a list of the systematics to scan through
  std::vector<string> scanList; 
  std::vector<t2krew::T2KSyst_t>::const_iterator p_iter;
  for(p_iter = svec.begin() ; p_iter != svec.end(); ++p_iter) {
    t2krew::T2KSyst_t syst = *p_iter;
    std::string systString = t2krew::T2KSyst::AsString(syst);
    if (fixValues.find(systString) != fixValues.end()) 
      if (fixValues.at(systString) == 2) scanList.push_back(systString);
  }
  
  std::cout << std::endl << "Creating chi-square TGraphs for " << scanList.size() << " systematics." << std::endl;
  
  // Now loop over the systematics to iterate through
  for (std::vector<string>::iterator it = scanList.begin(); it != scanList.end(); ++it){
    
    std::cout << std::endl << "Looping over values for: " << *it << std::endl;

    // This is where the values get stored for the TGraph
    std::vector<double> chiSq;
    std::vector<double> values;

    // Now loop over the values of the systematic to scan through
    double scanMin = 0.;
    double scanMax = 0.;
    double scanInc = 0.;

    if (minValues.find(*it) != minValues.end()) scanMin = minValues.at(*it);
    if (maxValues.find(*it) != maxValues.end()) scanMax = maxValues.at(*it);
    if (denValues.find(*it) != denValues.end()) scanInc = (scanMax - scanMin)/(denValues.at(*it)+0.);
    else continue;

    for (double val = scanMin; val < scanMax+scanInc; val=val+scanInc){
       
      int ipar=0;     
      std::vector<t2krew::T2KSyst_t>::const_iterator parm_iter;

      // Now loop over all of the systematics to be included
      for(parm_iter = svec.begin() ; parm_iter != svec.end(); ++parm_iter) {
	t2krew::T2KSyst_t syst = *parm_iter;
	std::string systString = t2krew::T2KSyst::AsString(syst);
	
	// Parameter values
	double vhigh = 0;   // Parameter upper bound
	double vlow  = 0;   // Parameter lower bound    	
	
	if (minValues.find(systString) != minValues.end()) vlow = minValues.at(systString);
	if (maxValues.find(systString) != maxValues.end()) vhigh = maxValues.at(systString);
	if (fixValues.find(systString) == fixValues.end()) continue;
	
	// Check if this is a parameter to be scanned. If this is the current parameter being scanned, set to the current value
	// If this is not, set to halfway between the max and min set
	if (fixValues.at(systString) == 2) {
	  if (*it == systString){
	    minuit->SetParameter(ipar, systString.c_str(), val, 1., vlow, vhigh);
	  } else 
	    minuit->SetParameter(ipar, systString.c_str(), (vhigh - vlow)/2.0 + vlow, 1, vlow, vhigh);
	    
	  minuit->FixParameter(ipar);
	} else if (fixValues.at(systString) == 1) {
	  minuit->SetParameter(ipar, systString.c_str(), vlow, 1., vlow, vhigh);
	  minuit->FixParameter(ipar);
	}
	else {
	  // Note, starting value is set to 0 (as this is the NEUT output), and the error corresponds to the 1 sigma error in T2KReWeight
	  minuit->SetParameter(ipar,systString.c_str(), 0., 1., vlow, vhigh);
	}
	
	ipar++;
      }
      
      // Now deal with the normalisation parameters
      for (int norm = 0; norm < int(nSampleNames.size()); ++norm){
	// For all normalisation parameters, 
	minuit->SetParameter(ipar, (nSampleNames[norm]+"_norm").c_str(), nSampleVal[norm], 0.05, 0.3, 1.7);
	
	// Fix any which shouldn't have a normalisation parameter
	if (!nSampleShape[norm]) minuit->FixParameter(ipar);
	
	ipar++;
      }
      
      // Now minimise (some parameters may be free in the scan)
      minuit->SetPrintLevel(-1);
      minuit->CreateMinimizer();
      int iret = minuit->Minimize();
      
      // Exit function now if there is a problem...
      if (iret != 0) { 
	return iret; 
      }
      
      // Now append the chiSq and value of the scanned parameter to the vector
       double amin, edm, errdef;
       int nvpar, nparx;
       minuit->GetStats(amin, edm, errdef, nvpar, nparx);
       
       chiSq  .push_back(amin);
       values .push_back(val);
       
    }
    // Now make a TGraph for this systematic
    TGraph *gr = new TGraph(chiSq.size(), &(values[0]), &(chiSq[0]));
    gr ->SetTitle((*it+"_SCAN").c_str());
    gr ->SetName((*it+"_SCAN").c_str());
    
    outFile ->cd();
    gr      ->Write();

    delete gr;
  }

  return 0; 
}



int main(int argc, char* argv[]){ 

  std::string cardFile;
  std::string outFile;

  for (int i = 1; i< argc; ++i){
    if (i+1 != argc){
      // Cardfile
      if (!std::strcmp(argv[i], "-c")) {cardFile=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-o")) {outFile=argv[i+1]; ++i;}
      else std::cerr << "ERROR: unknown command line option given!" << std::endl;
    }
  }

  // Open the output file
  TFile *out  = new TFile(outFile.c_str(),"RECREATE");

  int iret = testMinimize(cardFile, out);
  if (iret != 0) { 
    std::cerr << "ERROR: Minimize test failed !" << std::endl;
    return iret;
  }

  // Close the output file
  out->Close();

  delete out;
  return 0;
}



void readParameters(std::string cardFile, std::vector<std::string>& name, std::map<std::string, double>& min, std::map<std::string, double>& max, 
		    std::map<std::string, int>& fix, std::map<std::string, int>& density){

  std::string line;
  std::ifstream card(cardFile.c_str(), ifstream::in);

  while(std::getline(card, line, '\n')){
    std::istringstream stream(line);
    
    std::string token;
    int val = 0;
    
    std::string thisName;
    double minVal = 0;
    double maxVal = 0;
    double denVal = 0;
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
	name .push_back(token);
	thisName = token;
      } else if (val == 2) {
	stoken >> minVal;
	min  .insert( map<std::string, double>::value_type(thisName, minVal) );
      } else if (val == 3) {
	stoken >> maxVal;
	max  .insert( map<std::string, double>::value_type(thisName, maxVal) );
      } else if (val == 4){
	if (token.compare("FIX") == 0) {
	  fix  .insert( map<std::string, int>::value_type(thisName, 1) );
	} else if (token.compare("SCAN") == 0) {
	  fix  .insert( map<std::string, int>::value_type(thisName, 2) );
	} else fix .insert( map<std::string, int>::value_type(thisName, 0) );
      } else if (val == 5){
	stoken >> denVal;
	density .insert( map<std::string, int>::value_type(thisName, denVal) );
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
	double val;
	stoken >> val;
	normVal[name.size()-1] =val;
      } else break;

      val++;
    }
  }
  card.close();
}
