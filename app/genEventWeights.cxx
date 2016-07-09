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

// Author: Patrick Stowell    10/2015
/**
   Usage: ./genEventWeights.exe -c card file, where samples and parameters are defined
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
#include "minimizerFCN.h"
#include "FitParameters.h"

//********************************************************************  
void readParameters(std::string cardFile, std::vector<std::string>& name, std::map<std::string, double>& start, 
		    std::map<std::string, double>& min, std::map<std::string, double>& max, std::map<std::string, bool>& fix, std::map<std::string, int>& nKnots,
		    std::map<std::string, bool>& splines);

//********************************************************************  
void readFakeDataPars(std::string cardFile, std::vector<std::string>& name, std::map<std::string, double>& start);

//********************************************************************  
void readSamples(std::string cardFile, std::vector<std::string>& name, std::vector<bool>& fix, std::vector<double>& normVal);

// Command line inputs
//********************************************************************  
int testMinimize(std::string cardFile, TFile *outFile, std::string fitStrategy, std::string fakeDataFile, int type, std::string infile) { 

  std::vector<std::string>           params;
  std::map<std::string, double>      startVals, minValues, maxValues;
  std::map<std::string, bool>        fixValues;
  std::vector<std::string>           nSampleNames;
  std::vector<bool>                  nSampleShape;
  std::vector<double>                nSampleVal;
  std::map<std::string,int>                   knotVals;
  std::vector<std::string>           nFakeParams;
  std::map<std::string,double>       nFakeStarts;
  std::map<std::string,bool>         useSplines;
  
  // Read the card file for this fit
  std::cout<<"Reading Card Files"<<std::endl;
  readParameters(cardFile, params, startVals, minValues, maxValues, fixValues, knotVals, useSplines);
  readSamples(cardFile, nSampleNames, nSampleShape, nSampleVal);
  readFakeDataPars(cardFile, nFakeParams, nFakeStarts);

  FitPar::Variable().SaveEventSplines = true;
  FitPar::Variable().SplineOutputFile = outFile;
  FitPar::Config().out = outFile;
  
  outFile->cd();
  // make a header tree for splines
  std::vector<std::string> FreeDial_names;
  std::vector<bool> FreeDial_free;
  std::vector<double> FreeDial_start;
  std::vector<double> FreeDial_high;
  std::vector<double> FreeDial_low;
  std::vector<int> FreeDial_knots;
  std::vector<bool> FreeDial_spline;

  TTree* headerTree = new TTree("splines_header","splines_header");
  
  headerTree->Branch("dial_names",&FreeDial_names);
  headerTree->Branch("dial_free",&FreeDial_free);
  headerTree->Branch("dial_start",&FreeDial_start);
  headerTree->Branch("dial_high",&FreeDial_high);
  headerTree->Branch("dial_low",&FreeDial_low);
  headerTree->Branch("dial_knots",&FreeDial_knots);
  headerTree->Branch("dial_spline",&FreeDial_spline);


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

  std::cout<<"Loading event1.so"<<std::endl;
  gSystem->Load("event1.so");

  std::cout<<"Loading t2k ReWeight"<<std::endl;
  t2krew::T2KReWeight *rw          = new t2krew::T2KReWeight();

  std::cout<<"Loading t2k Neut ReWeight"<<std::endl;
  t2krew::T2KNeutReWeight *neut_rw = new t2krew::T2KNeutReWeight();
  
  std::cout<<"Loading t2k NIWG ReWeight"<<std::endl;
  t2krew::T2KNIWGReWeight *niwg_rw = new t2krew::T2KNIWGReWeight();

  rw->AdoptWghtEngine("neut_rw", neut_rw);
  rw->AdoptWghtEngine("niwg_rw", niwg_rw);
  std::cout<<"Including dials in T2K ReWeight "<<std::endl;

  //std::map<std::string,std::vector< t2krew::T2KReWeight > > all_rw;
  FitPar::Variable().all_rw["NOMINAL"];

  // Counter to keep track of the parameter number  
  std::cout<<"N pars = "<<params.size()<<std::endl;
  for(int i = 0; i < params.size(); i++) {
    
    std::string systString = params.at(i);
    t2krew::T2KSyst_t syst = t2krew::T2KSyst::FromString(params.at(i));

    // Parameter values
    double vstart = 0;   // Parameter initial value
    double vlow   = 0;   // Parameter lower bound
    double vhigh  = 0;   // Parameter upper bound    
    int nKnots = 0;
    bool splineSwitch;
    
    if (startVals.find(systString) != startVals.end()) vstart = startVals.at(systString);
    if (minValues.find(systString) != minValues.end()) vlow   = minValues.at(systString);
    if (maxValues.find(systString) != maxValues.end()) vhigh  = maxValues.at(systString);
    if (knotVals.find(systString) != knotVals.end())   nKnots = knotVals.at(systString);
    if (useSplines.find(systString) != useSplines.end()) splineSwitch = useSplines.at(systString);
    if (fixValues.find(systString) == fixValues.end()) continue;
    
    rw->Systematics().Include(syst); 

    FreeDial_names.push_back(systString);
    FreeDial_start.push_back(vstart);
    FreeDial_high.push_back(vhigh);
    FreeDial_low.push_back(vlow);
    FreeDial_knots.push_back(nKnots);
    FreeDial_spline.push_back(splineSwitch);

    FitPar::Variable().AddDialSpline(systString,splineSwitch);

    // Check if fixed        
    if (fixValues.at(systString) == 1) {
      rw->Systematics().SetTwkDial(syst,vstart);
      std::cout << "Fixed: " << systString.c_str() << " at " << vstart << std::endl;
      FitPar::Variable().AddFixedDial(systString,0);
      FreeDial_free.push_back(0);
    } else {

      // Note, starting value is set to 0 (as this is the NEUT output), and the error corresponds to the 1 sigma error in T2KReWeight
      std::cout << "Free: " << systString.c_str() << " NKnots: " << nKnots << " Min: " << vlow << " Max: " << vhigh << std::endl;
      rw->Systematics().SetTwkDial(syst,0.0);

      FitPar::Variable().SetSplineLimits(systString,int(nKnots),vlow,vhigh);
      
      int myKnots = (FitPar::Variable().nSplineKnots(t2krew::T2KSyst::AsString(syst)));
      std::cout<<"MyKnots = "<<myKnots<<std::endl;
      std::cout<<"Adding free dial "<<systString<<std::endl;
      FitPar::Variable().AddFreeDial(systString);
      FitPar::Variable().AddFreeDial(systString,1);
      
      // if (fixValues[params[i]]) FitPar::Variable().AddFixedDial(params[i],splinedial);
      // else FitPar::Variable().AddFreeDial(params[i],splinedial);

      FitPar::Variable().all_rw[systString];
      FitPar::Variable().all_neutrw[systString];
      FitPar::Variable().all_niwgrw[systString];

      bool useSpline = true;
      int nKnots = 20;

      FreeDial_free.push_back(1);

    }	  
  }

  rw->Reconfigure();

  FitPar::Variable().all_rw["NOMINAL"];
  FitPar::Variable().all_rw["NOMINAL"].push_back(rw);
  
  FitPar::Variable().all_neutrw["NOMINAL"];
  FitPar::Variable().all_neutrw["NOMINAL"].push_back(neut_rw);

  FitPar::Variable().all_niwgrw["NOMINAL"];
  FitPar::Variable().all_niwgrw["NOMINAL"].push_back(niwg_rw);
   
  // Save Heeader
  outFile->cd();
  headerTree->Fill();
  headerTree->Write();

  // Doing it this way is far quicker, but limited to a set number of dials unfortunately.
  // Should change this when T2KReweight is fixed.
  std::cout<<"Making reweight instance for all possible points"<<std::endl;

  // Get starting values
  // Make a copy of T2K ReWeight for all possible input variations.
  std::vector<t2krew::T2KSyst_t> svec = rw->Systematics().AllIncluded();
  std::vector<double> oldDialVals;

  for(std::vector<t2krew::T2KSyst_t>::const_iterator parm_iter = svec.begin() ; parm_iter != svec.end(); ++parm_iter) {

    t2krew::T2KSyst_t syst = *parm_iter;
    std::string systString = (t2krew::T2KSyst::AsString(syst));
    std::cout<<"i"<<std::endl;

    if (FitPar::Variable().isDialFree(systString)){
      
      
      std::cout<<"Getting values"<<t2krew::T2KSyst::AsString(syst)<<std::endl;
      std::cout<<"Vals = "<<FitPar::Variable().nSplineKnots(t2krew::T2KSyst::AsString(syst))<<std::endl;
      int nKnots        = FitPar::Variable().nSplineKnots(t2krew::T2KSyst::AsString(syst));

      std::cout<<"Dial Knots = "<<nKnots<<std::endl;
      double lowSpline  = FitPar::Variable().SplineMin(t2krew::T2KSyst::AsString(syst));
      std::cout<<"Dial low "<<lowSpline<<std::endl;
      double highSpline = FitPar::Variable().SplineMax(t2krew::T2KSyst::AsString(syst));
      std::cout<<"High spline = "<<highSpline<<std::endl;
      double curVal;
      std::cout<<"Got them"<<std::endl;
      for (int i = 0; i < nKnots+2; i++){
	std::cout<<"New rw"<<std::endl;
	t2krew::T2KReWeight* tempRW = new t2krew::T2KReWeight();
	t2krew::T2KNeutReWeight *temp_neut_rw = new t2krew::T2KNeutReWeight();
	t2krew::T2KNIWGReWeight *temp_niwg_rw = new t2krew::T2KNIWGReWeight();

	tempRW->AdoptWghtEngine("neut_rw", temp_neut_rw);
	tempRW->AdoptWghtEngine("niwg_rw", temp_niwg_rw);
	
	std::cout<<i<<" : Cur val = "<<curVal<<std::endl;
	curVal = lowSpline + (i+0.)*(highSpline - lowSpline)/(nKnots+0.);
	
	std::cout<<"j"<<std::endl;
	
	for(std::vector<t2krew::T2KSyst_t>::const_iterator parm_iter2 = svec.begin() ; parm_iter2 != svec.end(); ++parm_iter2) {
	  t2krew::T2KSyst_t syst2 = *parm_iter2;
	  std::string systString2 = (t2krew::T2KSyst::AsString(syst2));
	  tempRW->Systematics().Include(syst2);
	  if (FitPar::Variable().isDialFree(systString2)){
	    //	    tempRW->Systematics().ResetTwkDial(syst2);
	    tempRW->Systematics().SetTwkDial(syst,0.0);
	    std::cout<<"Not resetting"<<std::endl;
	  } else { 
	    tempRW->Systematics().SetTwkDial(syst2, rw->Systematics().CurTwkDialVal(syst2));
	  }
	}
	tempRW->Systematics().SetTwkDial(syst,curVal);

	tempRW->Reconfigure();
	std::cout<<"Pushing back the rw engines"<<std::endl;
	(FitPar::Variable().all_rw.at(t2krew::T2KSyst::AsString(syst))).push_back(tempRW);
	(FitPar::Variable().all_neutrw.at(t2krew::T2KSyst::AsString(syst))).push_back(temp_neut_rw);
	(FitPar::Variable().all_niwgrw.at(t2krew::T2KSyst::AsString(syst))).push_back(temp_niwg_rw);
	// tempRW = NULL;
	// temp_neut_rw = NULL;
	// temp_niwg_rw = NULL;

      }
    }
    std::cout<<"Done"<<std::endl;
  }

 
  std::cout<<"---------------------------------------"<<std::endl;
  std::cout<<"Creating sample instances"<<std::endl;

  minimizerFCN *thisFCN = new minimizerFCN(cardFile, fakeDataFile, outFile, rw, 1);
  if (!fakeDataFile.empty() and fakeDataFile.compare("MC") != 0){

    std::cout<<"---------------------------------------"<<std::endl;
    std::cout<<"Creating fake data from external fit file shown in cardfile."<<std::endl;
    
    thisFCN->SetFakeData(fakeDataFile);
    
  }

  thisFCN->SetType(type);


  std::cout<<"-----------------------------------------"<<std::endl;
  std::cout<<" Generating Splines for events "<<std::endl;

  thisFCN->ReconfigureAllEvents();


  std::cout<<"---------------------------------------"<<std::endl;
  std::cout<<"Saving Final Output. "<<std::endl;

  // Now store the required output in the root file
  //  outFile->cd();
  thisFCN->Write();

  delete thisFCN;
  delete neut_rw;
  delete niwg_rw;
  return 0; 
}


//********************************************************************  
int main(int argc, char* argv[]){
//********************************************************************  

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



//********************************************************************  
void readParameters(std::string cardFile, std::vector<std::string>& name, std::map<std::string, double>& start, 
		    std::map<std::string, double>& min, std::map<std::string, double>& max, std::map<std::string, bool>&  fix, std::map<std::string, int>& nKnots,
		    std::map<std::string, bool>& splines){
//********************************************************************
  
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
    int knotVal = 0;
    bool splineSwitch = false;
    
    // check the type
    while(std::getline(stream, token, ' ')){
      // Strip any leading whitespace from the stream
      stream >> std::ws;

      // Ignore comments
      if (token.c_str()[0] == '#') continue;
      std::cout<<token<<std::endl;
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
      } else if (val == 5) {
	stoken >> knotVal;
	nKnots    .insert( map<std::string, int>::value_type(thisName, knotVal) );
      } else if (val == 6){
	if (token.compare("FIX") == 0) {
	  fix  .insert( map<std::string, bool>::value_type(thisName, 1) );
	} else fix .insert( map<std::string, bool>::value_type(thisName, 0) );
      } else if (val == 7){
	if (token.compare("TGRAPH") == 0) {
	  splines  .insert( map<std::string, bool>::value_type(thisName, 1) );
	} else splines .insert( map<std::string, bool>::value_type(thisName, 0) );
      }else break;
      val++;
    }
  }
  card.close();
}

//********************************************************************  
void readFakeDataPars(std::string cardFile, std::vector<std::string>& name, std::map<std::string, double>& start){
//********************************************************************
  
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


//********************************************************************  
void readSamples(std::string cardFile, std::vector<std::string>& name, std::vector<bool>& fix, std::vector<double>& normVal){
//********************************************************************
  
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



