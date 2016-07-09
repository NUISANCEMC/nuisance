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

// Author: Callum Wilkinson    01/2014 

//         Patrick Stowell     09/2015
/**
  Usage: ./GSLminimizerWithReWeight.exe -c card file, where samples and parameters are defined
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

void readParameters(std::string cardFile, std::vector<std::string>& name, std::map<std::string, double>& start, 
		    std::map<std::string, double>& min, std::map<std::string, double>& max, std::map<std::string, bool>& fix);

void readFakeDataPars(std::string cardFile, std::vector<std::string>& name, std::map<std::string, double>& start);

void readSamples(std::string cardFile, std::vector<std::string>& name, std::vector<bool>& fix, std::vector<double>& normVal);

void saveOutput(ROOT::Math::Minimizer* minimizerObj, minimizerFCN *thisFCN, 
		std::vector<double>& valStart, std::vector<double>& valMax, std::vector<double>& valMin,
		std::vector<double>& valStep,  std::vector<double>& valFix);

bool checkResult(std::string inFile){
  
  TFile* in = new TFile(inFile.c_str(),"READ");
  TH1D* fitState = (TH1D*) in->Get("fit_state");
  
  bool isValidMinimum = bool(fitState->GetBinContent(1));
  
  in->Close();
};

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

  // Single list for saving parameters later
  std::vector<double> parStart, parMin, parMax, parStep, parFix;

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
 
  // Default minimizer is Minuit2.
  ROOT::Math::Minimizer* minimizerObj = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
    
  minimizerObj->SetMaxFunctionCalls(FitPar::Config().GetParI("MAX_CALLS"));
  minimizerObj->SetMaxIterations(FitPar::Config().GetParI("MAX_ITERATIONS"));
  minimizerObj->SetTolerance(0.001);

  ROOT::Math::Functor callFCN(*thisFCN,nfreepars);
  minimizerObj->SetFunction(callFCN);
 
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
    double vstep = 0.1;
    std::cout<<systString<<" "<<fixValues.at(systString)<<std::endl;
    if (vhigh == vlow) vhigh += 1.0;
    // Put in joint list
    parStart.push_back(vstart);
    parMax.push_back(vlow);
    parMin.push_back(vhigh);
    parStep.push_back(vstep);
    parFix.push_back( fixValues.at(systString) );

    // Setup variables
    // Note, starting value is set to 0 (as this is the NEUT output), and the error corresponds to the 1 sigma error in T2KReWeight
    minimizerObj->SetVariable(ipar, systString, vstart, vstep);
    minimizerObj->SetVariableLimits(ipar,vlow,vhigh);
    

    // Check if fixed parameter      
    if (fixValues.at(systString) == 1) {
  
      minimizerObj->FixVariable(ipar);
      std::cout << "Fixed: " << systString.c_str() << " at " << vstart << std::endl;
  
    } else {
      
      std::cout << "Free: " << systString.c_str() << " Initial: " << vstart << " Min: " << vlow << " Max: " << vhigh << std::endl;
    }
    ipar++;
  }

  // Now deal with the sterile mixing paramaters.                                                                                                                                                           
  if (FitPar::Config().GetParI("STERILE_FREE")>0.0){


    double sin_start = FitPar::Config().GetParD("STERILE_sin2theta");
    double sin_min = FitPar::Config().GetParD("STERILE_MIN_sin2theta");
    double sin_max = FitPar::Config().GetParD("STERILE_MAX_sin2theta");

    double dm2_start = FitPar::Config().GetParD("STERILE_delmass2");
    double dm2_min = FitPar::Config().GetParD("STERILE_MIN_delmass2");
    double dm2_max = FitPar::Config().GetParD("STERILE_MAX_delmass2");

    minimizerObj->SetVariable( ipar, "sin2theta", sin_start, 0.05);
    minimizerObj->SetVariableLimits(ipar,sin_min,sin_max);
    ipar++;

    minimizerObj->SetVariable( ipar, "delmass2", dm2_start, 0.05);
    minimizerObj->SetVariableLimits(ipar, dm2_min, dm2_max);
    ipar++;

  }


  // Now deal with the normalisation parameters
  for (int norm = 0; norm < int(nSampleNames.size()); ++norm){

    double vstep = 0.1;
    minimizerObj->SetVariable(ipar, (nSampleNames[norm]+"_norm"), nSampleVal[norm], vstep);
    minimizerObj->SetVariableLimits(ipar,0.3,1.7);

    std::cout<<"is fixed = "<<nSampleShape[norm]<<std::endl;
    // Fix any which shouldn't have a normalisation parameter
    if (!nSampleShape[norm]) minimizerObj->FixVariable(ipar);
      
    // Put in joint list
    parStart.push_back(nSampleVal[norm]);
    parMax.push_back(0.3);
    parMin.push_back(1.7);
    parStep.push_back(vstep);
    parFix.push_back(nSampleShape[norm]);
    
    ipar++;
  }

  std::cout << std::endl << "Number of Free Parameters = " << minimizerObj->NFree() << std::endl;
  std::cout << "Total number of Parameters = " << minimizerObj->NDim() << std::endl;

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
  std::cout<<" Saving Starting state." <<std::endl;

  // Initial reconfigure
  rw->Reconfigure();
  thisFCN->ReconfigureAllEvents();

  TDirectory* prefitDIR =(TDirectory*) outFile->mkdir("prefit");
  prefitDIR->cd();

  thisFCN->Write();

  outFile->cd();


  std::cout<<"---------------------------------------"<<std::endl;
  std::cout<<" Beginning fit routine." <<std::endl;

  // Minimal MINUIT output
  minimizerObj->SetPrintLevel(1);

  // Minimization routing
  minimizerObj->Minimize();

  std::cout<<"---------------------------------------"<<std::endl;
  std::cout<<" Found Minimium, Now Scanning between limits." <<std::endl;

  int NDIM = minimizerObj->NDim();
  unsigned int npoints = 50; // Hardcoded at 50, change this...
  TDirectory* scanDIR =(TDirectory*) outFile->mkdir("parscans");
  scanDIR->cd();

  // Save current minimum
  const double *values = minimizerObj->X();

  for (unsigned int i = 0; i < NDIM; i++){

    if (minimizerObj->IsFixedVariable(i)) continue;

    double* xvals = new double[npoints];
    double* yvals = new double[npoints];

    double low = parMin.at(i);
    double high = parMax.at(i);
    double start = parStart.at(i);
    double finish = values[i];

    minimizerObj->SetVariableStepSize(i, (high-low)/(npoints+0.));
    minimizerObj->SetVariableValue(i,start);
    
    minimizerObj->Scan(i, npoints, (&*xvals), (&*yvals));

    minimizerObj->SetVariableValue(i,finish);

    TGraph* tempGraph = new TGraph(npoints, xvals, yvals);
    std::string graphName = minimizerObj->VariableName(i);

    std::cout<<"Saving graph for "<<graphName<<std::endl;
    tempGraph->SetTitle(graphName.c_str());
    tempGraph->Write(graphName.c_str());
  }

  std::cout<<"---------------------------------------"<<std::endl;
  std::cout<<"Running Final Reconfigure "<<std::endl;
  // Final reconfigure considering background loops
  rw->Reconfigure();
  thisFCN->ReconfigureAllEvents();

  std::cout<<"---------------------------------------"<<std::endl;
  std::cout<<"Saving Final Output. "<<std::endl;
 
  TDirectory* postfitDIR =(TDirectory*) outFile->mkdir("postfit");
  postfitDIR->cd();
  thisFCN->Write();

  outFile->cd();
  saveOutput(minimizerObj,thisFCN, parStart, parMax, parMin, parStep, parFix);

  delete minimizerObj;
  //  delete thisFCN;  // deleted when minizer is deleted
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

  
  if (fitContinue and checkResult(InputFile)){
    std::cout<<"Fit has already converged! Good Job!"<<std::endl;
  } else {
    FitPar::Config().SetParI("FIX_INPUT_DIALS",0);
    int iret = testMinimize(cardFile, out, fitStrategy, fakeDataFile, type, InputFile);
    std::cout<<"Returned"<<std::endl;

    if (iret != 0) { 
      std::cerr << "ERROR: Minimize test failed !" << std::endl;
      return iret;
    }
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


// also put maps etc
void saveOutput(ROOT::Math::Minimizer* minimizerObj, minimizerFCN *thisFCN, 
		std::vector<double>& valStart, std::vector<double>& valMax, std::vector<double>& valMin,
		std::vector<double>& valStep,  std::vector<double>& valFix){

  // Save fit status quickly in a histogram
  // This is just for the fitter to read at later points, and is less code to just read from histogram.
  TH1D* fit_status = new TH1D("fit_status","fit_status",10,0,10);
  fit_status->GetXaxis()->SetBinLabel(1,"chi2");
  fit_status->SetBinContent(1,0.0);

  // chi2, NDOF, covariance status, maxiterations, maxfunctioncalls, iterations, functioncalls,
  fit_status->GetXaxis()->SetBinLabel(3,"cov_status");
  fit_status->SetBinContent(3,minimizerObj->CovMatrixStatus());

  fit_status->GetXaxis()->SetBinLabel(4,"maxiterations");
  fit_status->SetBinContent(4,minimizerObj->MaxIterations());

  fit_status->GetXaxis()->SetBinLabel(5,"maxfunctioncalls");
  fit_status->SetBinContent(5,minimizerObj->MaxFunctionCalls());

  fit_status->GetXaxis()->SetBinLabel(6,"iterations");
  fit_status->SetBinContent(6,minimizerObj->NIterations());

  fit_status->GetXaxis()->SetBinLabel(7,"calls");
  fit_status->SetBinContent(7,minimizerObj->NCalls());

  fit_status->GetXaxis()->SetBinLabel(8,"precision");
  fit_status->SetBinContent(8,minimizerObj->Precision());

  fit_status->GetXaxis()->SetBinLabel(9,"tolerance");
  fit_status->SetBinContent(9,minimizerObj->Tolerance());

  fit_status->GetXaxis()->SetBinLabel(10,"status");
  fit_status->SetBinContent(10,minimizerObj->Status());

  fit_status->Write();
  
  // This function saves all of the interesting output in the output ROOT file
  Int_t nPars = minimizerObj->NFree();
  // Create vectors to store useful information in
  std::vector<std::string> nameVect;
  std::vector<double>      valVect;
  std::vector<double>      errVect;
  std::vector<double>      minVect;
  std::vector<double>      maxVect;
  std::vector<double>      startVect;
  std::vector<double>      endfixVect;
  std::vector<double>      startfixVect;
  
  // A fixed length character array seems to be the only way to get the name from minuit. Disgusting stuff!
  double val, err, min, max;

  const double *values = minimizerObj->X();
  const double *errors = minimizerObj->Errors();
  std::string name = "";
  bool startfixed,endfixed;
    
  for (int npar = 0; npar < minimizerObj->NDim(); ++npar) {

    name = minimizerObj->VariableName(npar);
    endfixed = minimizerObj->IsFixedVariable(npar);

    nameVect.push_back(name);
    valVect .push_back(values[npar]);
    minVect .push_back(valMin[npar]);
    maxVect .push_back(valMax[npar]);
    startVect .push_back(valStart[npar]);
    startfixVect .push_back(valFix[npar]);

    if (endfixed){
      errVect.push_back(0);
      endfixVect.push_back(1);
    } else {
      errVect.push_back(errors[npar]);
      endfixVect.push_back(0);
    }
  }

  int NDOF = thisFCN->GetNDOF();
  int NPARS = minimizerObj->NDim();
  const int NFREE = minimizerObj->NFree();
  double chi2 = thisFCN->DoEval(values);

  std::cout << "Minimum chi^2 = " << chi2 << std::endl;
  std::cout << "NDOF = " << NDOF << " (# of free pars already subtracted)" << std::endl;

  int CovResult = minimizerObj->CovMatrixStatus();
  if (CovResult>0){
    
    // Have to fill a TH2D Awkwardly.                                                                                                                                                                    
    TH2D covarHist = TH2D("fit_covariance","fit_covariance; Pars;  Pars; Covariance",NPARS,0,NPARS,NPARS,0,NPARS);                                                                                       
    TH2D corrHist = TH2D("fit_correlations","fit_correlations; Pars;  Pars; Covariance",NPARS,0,NPARS,NPARS,0,NPARS);
       
    int xBin = 0;
    // Set labels and sort out correlation matrix
    for (int ipar = 0; ipar < NPARS; ++ipar){
      
      covarHist.GetXaxis()->SetBinLabel(xBin+1, nameVect[ipar].c_str());
      covarHist.GetYaxis()->SetBinLabel(xBin+1, nameVect[ipar].c_str());

      corrHist .GetXaxis()->SetBinLabel(xBin+1, nameVect[ipar].c_str());
      corrHist .GetYaxis()->SetBinLabel(xBin+1, nameVect[ipar].c_str());
      
      int yBin = 0;
      for (int jpar = 0; jpar < NPARS; ++jpar){

	double covval = minimizerObj->CovMatrix(ipar,jpar);

   	covarHist .SetBinContent(xBin+1, yBin+1, covval );
  	corrHist .SetBinContent(xBin+1, yBin+1, covval /(errVect[ipar]*errVect[jpar] +0.0));

   	++yBin;
      }
      ++xBin;
    }
    std::cout<<"Writing covariance matrix"<<std::endl;
    covarHist.Write("fit_covariances");
    corrHist .Write("fit_correlations");
  }

  //  Create a TTree and save the output of the fi
  TTree *tree = new TTree("fit_results", "fit_results");
  tree ->Branch("parameter_names",  &nameVect);

  tree ->Branch("parameter_start",  &startVect);
  tree ->Branch("parameter_values", &valVect);
  tree ->Branch("parameter_errors", &errVect);
  tree ->Branch("parameter_min",    &minVect);
  tree ->Branch("parameter_max",    &maxVect);
  tree ->Branch("parameter_fix",    &endfixVect);
  tree ->Branch("parameter_startfix", &startfixVect);
  tree ->Branch("chi2", &chi2, "chi2/D");
  tree ->Branch("NDOF", &NDOF, "NDOF/I");

  tree ->Fill();
  tree ->Write();
  
  // delete tree;
  // delete values;
  // delete errors;
  
  return;
}
