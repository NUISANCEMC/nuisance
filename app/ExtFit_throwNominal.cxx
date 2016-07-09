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

// Author:  Patrick Stowell     09/2015%    
/**
  Usage: ./ExtFit_throwNominal.exe -c card file, where samples and parameters are defined
         -o output file, where the results of the fit are stored
   where:   
*/

#include "TH1.h"
#include "TF1.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "Minuit2/FCNBase.h"
#include "Minuit2/Minuit2Minimizer.h"
#include "TFitterMinuit.h"
#include "TSystem.h"

#include "Math/IFunction.h"

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
#include "minimizerFCN.h"
#include "splinesFCN.h"
#include "FitParameters.h"

#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"

//********************************************************************  
void readParameters(std::string cardFile, std::vector<std::string>& name, std::map<std::string, double>& start, 
		    std::map<std::string, double>& min, std::map<std::string, double>& max, std::map<std::string, bool>& fix);

//********************************************************************  
void readFakeDataPars(std::string cardFile, std::vector<std::string>& name, std::map<std::string, double>& start);

//********************************************************************  
void readSamples(std::string cardFile, std::vector<std::string>& name, std::vector<bool>& fix, std::vector<double>& normVal);


//********************************************************************  
int testMinimize(std::string cardFile, TFile *outFile, std::string fitStrategy, std::string fakeDataFile, int type, std::string infile, int nthrows,bool asimov) { 
//********************************************************************
  
  std::vector<std::string>           params;
  std::map<std::string, double>      startVals, minValues, maxValues;
  std::map<std::string, bool>        fixValues;
  std::vector<std::string>           nSampleNames;
  std::vector<bool>                  nSampleShape;
  std::vector<double>                nSampleVal;

  std::vector<std::string>           nFakeParams;
  std::map<std::string,double>       nFakeStarts;

  // Single list for saving parameters later
  std::vector<double> parStart, parMin, parMax, parStep, parFix;
  std::vector<std::string> parNames;

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
      std::vector<double> *dialStart      ;// = fitpars.parameter_start;
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

  int nfreepars = 0;

  // Decide which parameters are included in T2KReWeight
  for(unsigned int i = 0; i < params.size(); ++i){
    rw->Systematics().Include(t2krew::T2KSyst::FromString(params[i])); 

    // Use tgraphs for rewighting by default           
    bool splinedial = false;
    if (FitPar::Config().GetParI(params[i]+"_SPLINE") == 1) splinedial = true;

    // Keep track of which dials are free.             
    // - This is needed when doing bin or event spline reweighting            
    if (fixValues.at(params[i])) FitPar::Variable().AddFixedDial(params[i],splinedial);
    else FitPar::Variable().AddFreeDial(params[i],splinedial);

    nfreepars++;
  }
  nfreepars += nSampleNames.size();
 
  std::cout<<"---------------------------------------"<<std::endl;
  std::cout<<"Creating sample instances"<<std::endl;

  minimizerFCN *thisFCN = new minimizerFCN(cardFile, fakeDataFile, outFile, rw, 1);

  std::cout<<"---------------------------------------"<<std::endl;
  std::cout<<" Saving NEUT Nominal (be cautious with this)." <<std::endl;

  // Initial reconfigure                
  rw->Reconfigure();
  thisFCN->ReconfigureAllEvents();

  TDirectory* nominalDIR =(TDirectory*) outFile->mkdir("nominalneut");
  nominalDIR->cd();

  thisFCN->Write();
  outFile->cd();

  std::cout<<"---------------------------------------"<<std::endl;

  // Fake data
  if (!fakeDataFile.empty() and fakeDataFile.compare("MC") != 0){

    std::cout<<"---------------------------------------"<<std::endl;
    std::cout<<"Creating fake data from external fit file shown in cardfile."<<std::endl;
    
    thisFCN->SetFakeData(fakeDataFile);
    
  }

  thisFCN->SetType(type);

  std::cout<<"---------------------------------------"<<std::endl;
  std::cout<<"Setting up Minuit."<<std::endl;

  gSystem->Load("libMinuit2");

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
    double vstep = 0.1;
    std::cout<<systString<<" "<<fixValues.at(systString)<<std::endl;
    if (vhigh == vlow) vhigh += 1.0;
    // Put in joint list           
                   
    // Note, starting value is set to 0 (as this is the NEUT output), and the error corresponds to the 1 sigma error in T2KReWeight       
    parStart.push_back(vstart);
    parMin.push_back(vlow);
    parMax.push_back(vhigh);
    parStep.push_back(0.01);
    parNames.push_back(systString);

    // Check if fixed parameter       
    if (fixValues.at(systString) == 1) {

      //      minimizerObj->FixVariable(ipar);
      std::cout << "Fixed: " << systString.c_str() << " at " << vstart << std::endl;
      parFix.push_back(1);

    } else {
      parFix.push_back(0);
      std::cout << "Free: " << systString.c_str() << " Initial: " << vstart << " Min: " << vlow << " Max: " << vhigh << std::endl;
    }
    ipar++;
  }
                   

  // Now deal with the normalisation parameters      
  for (int norm = 0; norm < int(nSampleNames.size()); ++norm){

    double vstep = 0.1;
    parStart.push_back(nSampleVal[norm]);
    parMin.push_back(0.3);
    parMax.push_back(1.7);
    parStep.push_back(0.01);
    parNames.push_back((nSampleNames[norm]+"_norm"));
    
    parFix.push_back(!nSampleShape[norm]);
    ipar++;
  }


  // Set Fake data to MC if we want
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

    for (int i = 0; i < nSampleNames.size(); i++){
      nFakeStarts[nSampleNames[i]+"_norm"] = (nSampleVal[i]);
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
  std::cout<<" Saving Starting state." <<std::endl;

  // Initial reconfigure
  rw->Reconfigure();
  thisFCN->ReconfigureAllEvents();

  TDirectory* prefitDIR =(TDirectory*) outFile->mkdir("prefit");
  prefitDIR->cd();

  thisFCN->Write();

  outFile->cd();

  std::cout<<"---------------------------------------"<<std::endl;
  std::cout<<" Setting up throw tree." <<std::endl;

  // Branches for parameters 
  double chi2;
  int ndof;

  std::vector<double> truevalues;
  std::vector<double> fitvalues;
  std::vector<double> fiterrors;
  std::vector<double> fitresiduals;

  TH1D*  bestFit_MC = new TH1D();
  TH1D*  bestFit_DT = new TH1D();
  TTree* FakeTree = new TTree("FakeTree","FakeTree");

  FakeTree->Branch("chi2",&chi2,"chi2/D");
  FakeTree->Branch("ndof",&ndof,"ndof/I");
  FakeTree->Branch("truevalues",&truevalues);
  FakeTree->Branch("fitvalues",&fitvalues);
  FakeTree->Branch("fiterrors",&fiterrors);
  FakeTree->Branch("fitresiduals",&fitresiduals);

  std::cout<<"---------------------------------------"<<std::endl;
  std::cout<<" Set up throw tree." <<std::endl;
  

  if (asimov) nthrows = 1;
  // Keep track of each parameters starting values                   
  std::vector<double> truePars;
  std::vector<int> fixPars;
  std::map<std::string,double> trueFitValues;
  bool throwstart = FitPar::Config().GetParI("throw_mcstart");
  bool uniformstart = FitPar::Config().GetParI("uniform_mcstart");
  int uniform_nvals = FitPar::Config().GetParI("uniform_mcstart_nvals");
  int uniform_count = 0;
  bool throwpars = FitPar::Config().GetParI("throw_fakestart");
  bool uniformpars = FitPar::Config().GetParI("uniform_fakestart");
  
  ROOT::Math::Minimizer* minimizerObj = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");  
  ROOT::Math::Functor callFCN(*thisFCN,nfreepars);
  minimizerObj->SetFunction(callFCN);        
  bool minimized=false;
  // Run throw loop
  for (int ithrow = 0; ithrow < nthrows; ithrow++){
   
    // Reset vectors
    minimizerObj->Clear();
    fitvalues.clear();
    fiterrors.clear();
    fitresiduals.clear();
    truevalues.clear();
    
    if (uniformstart)
      if (ithrow % int((nthrows+1.0+0.)/((uniform_nvals+2.0)+0.)) == 0) uniform_count++;


    // Set the minimizer variables
    for (int ipar = 0; ipar < parNames.size(); ipar++){

      // Throw start values
      if (throwstart) parStart[ipar] = gRandom->Uniform(parMin[ipar],parMax[ipar]);
      if (uniformstart and !parFix[ipar]) parStart[ipar] = ((uniform_count+0.)/(uniform_nvals+2.0+0.))*(parMax[ipar]-parMin[ipar]) + parMin[ipar];

      // Set the variables
      minimizerObj->SetVariable(ipar, parNames[ipar], parStart[ipar], parStep[ipar]);
      minimizerObj->SetVariableLimits(ipar,parMin[ipar],parMax[ipar]);

      if (parFix[ipar]) minimizerObj->FixVariable(ipar);
    }


    if (!minimized){
    //Set fake data using uniform start value
    // if (throwpars or uniformpars){
    for(int i = 0; i < nFakeParams.size(); i++){

      std::string systName = nFakeParams.at(i);
      
      // Throw the parameters
      if (throwpars)  nFakeStarts.at(systName) = gRandom->Uniform(parMin[i],parMax[i]);
      if (uniformpars) nFakeStarts.at(systName) = ((uniform_count+0.)/(uniform_nvals+0.))*(parMax[i]-parMin[i]) + parMin[i];
      
      // rw systematics
      rw->Systematics().Include(t2krew::T2KSyst::FromString(systName));
      rw->Systematics().SetTwkDial(t2krew::T2KSyst::FromString(systName), nFakeStarts.at(systName));
    }
    
    // Don't throw sample norm
    for (int i = 0; i < nSampleNames.size(); i++){
      nFakeStarts[nSampleNames[i]+"_norm"] = (nSampleVal[i]);
    }
    
    // Reset the rw libraries
    rw->Reconfigure();
    thisFCN->ReconfigureAllEvents();
    thisFCN->SetFakeData("MC");
    
    for(int i = 0; i < nFakeParams.size(); i++){
      rw->Systematics().ResetTwkDial(t2krew::T2KSyst::FromString(nFakeParams.at(i)));
    }
    //      std::cout<<"Throwing data "<<std::endl;
    }

    // Throw dataset with stats
    if(!asimov) thisFCN->ThrowSamples();
    
    // Run the fit
    minimizerObj->SetTolerance(0.0001);
    minimizerObj->Minimize();

    // Get fit results
    chi2 = thisFCN->GetCurrentChi2();

    // Fill true values etc
    const double *values = minimizerObj->X();
    const double *errors = minimizerObj->Errors();

    int idial = 0;
    for (int idial = 0; idial < minimizerObj->NDim();idial++){
      
      // Individual dial variation
      std::string dialName  = (minimizerObj->VariableName(idial));
      if (parFix[idial]) continue;
      //      std::cout<<dialName;

      // Save true datavalue
      double val = parStart[idial];
      
      truevalues.push_back(val);
      fitvalues.push_back(values[idial]);
      fiterrors.push_back(errors[idial]);
      std::cout<<"Fit residual = "<<(val-values[idial])/errors[idial]<<std::endl;
      fitresiduals.push_back((val-values[idial])/errors[idial]);


    }
    minimized=true;
    // Fill the results from the fake data studies
    FakeTree->Fill();
  }

  TDirectory* postfitDIR =(TDirectory*) outFile->mkdir("postfit");
  postfitDIR->cd();

  thisFCN->Write();

  outFile->cd();
  
  std::cout<<"---------------------------------------"<<std::endl;
  std::cout<<"Saving Final Output. "<<std::endl;  
  FakeTree->Write();
  std::cout<<"Saved Tree."<<std::endl;
  //  delete minimizerObj;
  //  delete thisFCN;  // deleted when minizer is deleted
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
  int nthrows = 500;
  bool asimov = false;
  
  int type = 0;
  bool fitContinue = false;
  std::cout<<"Arguments:"<<std::endl;
  for (int i = 1; i< argc; ++i){
    if (i+1 != argc){

      // Cardfile
      std::cout<<argv[i]<<" "<<argv[i+1]<<std::endl;
      if (!std::strcmp(argv[i], "-c")) {cardFile=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-o")) {outFile=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-f")) {fitStrategy=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-d")) {fakeDataFile=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-t")) {type=atoi(argv[i+1]); ++i;}
      else if (!std::strcmp(argv[i], "-p")) { ParsFileOverride=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-q")) { ParsOverrides.push_back(argv[i+1]); ++i;}
      else if (!std::strcmp(argv[i], "-i")) { InputFile=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-n")) {nthrows=atoi(argv[i+1]); ++i;}
      else if (!std::strcmp(argv[i], "-a")) {asimov=bool(atoi(argv[i+1])); ++i;}
      } else if (!std::strcmp(argv[i], "-x")) { 
	InputFile=argv[i+1]; 
	++i;
	fitContinue = true;
    } else std::cerr << "ERROR: unknown command line option given! - '" <<argv[i]<<" "<<argv[i+1]<<"'"<< std::endl;
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
  int iret = testMinimize(cardFile, out, fitStrategy, fakeDataFile, type, InputFile,nthrows,asimov);
  
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
		    std::map<std::string, double>& min, std::map<std::string, double>& max, std::map<std::string, bool>& fix){
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

