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

#include "comparisonRoutines.h"

/*
  Constructor/Destructor
*/
//************************
comparisonRoutines::comparisonRoutines(int argc, char* argv[]){
//************************

  // Set Defaults
  inputFileName = "";
  outputFileName = "";

  covarHist = NULL;
  covarHist_Free = NULL;
  correlHist = NULL;
  correlHist_Free = NULL;
  decompHist = NULL;
  decompHist_Free = NULL;

  inputFile = NULL;
  outputFile = NULL;
  fitStrategy = "Migrad,FixAtLim";
  fakeDataFile = "";

  minimizerObj = NULL;
  thisFCN = NULL;
  callFCN = NULL;

  parseArgs(argc, argv);

};

//*************************************
comparisonRoutines::~comparisonRoutines(){
//*************************************
};

/*
  Input Functions
*/
//*************************************
void comparisonRoutines::parseArgs(int argc, char* argv[]){
//*************************************

  std::string maxevents_flag = "";
  int verbosity_flag = 0;
  int error_flag = 0;

  // If No Arguments print commands
  for (int i = 1; i< argc; ++i){
    if (i+1 != argc){

      // Cardfile
      if (!std::strcmp(argv[i], "-c")) { cardFile=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-o")) { outputFileName=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-f")) { fitStrategy=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-i")) { inputFileName=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-q")) { configCmdFix.push_back(argv[i+1]); ++i;}
      else if (!std::strcmp(argv[i], "-n")) { maxevents_flag=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-v")) { verbosity_flag -= 1; }
      else if (!std::strcmp(argv[i], "+v")) { verbosity_flag += 1; }
      else if (!std::strcmp(argv[i], "-e")) { error_flag -= 1; }
      else if (!std::strcmp(argv[i], "+e")) { error_flag += 1; }
    } else std::cerr << "ERROR: unknown command line option given! - '" <<argv[i]<<" "<<argv[i+1]<<"'"<< std::endl;
  }

  if (outputFileName.empty())  std::cerr << "ERROR: output file not specified." << std::endl;
  if (cardFile.empty()) std::cerr << "ERROR: card file not specified."   << std::endl;

  // Parse fit routine
  std::string token;
  std::istringstream stream(fitStrategy);

  LOG(FIT)<< "Fit Routine = " << fitStrategy <<std::endl;
  while(std::getline(stream, token, ',')){
    fit_routines.push_back(token);
  }

  readCard();
  setupConfig();

  if (!maxevents_flag.empty()) FitPar::Config().SetParI("MAXEVENTS", atoi(maxevents_flag.c_str()));
  if (verbosity_flag != 0) FitPar::Config().SetParI("VERBOSITY", FitPar::Config().GetParI("VERBOSITY") + verbosity_flag);
  if (error_flag != 0) FitPar::Config().SetParI("ERROR", FitPar::Config().GetParI("ERROR") + error_flag);


  return;
};

//*************************************
void comparisonRoutines::initialSetup(){
//*************************************

  SetupCovariance();

  // output file open
  outputFile = new TFile(outputFileName.c_str(),"RECREATE");
  FitPar::Config().Write();

  // setup RW and FCN
  setupRWEngine();
  setupFCN();

  return;
}

//*************************************
void comparisonRoutines::readCard(){
//*************************************

  std::string line;
  std::ifstream card(this->cardFile.c_str(), ifstream::in);

  while(std::getline(card, line, '\n')){
    std::istringstream stream(line);

    FitPar::Config().cardLines.push_back(line);

    readParameters(line);
    readFakeDataPars(line);
    readSamples(line);
    readCovariance(line);
  }
  card.close();
  return;
};

//*****************************************
void comparisonRoutines::readParameters(std::string parstring){
//******************************************

  std::string token, parname;
  std::istringstream stream(parstring);   int val = 0;
  double entry;
  int partype;
  std::string curparstate = "";
  std::string partype_str = "";

  if (parstring.c_str()[0] == '#') return;

  while(std::getline(stream, token, ' ')){

    stream >> std::ws;

    std::istringstream stoken(token);
    if (val > 1 and val < 6) stoken >> entry;

    // Allow (parameter name val FIX)
    if (val > 2 and val < 6 and token.find("FIX") != std::string::npos) {
      startFixVals[parname] = true;
      fixVals[parname] = true;
      break;
    }

    if (val == 0 &&
	token.compare("niwg_parameter") &&
	token.compare("neut_parameter") &&
	token.compare("genie_parameter") &&
	token.compare("nuwro_parameter") &&
	token.compare("custom_parameter") &&
	token.compare("t2k_parameter")) {

      return;

    } else if (val == 0){
      partype_str = token;

      if (!token.compare("neut_parameter")) partype = 0;
      else if (!token.compare("niwg_parameter")) partype = 1;
      else if (!token.compare("genie_parameter")) partype = 3;
      else if (!token.compare("nuwro_parameter")) partype = 2;
      else if (!token.compare("custom_parameter")) partype = 4;
      else if (!token.compare("t2k_parameter")) partype = 5;

    } else if (val == 1) {
      params.push_back(token);
      parname = token;

      // Set Type
      params_type[parname] = partype;

      // Defaults
      startVals[parname]   = 0.0;
      currentVals[parname] = 0.0;
      errorVals[parname]   = 0.0;

      minVals[parname] = -1.0;
      maxVals[parname] = 1.0;
      stepVals[parname] = 0.5;

    } else if (val == 2){  // Nominal
      startVals[parname]   = entry;
      currentVals[parname] = entry;
      errorVals[parname]   = 0.0;
    } else if (val == 3){  minVals[parname] = entry;  // min
    } else if (val == 4){  maxVals[parname] = entry;  // max
    } else if (val == 5){  stepVals[parname] = entry; // step
    } else if (val == 6){ // type
      startFixVals[parname] = ( token.find("FIX") != std::string::npos );
      fixVals[parname] = ( token.find("FIX") != std::string::npos );

      curparstate = token;

    } else break;

    val++;
  }

  // Run Dial Conversions

  // ABSOLUTE CONVERSION
  if (curparstate.find("ABS") != std::string::npos){

    LOG(MIN)<<"Converting abs dial "<<parname<<" : "<<startVals[parname];
    startVals[parname]   = FitBase::RWAbsToSigma(partype_str, parname, startVals[parname]);
    currentVals[parname] = FitBase::RWAbsToSigma(partype_str, parname, currentVals[parname]);
    minVals[parname]     = FitBase::RWAbsToSigma(partype_str, parname, minVals[parname]);
    maxVals[parname]     = FitBase::RWAbsToSigma(partype_str, parname, maxVals[parname]);
    stepVals[parname]    = fabs((FitBase::RWAbsToSigma(partype_str, parname, startVals[parname] + stepVals[parname])
				 - FitBase::RWAbsToSigma(partype_str, parname, startVals[parname])));

    LOG(MIN)<<" -> "<<startVals[parname]<<std::endl;

    // FRACTION CONVERSION
  } else if (curparstate.find("FRAC") != std::string::npos){

    LOG(FIT)<<"Converting frac dial "<<parname<<" : "<<startVals[parname];
    startVals[parname]   = FitBase::RWFracToSigma(partype_str, parname, startVals[parname]);
    currentVals[parname] = FitBase::RWFracToSigma(partype_str, parname, currentVals[parname]);
    minVals[parname]     = FitBase::RWFracToSigma(partype_str, parname, minVals[parname]);
    maxVals[parname]     = FitBase::RWFracToSigma(partype_str, parname, maxVals[parname]);
    stepVals[parname]    = (FitBase::RWFracToSigma(partype_str, parname, stepVals[parname]));


    LOG(MIN)<<" -> "<<startVals[parname]<<std::endl;

  }


  return;
}

void comparisonRoutines::PlotLimits(){

  TDirectory* limfolder = (TDirectory*) outputFile->mkdir("Limits");
  limfolder->cd();

  // Set all parameters at their starting values
  for (UInt_t i = 0; i < params.size(); i++){
    currentVals[params[i]] = startVals[params[i]];
  }
  for (UInt_t i = 0; i < sampleDials.size(); i++){
    currentNorms[sampleDials[i]] = sampleNorms[sampleDials[i]];
  }

  TDirectory* nomfolder = (TDirectory*) limfolder->mkdir("nominal");
  nomfolder->cd();

  updateRWEngine(currentVals, currentNorms);
  thisFCN->ReconfigureAllEvents();
  thisFCN->Write();

  limfolder->cd();
  std::vector<std::string> allfolders;

  // Loop through each parameter
  for (UInt_t i = 0; i < params.size(); i++){

    if (fixVals[params[i]]) continue;

    while (currentVals[params[i]] > minVals[params[i]]){

      currentVals[params[i]] = currentVals[params[i]] - stepVals[params[i]];

      if (currentVals[params[i]] < minVals[params[i]])
	currentVals[params[i]] = minVals[params[i]];

      std::string curvalstring = std::string( Form( (params[i] + "_%f").c_str(), currentVals[params[i]] ) );
      if (std::find(allfolders.begin(), allfolders.end(), curvalstring) != allfolders.end())
	break;

      TDirectory* minfolder = (TDirectory*) limfolder->mkdir(Form( (params[i] + "_%f").c_str(), currentVals[params[i]] ) );
      minfolder->cd();

      allfolders.push_back(curvalstring);

      updateRWEngine(currentVals, currentNorms);
      thisFCN->ReconfigureAllEvents();

      thisFCN->Write();
    }
    currentVals[params[i]] = startVals[params[i]];

    while (currentVals[params[i]] < maxVals[params[i]]){

      currentVals[params[i]] = currentVals[params[i]] + stepVals[params[i]];

      if (currentVals[params[i]] > maxVals[params[i]])
	currentVals[params[i]] = maxVals[params[i]];

      std::string curvalstring = std::string( Form( (params[i] + "_%f").c_str(), currentVals[params[i]] ) );
      if (std::find(allfolders.begin(), allfolders.end(), curvalstring) != allfolders.end())
	break;

      TDirectory* maxfolder = (TDirectory*) limfolder->mkdir(Form( (params[i] + "_%f").c_str(), currentVals[params[i]] ) );

      maxfolder->cd();
      allfolders.push_back(curvalstring);


      updateRWEngine(currentVals, currentNorms);
      thisFCN->ReconfigureAllEvents();

      thisFCN->Write();
    }

    currentVals[params[i]] = startVals[params[i]];
    updateRWEngine(currentVals, currentNorms);
  }

}

//*******************************************
void comparisonRoutines::readFakeDataPars(std::string parstring){
//******************************************

  std::string token, parname;
  std::istringstream stream(parstring);
  int val = 0;
  double entry;

  if (parstring.c_str()[0] == '#') return;


  while(std::getline(stream, token, ' ')){
    stream >> std::ws;    // strip whitespace
    std::istringstream stoken(token);
    if (val == 2) stoken >> entry;

    if (val == 0){
      if(token.compare("fake_parameter") != 0) return;
    } else if (val == 1){
      fakeParams.push_back(token);
      parname = token;
    } else if (val == 2){

      std::cout<<"Set fake parameter "<<parname<<"  = "<<entry<<std::endl;
      fakeVals[parname] = entry;

    } else {
      break;
    }

    val++;
  }

  val = 0;
  while(std::getline(stream, token, ' ')){
    stream >> std::ws;    // strip whitespace
    std::istringstream stoken(token);
    if (val == 2) stoken >> entry;

    if (val == 0 && token.compare("fake_norm") != 0){ return; }
    if (val == 1){
      fakeSamples.push_back(token);
      parname = token;
    } else if (val == 2) fakeNorms[parname] = entry;

    val++;
  }
  return;
}



//******************************************
void comparisonRoutines::readSamples(std::string sampleString){
//******************************************

  std::string token, samplename;
  std::istringstream stream(sampleString);   int val = 0;
  double entry;

  if (sampleString.c_str()[0] == '#') return;


  while(std::getline(stream, token, ' ')){
    stream >> std::ws;    // strip whitespace
    std::istringstream stoken(token);
    stoken >> entry;


    if (val == 0){
      if (token.compare("sample") != 0){ return; }
    } else if (val == 1){

      samplename = token + "_norm";
      samples.push_back(token);
      sampleDials.push_back(samplename);

      sampleNorms[samplename]  = 1.0;
      currentNorms[samplename] = 1.0;
      errorNorms[samplename]   = 1.0;

    } else if (val == 2) {

      sampleTypes[samplename]  = token;
      bool fixed = (token.find("FREE") == std::string::npos);

      fixNorms[samplename]      = fixed; //fixed;
      startFixNorms[samplename] = fixed; //fixed;

      // defaults
      sampleNorms[samplename]  = 1.0;
      currentNorms[samplename] = 1.0;
      errorNorms[samplename]   = 0.0;

    } else if (val == 3) { sampleFiles[samplename] = token;
    } else if (val == 4){
      if (entry > 0.3 and entry < 1.7) {
	sampleNorms[samplename]  = entry;
	currentNorms[samplename] = entry;
	errorNorms[samplename]   = entry;
      }
    }

    val++;
  }
  std::cout<<"added sample "<<samplename<<" with norm "<<sampleNorms[samplename]<<" "<<currentNorms[samplename]<<std::endl;
  return;
}


/*
  Setup Functions
*/
//*************************************
void comparisonRoutines::setupConfig(){
//*************************************

  std::string par_dir =  std::string(std::getenv("EXT_FIT"))+"/parameters/";
  FitPar::Config().ReadParamFile( par_dir + "config.list.dat" );
  FitPar::Config().ReadParamFile( cardFile );

  for (unsigned int iter = 0; iter < configCmdFix.size(); iter++)
    FitPar::Config().ForceParam(configCmdFix[iter]);

  LOG_VERB(FitPar::Config().GetParS("VERBOSITY"));
  ERR_VERB(FitPar::Config().GetParS("VERBOSITY"));

  return;
};

//*************************************
void comparisonRoutines::setupRWEngine(){
//*************************************

  for (UInt_t i = 0; i < params.size(); i++){
    std::string name = params[i];
    std::cout << "Adding parameter " << name << std::endl;
    FitBase::GetRW() -> IncludeDial(name, params_type.at(name) );
  }

  for (UInt_t i = 0; i < sampleDials.size(); i++){
    std::string name = sampleDials[i];
    FitBase::GetRW()->IncludeDial(name, 6);
  }
  FitBase::GetRW()->Reconfigure();

  updateRWEngine(startVals, sampleNorms);

  return;
}

//*************************************
void comparisonRoutines::setupFCN(){
//*************************************

  LOG(FIT)<<"Making the minimizerFCN"<<std::endl;
  if (thisFCN) delete thisFCN;
  thisFCN = new minimizerFCN(cardFile, outputFile);
  thisFCN->SetOutName(outputFileName);
  setFakeData();
  callFCN = new ROOT::Math::Functor(*thisFCN,params.size() + sampleDials.size());

  return;
}

//*************************************
void comparisonRoutines::setFakeData(){
//*************************************

  if (fakeDataFile.empty()) return;

  if (fakeDataFile.compare("MC") == 0){

    LOG(FIT)<<"Setting fake data from MC starting prediction." <<std::endl;
    updateRWEngine(fakeVals, fakeNorms);

    FitBase::GetRW()->Reconfigure();
    thisFCN->ReconfigureAllEvents();
    thisFCN->SetFakeData("MC");

    updateRWEngine(currentVals, currentNorms);

    LOG(FIT)<<"Set all data to fake MC predictions."<<std::endl;
  } else {
    thisFCN->SetFakeData(fakeDataFile);
  }

  return;
}

/*
  Fitting Functions
*/
//*************************************
void comparisonRoutines::updateRWEngine(std::map<std::string,double>& updateVals, std::map<std::string,double>& updateNorms){
//*************************************

  for (UInt_t i = 0; i < params.size(); i++){
    std::string name = params[i];

    if (updateVals.find(name) == updateVals.end()) continue;
    FitBase::GetRW()->SetDialValue(name,updateVals.at(name));
  }

  for (UInt_t i = 0; i < sampleDials.size(); i++){
    std::string name = sampleDials[i];

    if (updateNorms.find(name) == updateNorms.end()){
      FitBase::GetRW()->SetDialValue(name, 1.0);
    } else {
      FitBase::GetRW()->SetDialValue(name, updateNorms.at(name));
    }
  }

  FitBase::GetRW()->Reconfigure();
  return;
}

//**************
void comparisonRoutines::SelfFit(){
//*************



  for (UInt_t i = 0; i < fit_routines.size(); i++){

    std::string routine = fit_routines.at(i);

    // Stop fit if all pars are constant
    int NFREE = 0;
    for (UInt_t i = 0; i < params.size(); i++){
      if (!fixVals[params[i]]) NFREE++;
    }
    for (UInt_t i = 0; i < sampleDials.size(); i++){
      if (!fixNorms[sampleDials[i]]) NFREE++;
    }
    if (NFREE == 0) break;

    LOG(FIT)<<"Running Routine: "<<routine<<std::endl;
    if (routine.find("PlotLimits") != std::string::npos) PlotLimits();
    else if (routine.find("ErrorBands") != std::string::npos) GenerateErrorBands();
    
  }

  return;
}


//*************************************
void comparisonRoutines::saveCurrentState(std::string subdir){
//*************************************

  LOG(FIT)<<"Saving current FCN predictions" <<std::endl;

  outputFile->cd();
  FitBase::GetRW()->Reconfigure();
  thisFCN->ReconfigureAllEvents();

  if (!subdir.empty()){
    TDirectory* nominalDIR =(TDirectory*) outputFile->mkdir("nominal");
    nominalDIR->cd();
  }

  thisFCN->Write();

  outputFile->cd();

  return;
}

//*************************************
void comparisonRoutines::saveNominal(){
//*************************************

  LOG(FIT)<<"Saving Neut Nominal Predictions (be cautious with this)" <<std::endl;

  //rw->ResetAll();
  FitBase::GetRW()->Reconfigure();
  saveCurrentState("nominal");
};

//*************************************
void comparisonRoutines::savePrefit(){
//*************************************

  LOG(FIT)<<"Saving Prefit Predictions"<<std::endl;

  updateRWEngine(startVals, sampleNorms);
  saveCurrentState("prefit");
  updateRWEngine(currentVals, currentNorms);


};


/*
  MISC Functions
*/

//*************************************
void comparisonRoutines::SetupCovariance(){
//*************************************

  // Remove covares if they exist
  if (covarHist) delete covarHist;
  if (covarHist_Free) delete covarHist_Free;
  if (correlHist) delete correlHist;
  if (correlHist_Free) delete correlHist_Free;
  if (decompHist) delete decompHist;
  if (decompHist_Free) delete decompHist_Free;

  int NFREE = 0;
  int NDIM = 0;

  // Get NFREE from min or from vals (for cases when doing throws)
  if (minimizerObj){
    NFREE = minimizerObj->NFree();
    NDIM  = minimizerObj->NDim();
  } else {
    NDIM = params.size() + sampleDials.size();
    for (UInt_t i = 0; i < params.size(); i++){
      if (!fixVals[params[i]]) NFREE++;
    }
    for (UInt_t i = 0; i < sampleDials.size(); i++){
      if (!fixNorms[sampleDials[i]]) NFREE++;
    }
  }

  if (NDIM == 0) return;

  covarHist = new TH2D("covariance","covariance",NDIM,0,NDIM,NDIM,0,NDIM);
  if (NFREE > 0) covarHist_Free = new TH2D("covariance_free","covariance_free",NFREE,0,NFREE,NFREE,0,NFREE);

  // Set Bin Labels
  int countall = 0;
  int countfree = 0;
  for (UInt_t i = 0; i < params.size(); i++){

    covarHist->GetXaxis()->SetBinLabel(countall+1,params[i].c_str());
    covarHist->GetYaxis()->SetBinLabel(countall+1,params[i].c_str());
    countall++;

    if (!fixVals[params[i]] and NFREE > 0){
      covarHist_Free->GetXaxis()->SetBinLabel(countfree+1,params[i].c_str());
      covarHist_Free->GetYaxis()->SetBinLabel(countfree+1,params[i].c_str());
      countfree++;
    }
  }

  for (UInt_t i = 0; i < sampleDials.size(); i++){

    covarHist->GetXaxis()->SetBinLabel(countall+1,sampleDials[i].c_str());
    covarHist->GetYaxis()->SetBinLabel(countall+1,sampleDials[i].c_str());
    countall++;

    if (!fixNorms[sampleDials[i]] and NFREE > 0){
      covarHist_Free->GetXaxis()->SetBinLabel(countfree+1,sampleDials[i].c_str());
      covarHist_Free->GetYaxis()->SetBinLabel(countfree+1,sampleDials[i].c_str());
      countfree++;
    }
  }



  // Fill Input Covariances
  for (UInt_t i = 0; i < input_covariances.size(); i++){
    TH2D covplot = input_covariances.at(i);

    for(Int_t i = 0; i < covplot.GetNbinsX(); i++){
      for (Int_t j = 0; j < covplot.GetNbinsY(); j++){

	std::string parx = covplot.GetXaxis()->GetBinLabel(i+1);
	std::string pary = covplot.GetYaxis()->GetBinLabel(j+1);

	for (Int_t k = 0; k < covarHist->GetNbinsX(); k++){
	  for (Int_t l = 0; l < covarHist->GetNbinsY(); l++){

	    if (!parx.compare(covarHist->GetXaxis()->GetBinLabel(k+1)) and
		!pary.compare(covarHist->GetYaxis()->GetBinLabel(l+1)))

	      covarHist->SetBinContent(k+1,l+1, covarHist->GetBinContent(k+1,l+1) + covplot.GetBinContent(i+1,k+1));

	  }
	}

	for (Int_t k = 0; k < covarHist_Free->GetNbinsX(); k++){
          for (Int_t l = 0; l < covarHist_Free->GetNbinsY(); l++){

            if (!parx.compare(covarHist_Free->GetXaxis()->GetBinLabel(k+1)) and
                !pary.compare(covarHist_Free->GetYaxis()->GetBinLabel(l+1))){

	      covarHist_Free->SetBinContent(k+1,l+1, covarHist_Free->GetBinContent(k+1,l+1) + covplot.GetBinContent(i+1,j+1));
	    }
          }
        }
      }
    }
  }

  // Setup Current Vals due to covariance inputs now as well
  for (UInt_t i = 0; i < input_dials.size(); i++){
    TH1D* plot = input_dials.at(i);

    for (Int_t j = 0; j < plot->GetNbinsX(); j++){
      std::string parname = std::string(plot->GetXaxis()->GetBinLabel(j+1));

      if (currentVals.find(parname) != currentVals.end()) currentVals[parname] = plot->GetBinContent(j+1);
      else if (currentNorms.find(parname) != currentNorms.end()) currentNorms[parname] = plot->GetBinContent(j+1);

    }
  }

  // Final Step Setting
  countall = 0;
  countfree = 0;
  for (UInt_t i = 0; i < params.size(); i++){

    double stepsq = stepVals[params[i]] * stepVals[params[i]];

    if (!fixVals[params[i]] and covarHist->GetBinContent(countall+1,countall+1) == 0.0)
      covarHist->SetBinContent(countall+1,countall+1,stepsq);

    countall++;

    if (!fixVals[params[i]] and NFREE > 0){
      if (covarHist_Free->GetBinContent(countfree+1,countfree+1) == 0.0)
	covarHist_Free->SetBinContent(countfree+1,countfree+1,stepsq);
      countfree++;
    }
  }


  for (UInt_t i = 0; i < sampleDials.size(); i++){

    double stepsq = 0.1*0.1;

    if (!fixNorms[sampleDials[i]] and covarHist->GetBinContent(countall+1,countall+1) == 0.0)
      covarHist->SetBinContent(countall+1,countall+1,stepsq);

    countall++;

    if (!fixNorms[sampleDials[i]] and NFREE > 0){

      if (covarHist_Free->GetBinContent(countfree+1,countfree+1) == 0.0)
	covarHist_Free->SetBinContent(countfree+1,countfree+1,stepsq);

      countfree++;
    }
  }

  correlHist = PlotUtils::GetCorrelationPlot(covarHist,"correlation");
  decompHist = PlotUtils::GetDecompPlot(covarHist,"decomposition");

  if (NFREE > 0)correlHist_Free = PlotUtils::GetCorrelationPlot(covarHist_Free, "correlation_free");
  if (NFREE > 0)decompHist_Free = PlotUtils::GetDecompPlot(covarHist_Free,"decomposition_free");

  return;
};

//*************************************
void comparisonRoutines::readCovariance(std::string covarString){
//*************************************

  std::string token, covarname, covartype;
  std::istringstream stream(covarString);   int val = 0;

  if (covarString.c_str()[0] == '#') return;

  while(std::getline(stream, token, ' ')){
    stream >> std::ws;    // strip whitespace
    std::istringstream stoken(token);


    if (val == 0 && token.compare("covar") != 0){ return; }
    else if (val == 1){ covarname = token; }
    else if (val == 2) { covartype = token; }
    else if (val == 3) {

      parameter_pulls* temp_pulls = new parameter_pulls(covarname, token, rw, covartype, "");
      this->input_covariances.push_back(temp_pulls->GetFullCovarMatrix());
      this->input_dials.push_back((TH1D*)temp_pulls->GetDataList().at(0)->Clone());

      delete temp_pulls;
    }

    val++;
  }

  return;
};



//*************************************
void comparisonRoutines::ThrowCovariance(bool uniformly){
//*************************************

  std::vector<double> rands;

  if (!decompHist_Free) {
    ERR(WRN) << "Trying to throw 0 free parameters"<<std::endl;
    return;
  }

  for (Int_t i = 0; i < decompHist_Free->GetNbinsX(); i++){
    rands.push_back(gRandom->Gaus(0.0,1.0));
  }

  for (UInt_t i = 0; i < params.size(); i++){
    thrownVals[params[i]] = currentVals[params[i]];
  }
  for (UInt_t i = 0; i < sampleDials.size(); i++){
    thrownNorms[sampleDials[i]] = currentNorms[sampleDials[i]];
  }

  for (Int_t i = 0; i < decompHist_Free->GetNbinsX(); i++){

    std::string parname = std::string(decompHist_Free->GetXaxis()->GetBinLabel(i+1));
    double mod = 0.0;

    if (!uniformly){
      for (Int_t j = 0; j < decompHist_Free->GetNbinsY(); j++){
	mod += rands[j] * decompHist_Free->GetBinContent(j+1,i+1);
      }
    }

    if (currentVals.find(parname) != currentVals.end()) {

      if (uniformly) thrownVals[parname] = gRandom->Uniform(minVals[parname],maxVals[parname]);
      else {  thrownVals[parname] = 	  currentVals[parname] + mod; }

    } else if (currentNorms.find(parname) != currentNorms.end()){

      if (uniformly) thrownNorms[parname] = gRandom->Uniform(1.0, 0.7);
      else thrownNorms[parname] = currentNorms[parname]+ mod;

    }
  }

  for (UInt_t i = 0; i < params.size(); i++){
    if (thrownVals[params[i]] < minVals[params[i]]) thrownVals[params[i]] = minVals[params[i]];
    if (thrownVals[params[i]] > maxVals[params[i]]) thrownVals[params[i]] = maxVals[params[i]];
  }

  for (UInt_t i = 0; i < sampleDials.size(); i++){
    if (thrownNorms[sampleDials[i]] < 0.3) thrownNorms[sampleDials[i]] = 0.3;
    if (thrownNorms[sampleDials[i]] > 1.7) thrownNorms[sampleDials[i]] = 1.7;
  }

  return;
};

//*************************************
void comparisonRoutines::GenerateErrorBands(){
//*************************************

  TDirectory* errorDIR = (TDirectory*) outputFile->mkdir("error_bands");
  errorDIR->cd();

  TFile* tempfile = new TFile((outputFileName + ".throws.root").c_str(),"RECREATE");
  tempfile->cd();
  int nthrows = FitPar::Config().GetParI("error_throws");

  updateRWEngine(currentVals, currentNorms);
  thisFCN->ReconfigureAllEvents();

  TDirectory* nominal = (TDirectory*) tempfile->mkdir("nominal");
  nominal->cd();
  thisFCN->Write();


  TDirectory* outnominal = (TDirectory*) outputFile->mkdir("nominal_throw");
  outnominal->cd();
  thisFCN->Write();


  errorDIR->cd();
  TTree* parameterTree = new TTree("throws","throws");
  double chi2;
  for (UInt_t i = 0; i < params.size(); i++)
    parameterTree->Branch(params[i].c_str(), &thrownVals[params[i]], (params[i] + "/D").c_str());
  for (UInt_t i = 0; i < sampleDials.size(); i++)
    parameterTree->Branch(sampleDials[i].c_str(), &thrownNorms[sampleDials[i]], (sampleDials[i] + "/D").c_str());
  parameterTree->Branch("chi2",&chi2,"chi2/D");


  bool uniformly = FitPar::Config().GetParB("error_uniform");

  // Run Throws and save
  for (Int_t i = 0; i < nthrows; i++){

    TDirectory* throwfolder = (TDirectory*)tempfile->mkdir(Form("throw_%i",i));
    throwfolder->cd();

    ThrowCovariance(uniformly);
    updateRWEngine(thrownVals, thrownNorms);
    thisFCN->ReconfigureAllEvents();
    thisFCN->Write();
    chi2 = thisFCN->GetLikelihood();

    parameterTree->Fill();
  }

  errorDIR->cd();
  decompHist_Free->Write();
  covarHist_Free->Write();
  parameterTree->Write();

  for (UInt_t i = 0; i < input_covariances.size(); i++){
    input_covariances.at(i).Write();
  }

  delete parameterTree;

  // Now go through the keys in the temporary file and look for TH1D, and TH2D plots
  TIter next(nominal->GetListOfKeys());
  TKey *key;
  while ((key = (TKey*)next())) {
    TClass *cl = gROOT->GetClass(key->GetClassName());
    if (!cl->InheritsFrom("TH1D") and !cl->InheritsFrom("TH2D")) continue;
    TH1D *baseplot = (TH1D*)key->ReadObj();
    std::string plotname = std::string(baseplot->GetName());

    int nbins = baseplot->GetNbinsX()*baseplot->GetNbinsY();

    // Setup TProfile with RMS option
    TProfile* tprof = new TProfile((plotname + "_prof").c_str(),(plotname + "_prof").c_str(),nbins, 0, nbins, "S");

    // Setup The TTREE
    double* bincontents;
    bincontents = new double[nbins];

    double* binlowest;
    binlowest = new double[nbins];

    double* binhighest;
    binhighest = new double[nbins];

    errorDIR->cd();
    TTree* bintree = new TTree((plotname + "_tree").c_str(), (plotname + "_tree").c_str());
    for (Int_t i = 0; i < nbins; i++){
      bincontents[i] = 0.0;
      binhighest[i] = 0.0;
      binlowest[i] = 0.0;
      bintree->Branch(Form("content_%i",i),&bincontents[i],Form("content_%i/D",i));
    }

    for (Int_t i = 0; i < nthrows; i++){
      TH1* newplot = (TH1*)tempfile->Get(Form(("throw_%i/" + plotname).c_str(),i));

      for (Int_t j = 0; j < nbins; j++){
	tprof->Fill(j+0.5, newplot->GetBinContent(j+1));
	bincontents[j] = newplot->GetBinContent(j+1);

	if (bincontents[j] < binlowest[j] or i == 0) binlowest[j] = bincontents[j];
	if (bincontents[j] > binhighest[j] or i == 0) binhighest[j] = bincontents[j];
      }

      errorDIR->cd();
      bintree->Fill();

      delete newplot;
    }

    errorDIR->cd();

    for (Int_t j = 0; j < nbins; j++){

      if (!uniformly){
	baseplot->SetBinError(j+1,tprof->GetBinError(j+1));

      } else {
	baseplot->SetBinContent(j+1, (binlowest[j] + binhighest[j]) / 2.0);
        baseplot->SetBinError(j+1, (binhighest[j] - binlowest[j])/2.0);
      }
    }

    errorDIR->cd();
    baseplot->Write();
    tprof->Write();
    bintree->Write();

    delete baseplot;
    delete tprof;
    delete bintree;
    delete [] bincontents;
  }

  return;
};
