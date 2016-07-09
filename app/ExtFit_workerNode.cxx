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
/*
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

#ifdef __GENIE_ENABLED__
#include "T2KGenieReWeight.h"
#endif

#include "FitEvent.h"
#include "nodeFCN.h"
#include "FitParameters.h"

#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include <dirent.h>
#include <sys/types.h>


// read parameters and starting values
void readParameters(std::string cardFile, std::vector<std::string>& names, std::vector<double>& parvals);

// read samples and starting norms
void readSamples(std::string cardFile, std::vector<std::string>&samples){

  std::string line;
  std::ifstream card(cardFile.c_str(), ifstream::in);

  while(std::getline(card, line, '\n')){
    std::istringstream stream(line);

    std::string token;
    int val = 0;

    // check the type
    while(std::getline(stream, token, ' ')){

      // Strip any leading whitespace from the stream  and ignore comments
      stream >> std::ws;
      if (token.c_str()[0] == '#') continue;

      // setup string stream
      std::istringstream stoken(token);

      // Read in values
      if (val == 0){
	if (token.compare("sample") != 0 ) { break; }
      } else if (val == 1) samples.push_back(token);

      val++;
    }
  }
  card.close();
  return;
}

// Get card name with this uid, and strip .card from it
std::string GetNextCard(std::string id, int iter){

  std::ostringstream ss;
  ss << iter;
  std::string cardname = id + "_" + ss.str();

  if ( access( (cardname + ".card").c_str(), F_OK ) != -1 ){
    return cardname;
  } else {
    return "";
  }

};


int CreateMCResult(std::string cardFile, nodeFCN* MCSamples){

  // Move the card to .processed
  system(("mv " + cardFile + ".card " + cardFile + ".processing").c_str());

  // read line from the card.
  std::string parstring;
  std::string line;
  std::ifstream card((cardFile+".processing").c_str(), ifstream::in);
  while(std::getline(card,line,'\n')){
    parstring = line;
    break;
  }

  if (parstring.find("KILL") != std::string::npos){
    std::cout<<"Node being ended."<<std::endl;
    system(("rm " + cardFile + ".processing").c_str());
    return -1;
  }

  std::cout<<" par string = "<<parstring<<std::endl;

  // Create an output root file
  TFile* saveFile = new TFile((cardFile + ".root").c_str(),"RECREATE");
  // Read in the parameters into MCSamples
  saveFile->cd();
  MCSamples->SetOutputFile(saveFile);
  MCSamples->GenerateMC(parstring);

  // Write to file.
  saveFile->cd();
  MCSamples->Write();

  saveFile->Close();
  delete saveFile;

  // Head back to main function
  return 0;

};

void readParameters(std::string cardFile, bool fake, std::vector<std::string>& names, std::vector<double>& parvals){
  //
    std::string line;
    std::ifstream card(cardFile.c_str(), ifstream::in);

    while(std::getline(card, line, '\n')){
      std::istringstream stream(line);

      std::string token;
      int val = 0;
      std::string thisType;
      std::string thisName;
      double minVal   = 0;
      double maxVal   = 0;
      double startVal = 0;
      bool fixVal = true;

      // check the type
      while(std::getline(stream, token, ' ')){

	// Strip any leading whitespace from the stream  and ignore comments
	stream >> std::ws;
	if (token.c_str()[0] == '#') continue;

	// setup string stream
	std::istringstream stoken(token);

	// Read in values
	if (val == 0)
	  if (token.find("parameter") != std::string::npos ) {  thisType = token;  }
	  else {  break;   }
	else if (val == 1)           {   thisName = token;   }
	else if (val == 2)           {   stoken >> startVal; }
	else { break; }
	val++;
      }
      if (val > 2) {

	bool isfakepar = (!thisType.compare("fake_parameter"));

	// Set the true starting values
	if (!fake and !isfakepar){


	  // read in parameter if all found
	  names .push_back( thisName );
	  parvals  .push_back( startVal );

	  // in fake parameters
	} else if (fake and isfakepar) {

	  // set the fake parameters
	  names  .push_back( thisName );
	  parvals   .push_back( startVal );

	} else {
	  break;
	}
      }
    }
    card.close();
};



bool CheckMCResult(std::string cardid){

  bool status = true;
  TFile* infile = new TFile((cardid + ".root").c_str(), "READ");
  if ((infile->GetListOfKeys()->GetEntries()) < 2) status = false;
  if (infile->IsZombie()) status = false;
  infile->Close();

  return status;
};


int main(int argc, char* argv[]){

  std::string cardFile;
  std::string outFile;
  std::string fakeDataFile;
  std::string ParsFileOverride;
  std::vector<std::string> ParsOverrides;
  std::string InputFile;
  std::string cardfile;

  int start = 1;
  std::string parameters;
  std::string values;
  std::string infile;
  std::string outfile;
  std::string limits;
  int type = 0;
  bool quietMode = false;
  std::string jobid;
  std::vector<std::string> parOverrides;

  // Parse Arguments
  for (int i = 1; i< argc; ++i){
    if (i+1 != argc){

      // Cardfile.
      std::cout<<argv[i]<<" "<<argv[i+1]<<std::endl;
      if      (!std::strcmp(argv[i], "-p")) {parameters=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-uid")) {jobid=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-v")) {values=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-f")) {infile=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-l")) {limits=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-i")) {InputFile=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-t")) {type=atoi(argv[i+1]); ++i;}
      else if (!std::strcmp(argv[i], "-basecard")) {cardfile=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-o")) {outfile=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-s")) {quietMode=bool(argv[i+1]); ++i;}
      else if (!std::strcmp(argv[i], "-q")) {parOverrides.push_back(argv[i+1]); ++i;}
      else if (!std::strcmp(argv[i], "-start")) {start=atoi(argv[i+1]); ++i;}
      else std::cerr << "ERROR: unknown command line option given! - '" <<argv[i]<<" "<<argv[i+1]<<"'"<< std::endl;
    }
  }

  // Quiet Mode Enabled
  // Set override parameters
  std::string defaultFile = std::string(std::getenv("EXT_FIT"))+"/parameters/fitter.requirements.dat";
  FitPar::Config().ReadParamFile(defaultFile);
  FitPar::Config().ReadParamFile(parameters);
  for (int i = 0; i < parOverrides.size(); i++){
    FitPar::Config().ForceParam(parOverrides[i]);
  }
  int thisverb = FitPar::Config().GetParI("VERBOSITY");

  // Analysis loop should go like this

  // Get Unique id
  // read in the base cards
  // setup the parameters and classes.
  // start a while loop
  // -- While running time < 24 hours
  // -- Look for the oldest unprocessed cards
  // -- Pass to a  function.
  // -- -- Make processing flag by moving the card
  // -- -- Parse parameters from file
  // -- -- Reconfigure All Events In Series
  // -- -- Write just MCHist to file.
  // -- Check root file is decent.
  // -- Show .complete or .failed relevant.
  // -- Wait for 20 seconds.
  // -- Run processing wasteage.


  // Open the output file
  TFile* outRootFile = new TFile((jobid + "tempout.root").c_str(),"RECREATE");

  std::vector<std::string> fit_params;
  std::vector<double>      fit_vals;
  std::vector<std::string> samples;

  // setup the reweighting libraries
  readParameters(cardfile, false, fit_params, fit_vals);
  readSamples(cardfile, samples);

  FitWeight* rw =  new FitWeight();

  for(unsigned int i = 0; i < fit_params.size(); ++i){
    rw->IncludeDial(fit_params[i]);
    rw->SetDialValue(fit_params[i], fit_vals[i]);
  }

  for (unsigned int i = 0; i < samples.size(); ++i){
    std::cout<<"Adding sample norm : "<<samples[i] + "_norm"<<std::endl;
    rw->IncludeSampleNorm(samples[i] + "_norm");
  }

  rw->Reconfigure();

  // Create the samples
  nodeFCN *thisFCN = new nodeFCN(cardfile, outRootFile, rw, 1);

  int curiter = start;
  // Now start the input loop
  while(true){

    // Get Card
    std::string cardid = GetNextCard(jobid,curiter);
    int timedif = time(NULL);

    // If cardid is empty wait for 30 seconds and look again
    if (cardid.empty()){
      FitPar::Config().SetParI("VERBOSITY",0);
      FitPar::log_verb=0;
      while((time(NULL) - timedif) < 1){
	double val  = thisFCN->GetCurrentChi2();
      }
      FitPar::Config().SetParI("VERBOSITY",thisverb);
      FitPar::log_verb=thisverb;
      continue;
    }

    // Create and save the MC result
    int status = CreateMCResult(cardid, thisFCN);

    // end loop if status asks
    if (status == -1) break;

    // Check the result is a useful file
    std::string resultflag;

    if (CheckMCResult(cardid)) {
      resultflag = ".complete";
      curiter++;
    }
    else resultflag = ".card";

    // Move it back and let it get picked up again if it failed
    system(("mv " + cardid + ".processing " + cardid + resultflag).c_str());

  }

  // ending job
  std::cout<<"Ending worker node."<<std::endl;

  // Delete everything
  delete thisFCN;

  std::cout<<"Finished output."<<std::endl;
  return 0;
}



