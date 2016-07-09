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

// Author: Patrick Stowell 22/09/15
/**
   Usage: ./genBinWeights.exe -c card file, where samples and parameters are defined
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
#include "splinesFCN.h"
#include "FitParameters.h"


bool savehists=true;

//********************************************************************  
void readParameters(std::string cardFile, std::vector<std::string>& name, std::map<std::string, double>& start, 
		    std::map<std::string, double>& min, std::map<std::string, double>& max, std::map<std::string, int>& knot, std::map<std::string, bool>& fix,std::map<std::string, bool> spline);

//********************************************************************  
void readSamples(std::string cardFile, std::vector<std::string>& name);

// Main Minimizer Function
//******************************************************************** 
int genBinWeights(std::string cardFile, TFile *outFile, std::string fitStrategy, std::string fakeDataFile, int type, std::string infile) { 
//******************************************************************** 

  // Value inputs
  std::vector<std::string>           params;
  std::map<std::string, double>      startVals, minValues, maxValues;
  std::map<std::string, int>         knotValues;
  std::map<std::string, bool>        fixValues;
  std::vector<std::string>           nSampleNames;
  std::map<std::string, bool>        splineValues;

  // Read the card file for this fit
  std::cout<<"Reading Card Files"<<std::endl;
  
  readParameters(cardFile, params, startVals, minValues, maxValues, knotValues, fixValues,splineValues);
  readSamples(cardFile,nSampleNames);

  std::cout<<"---------------------------------------"<<std::endl;
  std::cout<<"Setting up generator libraries"<<std::endl;

  //Load NUWRO Libraries if required
#ifdef __NUWRO_ENABLED__
  gSystem->Load("event1.so");
#endif

  // Load T2K Reweight libraries
  t2krew::T2KReWeight *rw          = new t2krew::T2KReWeight();
  t2krew::T2KNeutReWeight *neut_rw = new t2krew::T2KNeutReWeight();
  t2krew::T2KNIWGReWeight *niwg_rw = new t2krew::T2KNIWGReWeight();

  // Assign weight Engines
  rw->AdoptWghtEngine("neut_rw", neut_rw);
  rw->AdoptWghtEngine("niwg_rw", niwg_rw);

  // Include T2K ReWeight Systematics and set start nominal values
  std::cout<<"Nominal Dial Values"<<std::endl;

  // Loop params
  for(unsigned int i = 0; i < params.size(); ++i){

    // Make T2KReWeight Include the Dials
    rw->Systematics().Include(t2krew::T2KSyst::FromString(params[i])); 
    
    // Set dial default
    rw->Systematics().SetTwkDial(t2krew::T2KSyst::FromString(params[i]),startVals[params[i]]);

    // Use tgraphs for rewighting by default                                                                                                                                                          
    bool splinedial = false;
    if (FitPar::Config().GetParI(params[i]+"_SPLINE") == 1) splinedial = true; 

    if (fixValues[params[i]]) FitPar::Variable().AddFixedDial(params[i],splinedial);
    else FitPar::Variable().AddFreeDial(params[i],splinedial);

    // Print Dial State
    std::cout<<i<<". "<<params[i]<<" start value is "<<startVals[params[i]]<<", and is ";
    if (fixValues[params[i]]) std::cout<<"FIXED."<<std::endl;
    else std::cout<<"FREE."<<std::endl;

  }
  // Run an initial Reconfigure
  rw->Reconfigure();
 
  std::cout<<"---------------------------------------"<<std::endl;
  std::cout<<"Creating sample instances"<<std::endl;

  // Generate a function that hanldes spline generation
  splinesFCN *splineFCN = new splinesFCN(cardFile, outFile, rw, 1);
  
  // Create sample folders in outfile
  std::vector<std::string> outNames;
  splineFCN->GetMeasurementNames(outNames);

  // Make save directory for the splines
  outFile->cd();
  for (int i = 0; i < outNames.size(); i++) 
    TDirectory* newdir = (TDirectory*) outFile->mkdir((outNames[i]+"_binsplines").c_str());
  

  std::cout<<"---------------------------------------"<<std::endl;
  std::cout<<"Starting looping free parameters"<<std::endl;
 
  // Generate the TGraphs for each experiment.                                                                                                                                    
  std::map<std::string, std::vector<TGraph> > expGraphs;
  std::map<std::string, std::vector<double> > expBins;

  // Add in the nominal result.                                                                                                                                                       
  std::vector<std::string> newNames;
  splineFCN->GetMeasurementNames(newNames);

  // Make sample Nominal place holder
  for(std::vector<std::string>::iterator samiter = newNames.begin(); samiter != newNames.end(); samiter++){
    std::string sample = *samiter;

    expBins[sample+"NOMINAL"];

  }
  // Generate Weight set fills the map with vectors of bin contents
  splineFCN->GenerateWeightSet(expBins,"NOMINAL");
  

  // Start of actual dial loops
  int idial = 0;
  for (std::vector<std::string>::iterator sysiter = params.begin(); sysiter != params.end(); sysiter++){

    std::string dialname = *sysiter;
    idial++;

    std::cout<<idial<<". "<<dialname<<std::endl;

    if (fixValues[dialname]){
      std::cout<<dialname<<" dial is fixed at "<<startVals[dialname]<<std::endl;
      std::cout<<" - No splines generated."<<std::endl;	
    } else {
      std::cout<<dialname<<" dial is free. Generating splines with following properties."<<std::endl;
      double nomSpline = startVals[dialname];
      double minSpline = minValues[dialname];
      double maxSpline = maxValues[dialname];
      double startSpline = maxValues[dialname];
      double knotSpline  = knotValues[dialname];
      double densSpline  = (maxSpline - minSpline)/knotSpline;
      std::cout<<" - NKnots = "<<knotSpline<<std::endl;
      std::cout<<" - Nominal = "<<startSpline<<std::endl;
      std::cout<<" - Lower Limit = "<<minSpline<<std::endl;
      std::cout<<" - Upper Limit = "<<maxSpline<<std::endl;
      std::cout<<" - Dial Density = "<<densSpline <<" sigma/knot"<<std::endl;

      for(std::vector<std::string>::iterator samiter = newNames.begin(); samiter != newNames.end(); samiter++){
	std::string sample = *samiter;
	std::cout<<"Pushing back graph for "<<sample<<std::endl;
	expGraphs[sample+dialname];
	expBins[sample+dialname];

	expGraphs[sample+dialname+"_err"];
	expBins[sample+dialname+"_err"];
	
      }

      TDirectory* histdir;
      if (savehists){
	//	Make a folder to save a load of actual histograms for this dial
	histdir = (TDirectory*) outFile->mkdir(("mcHist_"+dialname).c_str());
	histdir->cd();
      }

      // Start loop ------------------------------
      double valSpline;
      for (int j = 0; j < knotSpline+1; j++){

	// Calculate sigma value
	valSpline = minSpline + densSpline*(j+0.);
	
	// Reconfigure reweight;
	rw->Systematics().SetTwkDial(t2krew::T2KSyst::FromString(dialname),valSpline);
	rw->Reconfigure();

	// Generate Event Weights
	splineFCN->GenerateWeightSet(expBins,dialname);

	// Fill points in the TGraphs
	for(std::vector<std::string>::iterator samiter = newNames.begin(); samiter != newNames.end(); samiter++){
	  std::string sample = *samiter;

	  // Get the vector of TGraphs
	  std::vector<TGraph>* sampleWeights = &(expGraphs.at(sample+dialname));
	  std::vector<TGraph>* sampleWeights_err = &(expGraphs.at(sample+dialname+"_err"));
	  
	  std::vector<double>  values = expBins.at(sample+dialname);
	  std::vector<double>  errors = expBins.at(sample+dialname+"_err");
	  std::vector<double> nominalValues = expBins.at(sample+"NOMINAL");

	  std::cout<<"Adding TGraphs"<<std::endl;
	  // Add TGraphs if required
	  if (sampleWeights->size() == 0){
	    for (int ibin = 0; ibin < values.size(); ibin++){
	      TGraph temp = TGraph();
	      TGraph temp_err = TGraph();
	      
	      temp.SetTitle(Form((dialname+" Bin %i Content;Dial Value (1#sigma);Bin Content").c_str(),ibin));
	      temp_err.SetTitle(Form((dialname+" Bin %i Error;Dial Value (1#sigma);Bin Content").c_str(),ibin));
	      sampleWeights->push_back(temp);
	      sampleWeights_err->push_back(temp);
	    }
	  }

	  // Fill Weights into TGraphs
	  for (int ibin = 0; ibin < values.size(); ibin++){
	    (sampleWeights->at(ibin)).SetPoint((sampleWeights->at(ibin)).GetN(),valSpline,values.at(ibin)/nominalValues.at(ibin));
	    (sampleWeights_err->at(ibin)).SetPoint((sampleWeights_err->at(ibin)).GetN(),valSpline,errors.at(ibin)/nominalValues.at(ibin));
	  }
	  
	  // Save hists for that dial
	  if (savehists){
	    histdir->cd();
	    std::ostringstream ss;
	    ss << std::setprecision(3) << valSpline;
	    splineFCN->WriteMC(dialname + "_"+ss.str());
	  }	  
	}

	// Reset Dial to Nominal                                                                                        
	rw->Systematics().SetTwkDial(t2krew::T2KSyst::FromString(dialname),nomSpline);
	rw->Reconfigure();

	std::cout<<"End of a weighting loop"<<std::endl;
      } // End Loop ---------


      // Save to file
      std::cout<<"saving individual graphs"<<std::endl;
      splineFCN->GetMeasurementNames(newNames);

      for(std::vector<std::string>::iterator samiter = newNames.begin(); samiter != newNames.end(); samiter++){
	std::string sample = *samiter;

	// Get correct save directory
	TDirectory* tempDir = (TDirectory*) outFile->Get((sample+"_binsplines").c_str());
	tempDir->cd();

	// Save either a TGraph
	TGraph* savePoint = new TGraph();
	TGraph* errorPoint = new TGraph();
	
	// Make a directory to save TGraphs
	TDirectory* graphDir = (TDirectory*) tempDir->mkdir((dialname+"_graphs").c_str());
        graphDir->cd();

	// Save all graphs
	for (int l = 0; l < expGraphs[sample+dialname].size(); l++){

	  // Get specific sample graph
	  savePoint = &(expGraphs[sample+dialname].at(l));
	  errorPoint = &(expGraphs[sample+dialname+"_err"].at(l));
	  
	  // Save TGraph out of tree for easy viewing (will double file size...)
	  std::stringstream ss;
	  ss << l;

	  std::cout<<"Saving "<<(dialname+"_histbin_"+ss.str())<<std::endl;
	  savePoint->Write((dialname+"_histbin_"+ss.str()).c_str());
	  errorPoint->Write((dialname+"_errorbin_"+ss.str()).c_str());
	}
      }
    } // --- Done with free dial
    std::cout<<std::endl;
  }


  // --- Now save all graphs into a single tree for that sample;
  std::cout<<"Saving individual sample TTress."<<std::endl;
  // Loop through samples
  for(std::vector<std::string>::iterator samiter = newNames.begin(); samiter != newNames.end(); samiter++){
    std::string sample = *samiter;

    //Setup Vectors of TGraphs and TSpline3
    std::vector<TGraph*> saveGraphs;
    std::vector<TSpline3*> saveSplines;
    std::vector<int> flagSplines;

    // Graph or TSpline3 Counters
    int igraph = 0;
    int ispline = 0;
    std::string dialname;

    // Get correct save directory                                                                                                                                                                           
    TDirectory* tempDir = (TDirectory*) outFile->Get((sample+"_binsplines").c_str());
    tempDir->cd();


    int nbins = 0;

    // Loop through dials and setup the TTree
    for (std::vector<std::string>::iterator sysiter = params.begin(); sysiter != params.end(); sysiter++){

      dialname = *sysiter;
      
      if (fixValues[dialname]) continue;
      TTree* dialTree = new TTree((dialname).c_str(),(dialname).c_str());
      int splineflag = 0;
      double binNominal = 0.0;

      dialTree->Branch("spline",&splineflag,"spline/I");
      dialTree->Branch("nominal",&binNominal,"nominal/D");

      TGraph* fillgraph     = new TGraph();
      TGraph* fillgraph_err = new TGraph();
      
      dialTree->Branch(dialname.c_str(),"TGraph",&fillgraph,256000,1);
      dialTree->Branch((dialname+"_err").c_str(),"TGraph",&fillgraph_err,256000,1);
      
      std::cout<<"getting nbins"<<std::endl;
      nbins = (expGraphs[sample+dialname].size());

      // Start filling bins into the TTree                                                                                                                                     
      for (int l = 0; l < nbins; l++){

	// reset counters              
	igraph = 0;
	ispline = 0;

	// Fill nominal xsec           
	binNominal = expBins.at(sample+"NOMINAL").at(l);
	(fillgraph) = (TGraph*)(&(expGraphs[sample+dialname].at(l)));
	(fillgraph_err) = (TGraph*)(&(expGraphs[sample+dialname+"_err"].at(l)));
	dialTree->Fill();
      }
      dialTree->Write();
    }
  }

  
  delete splineFCN;
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

int iret = genBinWeights(cardFile, out, fitStrategy, fakeDataFile, type, InputFile);
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
		    std::map<std::string, double>& min, std::map<std::string, double>& max, std::map<std::string, int>& knot, std::map<std::string, bool>& fix, std::map<std::string, bool> spline){
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
      } else if (val == 5) {
	stoken >> knotVal;
	knot    .insert( map<std::string, int>::value_type(thisName, knotVal) );
      } else if (val == 6){
	if (token.compare("FIX") == 0) {
	  fix  .insert( map<std::string, bool>::value_type(thisName, 1) );
	} else fix .insert( map<std::string, bool>::value_type(thisName, 0) );
      } else if (val == 7){
	if (token.compare("TGRAPH") == 0) {
          spline  .insert( map<std::string, bool>::value_type(thisName, 1) );
        } else spline .insert( map<std::string, bool>::value_type(thisName, 0) );
      
      }else break;

      val++;
    }
  }
  card.close();
}

//******************************************************************** 
void readSamples(std::string cardFile, std::vector<std::string>& name){
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
      } else break;

      val++;
    }
  }
  card.close();
}

