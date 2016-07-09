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

//#ifdef __GENIE_ENABLED__
//#include "T2KGenieReWeight.h"
//#endif

#include "FitEvent.h"
#include "minimizerFCN.h"
#include "splinesFCN.h"
#include "FitParameters.h"

#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"

//
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






int main(int argc, char* argv[]){

  std::string cardFile;
  std::string outFile;
  std::string fakeDataFile;
  std::string ParsFileOverride;
  std::vector<std::string> ParsOverrides;
  std::string InputFile;
  std::string cardfile;

  std::string parameters;
  std::string values;
  std::string infile;
  std::string outfile;
  std::string limits;
  int type = 0;
  bool quietMode = false;


  for (int i = 1; i< argc; ++i){
    if (i+1 != argc){

      // Cardfile.
      std::cout<<argv[i]<<" "<<argv[i+1]<<std::endl;
      if      (!std::strcmp(argv[i], "-p")) {parameters=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-v")) {values=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-f")) {infile=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-l")) {limits=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-i")) {InputFile=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-t")) {type=atoi(argv[i+1]); ++i;}
      else if (!std::strcmp(argv[i], "-c")) {cardfile=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-o")) {outfile=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-s")) {quietMode=bool(argv[i+1]); ++i;}
      else if (!std::strcmp(argv[i], "-q")) { ParsOverrides.push_back(argv[i+1]); ++i;}
      else std::cerr << "ERROR: unknown command line option given! - '" <<argv[i]<<" "<<argv[i+1]<<"'"<< std::endl;
    }
  }

  if (quietMode){
    std::cout.rdbuf( NULL );
    std::cerr.rdbuf( NULL );
  }

  // Set override parameters
  std::string defaultFile = std::string(std::getenv("EXT_FIT"))+"/parameters/fitter.requirements.dat";
  FitPar::Config().ReadParamFile(defaultFile);
  FitPar::Config().ReadParamFile(cardfile);

  if (!ParsOverrides.empty()){
    for (unsigned int iter = 0; iter < ParsOverrides.size(); iter++){
      FitPar::Config().ForceParam(ParsOverrides[iter]);
    }
  }

  FitPar::Config().ForceParam("SERIAL_EVAL=1");


  // Open the output file
  TFile* outRootFile = new TFile(outfile.c_str(),"RECREATE");

  // Setup variables
  double weight;
  int    index;

  // setup the reweighting libraries

  std::vector<std::string> fit_params;
  std::vector<double> fit_vals;
  readParameters(cardfile, false, fit_params, fit_vals);
  readParameters(cardfile, true,  fit_params, fit_vals);

  FitWeight* rw;

  for(unsigned int i = 0; i < fit_params.size(); ++i){
    rw->IncludeDial(fit_params[i]);
    rw->SetDialValue(fit_params[i], fit_vals[i]);
  }
  rw->Reconfigure();


  minimizerFCN *thisFCN = new minimizerFCN(cardfile, fakeDataFile, outRootFile, rw, 1);
  thisFCN->SetOutName(outFile);

  if (!InputFile.empty()){

    // Set sample normalisations...
    std::cout<<"Seting signal tree from input file "<<InputFile<<std::endl;
    TFile* signalFile = new TFile(InputFile.c_str(),"READ");
    thisFCN->SetSignalTree(signalFile);

    thisFCN->ReconfigureSignal();

  } else {

    thisFCN->ReconfigureAllEvents();

  }

  double chi2 = thisFCN->GetCurrentChi2();


  LOG(FIT)<<"Final Chi^2 = "<<chi2<<std::endl;
  // write the weight tree
  outRootFile->cd();

  TTree* results = new TTree("fit_result","fit_result");
  results->Branch("chi2",&chi2,"chi2/D");
  results->Branch("parameter_names",&fit_params);
  results->Branch("parameter_values",&fit_vals);

  results->Fill();
  results->Write();
  thisFCN->Write();

  // Close everything
  outRootFile->Close();

  delete thisFCN;
  std::cout<<"Finished output "<<outFile<<std::endl;
  return 0;
}



