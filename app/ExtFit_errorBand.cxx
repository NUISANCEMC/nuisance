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

#ifdef __GENIE_ENABLED__
#include "T2KGenieReWeight.h"
#endif

#include "FitEvent.h"
#include "fitFCN.h"
#include "minimizerFCN.h"
#include "splinesFCN.h"
#include "FitParameters.h"

#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"

#include "parameter_pulls.h"

bool savenominal = false;
bool saveprefit = false;
int nthrows = 50;
int nstart = 0;

std::string ReplaceString(std::string subject, const std::string& search,
                          const std::string& replace) {
  size_t pos = 0;
  while ((pos = subject.find(search, pos)) != std::string::npos) {
    subject.replace(pos, search.length(), replace);
    pos += replace.length();
  }
  return subject;
}

//********************************************************************
void readParameters(std::string cardFile, std::vector<std::string>& name, std::map<std::string, double>& start,
		    std::map<std::string, double>& min, std::map<std::string, double>& max, std::map<std::string, bool>& fix);
//********************************************************************
void readFakeDataPars(std::string cardFile, std::vector<std::string>& name, std::map<std::string, double>& start);
//********************************************************************
//void readSamples(std::string cardFile, std::vector<std::string>& name, std::vector<bool>& fix, std::vector<double>& normVal);
//********************************************************************
void saveOutput(ROOT::Math::Minimizer* minimizerObj, minimizerFCN *thisFCN,
		std::vector<double>& valStart, std::vector<double>& valMax, std::vector<double>& valMin,
		std::vector<double>& valStep,  std::vector<double>& valFix);
//********************************************************************
void readSamples(std::string cardFile, std::vector<std::string>& name, std::vector<std::string>& type, std::vector<std::string>& files, std::vector<bool>& fix, std::vector<double>& normVal);

//********************************************************************
bool checkResult(std::string inFile){
//********************************************************************

  TFile* in = new TFile(inFile.c_str(),"READ");
  TH1D* fitState = (TH1D*) in->Get("fit_state");

  bool isValidMinimum = bool(fitState->GetBinContent(1));

  in->Close();
};

//********************************************************************
int GLSminimize(std::string cardFile, std::string outName, TFile *outFile, std::string fitStrategy, std::string fakeDataFile, int type, std::string infile) {
//********************************************************************

  FitPar::Config().out = outFile;

  std::vector<std::string>           params;
  std::map<std::string, double>      startVals, minValues, maxValues;
  std::map<std::string, bool>        fixValues;
  std::vector<std::string>           nSampleNames;
  std::vector<bool>                  nSampleShape;
  std::vector<double>                nSampleVal;

  std::vector<std::string>           nFakeParams;
  std::map<std::string,double>       nFakeStarts;
  std::vector<std::string> parNames;

  std::vector<std::string> nSampleTypes;
  std::vector<std::string> nSampleFiles;

  // Single list for saving parameters later
  std::vector<double> parStart, parMin, parMax, parStep, parFix;

  // Read the card file for this fit
  std::cout<<"Reading Card Files"<<std::endl;
  readParameters(cardFile, params, startVals, minValues, maxValues, fixValues);
  readSamples(cardFile, nSampleNames, nSampleTypes, nSampleFiles, nSampleShape, nSampleVal);
  readFakeDataPars(cardFile, nFakeParams, nFakeStarts);



  std::cout<<"---------------------------------------"<<std::endl;
  std::cout<<"Setting up generator libraries"<<std::endl;

  gSystem->Load("event1.so");

  t2krew::T2KReWeight *rw           = new t2krew::T2KReWeight();
  t2krew::T2KNeutReWeight  *neut_rw = new t2krew::T2KNeutReWeight();
  t2krew::T2KNIWGReWeight  *niwg_rw = new t2krew::T2KNIWGReWeight();

  rw->AdoptWghtEngine("neut_rw", neut_rw);
  rw->AdoptWghtEngine("niwg_rw", niwg_rw);

  #ifdef __GENIE_ENABLED__
    // Warning - involves a rather obnoxious genie splash logo
    t2krew::T2KGenieReWeight *genie_rw = new t2krew::T2KGenieReWeight();
    rw->AdoptWghtEngine("genie_rw", genie_rw);
    GHepRecord::SetPrintLevel(-2);
#endif

  //  rw->Systematics().SetPrintLevel(-1);

  int nfreepars = 0;

  // Decide which parameters are included in T2KReWeight
  for(unsigned int i = 0; i < params.size(); ++i){
    std::cout<<"Including parameter "<<t2krew::T2KSyst::FromString(params[i])<<std::endl;
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

  outFile->cd();
  minimizerFCN *thisFCN = new minimizerFCN(cardFile, fakeDataFile, outFile, rw, 1);
  thisFCN->SetOutName(outName);

  std::cout<<"---------------------------------------"<<std::endl;
  std::cout<<" Saving NEUT Nominal (be cautious with this)." <<std::endl;

  savenominal = FitPar::Config().GetParI("SAVE_NOMINAL");
  saveprefit  = FitPar::Config().GetParI("SAVE_PREFIT");

  if (savenominal){
    // Initial reconfigure
    rw->Reconfigure();
    thisFCN->ReconfigureAllEvents();

    TDirectory* nominalDIR =(TDirectory*) outFile->mkdir("nominalneut");
    nominalDIR->cd();

    thisFCN->Write();
  }
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
  std::cout<<"Generating a signal tree for use "<<std::endl;

  // rw->Reconfigure();
  //  thisFCN->ScanSignalEvents("temp_event_file_"+outName);

  std::cout<<"---------------------------------------"<<std::endl;
  std::cout<<"Generating throws of covariance input"<<std::endl;

  int timetaken = 300;

  // split the infiles

  std::istringstream stream(infile);
  std::vector<std::string> infiles;
  std::string temp_string;
  while (std::getline(stream, temp_string, ',')) {
    infiles.push_back(temp_string);
  }

  std::vector<VExperimentBase*> covar_throwers;
  int i = 0;
  for (std::vector<std::string>::iterator iter; iter != infiles.end(); iter++){
    std::cout<<"Input file for covar = "<<infiles.at(i)<<std::endl;
    covar_throwers.push_back( new parameter_pulls("parameter_covariance", infiles.at(i), rw, "", "") );
    i++;
  }

  for (int i = nstart; i < nthrows+nstart; i++){

    int j = 0;
    for (std::vector<VExperimentBase*>::iterator iter; iter != covar_throwers.end(); iter++){

      VExperimentBase* pars_cov = (*iter);

      if (i > 0){
	pars_cov->ThrowCovariance();
      }

      TH1* pars_thrown = pars_cov->GetDataHistogram().at(0);

      for (int j = 0; j < pars_thrown->GetNbinsX(); j++){

	std::string name = pars_thrown->GetXaxis()->GetBinLabel(j+1);

	for (int k = 0; k < params.size(); k++){

	  if (name == params.at(k)){

	    if (pars_thrown->GetBinContent(j+1) < minValues.at(name)) startVals.at(name) = minValues.at(name);
	    else if (pars_thrown->GetBinContent(j+1) > maxValues.at(name)) startVals.at(name) = maxValues.at(name);
	    else startVals.at(name) = pars_thrown->GetBinContent(j+1);

	  }
	}
      }
    }


    // make a new script from the cards and submit it
    std::ostringstream parstring;
    for (int k = 0; k < params.size(); k++){

      std::string name = params.at(k);

      parstring << "echo 'parameter ";
      parstring << params.at(k)     << " " << startVals.at(name) << " ";
      parstring << minValues.at(name) << " " << maxValues.at(name)   << " FIX' >> $CARD \n";

    }

    // Make an equivalent data script
    std::ostringstream samstring;
    for (int k = 0; k < nSampleNames.size(); k++){

      samstring << "echo 'sample ";
      samstring << nSampleNames.at(k) << " " << nSampleTypes.at(k) << " ";
      samstring << nSampleFiles.at(k) << " " << nSampleVal.at(k) << "' >> $CARD \n";

    }

    // Make an identifier
    std::ostringstream ss;
    ss << "iter_" << i;

    std::string namescript = "workerScript_" +ss.str()+ "_sub.sh";
    std::string nameoutput = "workerOutput_" +ss.str()+ "_result.root";

    // Check if nameoutput exists, if it does skip.

    // Open the workscript
    ofstream workerscript;
    workerscript.open(namescript.c_str());
    workerscript << "#!/bin/sh \n";
    workerscript << "source /home/stowell/t2krep/FitterSetup/SetupFitterEnv.sh head \n";
    workerscript << "CARD="<<nameoutput<<".card \n";
    workerscript << "OUT=" <<nameoutput<<" \n";
    workerscript << "echo '' > $CARD \n";
    workerscript << parstring.str() << "\n";
    workerscript << samstring.str() << "\n";
    workerscript << "ExtFit_functionPoint.exe -c $CARD -o /scratch/stowell/$OUT \n";
    workerscript << "mv /scratch/stowell/$OUT ./ \n";
    workerscript << "rm $CARD \n";
    workerscript.close();

    if (i == nstart){
      timetaken = time(NULL);
      system(("source "+namescript).c_str());
      timetaken = (time(NULL) - timetaken) * 1.10;
    } else {
      system(("ExtFit_runJob.sh "+namescript).c_str());
    }

    if ((i+1) % 80 == 0) {
      std::cout<<"Waiting "<<timetaken<<std::endl;
      sleep(60*30);
    }
  }

  // Loop through the output shown in functionPoint
  // Use this to set Gaussian errors on every bin within every plot in the file
  // Maybe create a nominal value first and use that

  // Open the nominal file
  std::string nameoutput = "workerOutput_iter_0_result.root";
  TFile* nominalFile = new TFile(nameoutput.c_str(),"READ");

  // Go through the loop and check the output exits
  for (int i = nstart; i < nthrows+nstart; i++){

    // Make an identifier
    std::ostringstream ss;
    ss << "iter_" << i;

    nameoutput = "workerOutput_" +ss.str()+ "_result.root";

  }

  std::cout<<"---------------------------------------"<<std::endl;
  std::cout<<"Saving Final Output. "<<std::endl;

  TDirectory* postfitDIR =(TDirectory*) outFile->mkdir("postfit");
  postfitDIR->cd();
  thisFCN->Write();

  outFile->cd();

  //  delete minimizerObj;
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
      else if (!std::strcmp(argv[i], "-n")) { nthrows=atoi(argv[i+1]); ++i;}
      else if (!std::strcmp(argv[i], "-s")) { nstart=atoi(argv[i+1]); ++i;}
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

  FitPar::Config().ReadParamFile(cardFile);

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
    int iret = GLSminimize(cardFile, outFile, out, fitStrategy, fakeDataFile, type, InputFile);
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




void readSamples(std::string cardFile, std::vector<std::string>& name, std::vector<std::string>& type, std::vector<std::string>& files, std::vector<bool>& fix, std::vector<double>& normVal){

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
	type.push_back(token);
	if (token.find("FREE") != std::string::npos) fix.push_back(1); // dont try and vary norm of a ratio.
	else fix.push_back(0);
	normVal.push_back(1.0);
      } else if (val == 3) {
	files.push_back(token);
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
