#include "minimizerRoutine.h"

/*
  Constructor/Destructor
*/
//************************
minimizerRoutine::minimizerRoutine(int argc, char* argv[]){
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
minimizerRoutine::~minimizerRoutine(){
//************************************* 
};

/*
  Input Functions
*/
//*************************************
void minimizerRoutine::parseArgs(int argc, char* argv[]){
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
void minimizerRoutine::initialSetup(){
//*************************************

  SetupCovariance();
  
  // Check if an input file is provided
  if (!inputFileName.empty())  readInputFile();
    
  // Check if fit continues
  if (fitContinue){

    // Open previous output file to continue
    inputFileName = outputFileName;
    
    if (!checkPreviousResult()) readInputFile();
    else {return;}
  }

  // output file open
  outputFile = new TFile(outputFileName.c_str(),"RECREATE");
  FitPar::Config().Write();
  
  // setup RW and FCN
  setupRWEngine();
  setupFCN();

  return;
}

//*************************************
void minimizerRoutine::readCard(){
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
void minimizerRoutine::readParameters(std::string parstring){
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

void minimizerRoutine::PlotLimits(){

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
void minimizerRoutine::readFakeDataPars(std::string parstring){
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
void minimizerRoutine::readSamples(std::string sampleString){
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

//*************************************
void minimizerRoutine::readInputFile(){
//*************************************

  // Read in file
  inputFile = new TFile(inputFileName.c_str(), "READ");
  // Check results

  // Set starting vals as previous result

  inputFile->Close();
  delete inputFile;
  
  return;
}

//*************************************
bool minimizerRoutine::checkPreviousResult(){
//*************************************

  if (inputFile) delete inputFile;
  inputFile = new TFile(inputFileName.c_str(),"READ");
  
  TH1D* fitState = (TH1D*) inputFile->Get("fit_state");
  bool isValidMinimum = false;
  
  if (fitState){
    isValidMinimum = bool(fitState->GetBinContent(1));
  }

  inputFile->Close();
  delete inputFile;

  return isValidMinimum;
}

/*
  Setup Functions
*/
//*************************************
void minimizerRoutine::setupConfig(){
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
void minimizerRoutine::setupRWEngine(){
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

  std::cout<<"RW Engines updated"<<std::endl;
  
  return;
}

//*************************************
void minimizerRoutine::setupFCN(){
//*************************************

  LOG(FIT)<<"Making the minimizerFCN"<<std::endl;
  if (thisFCN) delete thisFCN;
  thisFCN = new minimizerFCN(cardFile, outputFile);
  thisFCN->SetOutName(outputFileName);
  setFakeData();
  callFCN = new ROOT::Math::Functor(*thisFCN,params.size() + sampleDials.size());

  return;
}


//******************************************
void minimizerRoutine::setupFitter(std::string routine){
//******************************************
    
  // Make the fitter
  std::string fitclass = "";
  std::string fittype  = "";


  // Get correct types
  if      (!routine.compare("Migrad"))      { fitclass = "Minuit2"; fittype = "Migrad";     
  } else if (!routine.compare("Simplex"))     { fitclass = "Minuit2"; fittype = "Simplex";   
  } else if (!routine.compare("Combined"))    { fitclass = "Minuit2"; fittype = "Combined";  
  } else if (!routine.compare("Brute"))       { fitclass = "Minuit2"; fittype = "Scan";      
  } else if (!routine.compare("Fumili"))      { fitclass = "Minuit2"; fittype = "Fumili";       
  } else if (!routine.compare("ConjugateFR")) { fitclass = "GSLMultiMin"; fittype = "ConjugateFR";   
  } else if (!routine.compare("ConjugatePR")) { fitclass = "GSLMultiMin"; fittype = "ConjugatePR";   
  } else if (!routine.compare("BFGS"))        { fitclass = "GSLMultiMin"; fittype = "BFGS";         
  } else if (!routine.compare("BFGS2"))       { fitclass = "GSLMultiMin"; fittype = "BFGS2";         
  } else if (!routine.compare("SteepDesc"))   { fitclass = "GSLMultiMin"; fittype = "SteepestDescent";         
    //  } else if (!routine.compare("GSLMulti"))    { fitclass = "GSLMultiFit"; fittype = "";         // Doesn't work out of the box
  } else if (!routine.compare("GSLSimAn"))    { fitclass = "GSLSimAn"; fittype = "";   }
  
  

  // make minimizer
  if (minimizerObj) delete minimizerObj;
  minimizerObj = ROOT::Math::Factory::CreateMinimizer(fitclass, fittype);
									       
  minimizerObj->SetMaxFunctionCalls(FitPar::Config().GetParI("minimizer.maxcalls"));

  if (!routine.compare("Brute")){
    minimizerObj->SetMaxFunctionCalls(params.size() * params.size()*4);
    minimizerObj->SetMaxIterations(params.size() * params.size()*4);
  }

  minimizerObj->SetMaxIterations(FitPar::Config().GetParI("minimizer.maxiterations"));
  minimizerObj->SetTolerance(FitPar::Config().GetParD("minimizer.tolerance"));

  cout<<"Current Tolerance = "<<FitPar::Config().GetParD("minimizer.tolerance")<<endl;
  cout<<"Current Tolerance 2 = "<<FitPar::Config().GetParD("minimizer.tolerance")<<endl;
  minimizerObj->SetStrategy(FitPar::Config().GetParI("minimizer.strategy"));
  minimizerObj->SetFunction(*callFCN);

  int ipar = 0;
  //Add Fit Parameters
  for (UInt_t i = 0; i < params.size(); i++){
    std::string systString = params.at(i);

    bool fixed = true;
    double vstart, vstep, vlow, vhigh;
    vstart = vstep = vlow = vhigh = 0.0;
    
    if (currentVals.find(systString) != currentVals.end()) vstart = currentVals.at(systString);
    if (minVals.find(systString)  != minVals.end() ) vlow   = minVals.at(systString);
    if (maxVals.find(systString)  != maxVals.end() ) vhigh  = maxVals.at(systString);
    if (stepVals.find(systString) != stepVals.end()) vstep  = stepVals.at(systString);
    if (fixVals.find(systString)  != fixVals.end() ) fixed  = fixVals.at(systString);

    // fix for errors
    if (vhigh == vlow) vhigh += 1.0;

    minimizerObj->SetVariable(ipar, systString, vstart, vstep);
    minimizerObj->SetVariableLimits(ipar,vlow,vhigh);
    if (fixed) {
      minimizerObj->FixVariable(ipar);
      LOG(FIT) << "Fixed Param: "<<systString<<std::endl;
    } else {
      LOG(FIT) << "Free  Param: "<<systString<<" Start:"<<vstart<<" Range:"<<vlow<<" to "<<vhigh<<" Step:"<<vstep<<std::endl;
    }

    ipar++;
  }

  // Add sample norms
  for (UInt_t i = 0; i < sampleDials.size(); i++){
    std::string sampString = sampleDials.at(i);

    bool fixed = true;
    double vnorm;

    if (currentNorms.find(sampString) != currentNorms.end()) vnorm = currentNorms.at(sampString);
    else {
      currentNorms[sampString] = 1.0;
      vnorm = 1.0;
    }

    if (fixNorms.find(sampString) != fixNorms.end())  fixed = fixNorms.at(sampString);
    
    minimizerObj->SetVariable(ipar, sampString, vnorm, 0.1);
    minimizerObj->SetVariableLimits(ipar,0.3, 2.0);

    if (fixed) {
      minimizerObj->FixVariable(ipar);
      LOG(FIT) << "Fixed Param: "<<sampString<<std::endl;
    } else {
      LOG(FIT) << "Free  Param: "<<sampString<<" Start:"<<vnorm<<" Range:"<<0.3<<"-"<<2.0<<" Step:"<<0.1<<std::endl;
    }
    
    ipar++;
  }

  LOG(FIT) << "Setup Minimizer: "<<minimizerObj->NDim()<<"(NDim) "<<minimizerObj->NFree()<<"(NFree)"<<std::endl;
  
  return;
}

//*************************************
void minimizerRoutine::setFakeData(){
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
void minimizerRoutine::updateRWEngine(std::map<std::string,double>& updateVals, std::map<std::string,double>& updateNorms){
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
void minimizerRoutine::SelfFit(){
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
    if (routine.find("LowStat") != std::string::npos) LowStatRoutine(routine);
    else if (routine.find("PlotLimits") != std::string::npos) PlotLimits();
    else if (routine.find("ErrorBands") != std::string::npos) GenerateErrorBands();
    else RunFitRoutine(routine);
      
  }

  return;
}

//*************************************
void minimizerRoutine::RunFitRoutine(std::string routine){
//*************************************

  int NFREE = 0;
  for (UInt_t i = 0; i < params.size(); i++){
    if (!fixVals[params[i]]) NFREE++;
  }
  for (UInt_t i = 0; i < sampleDials.size(); i++){
    if (!fixNorms[sampleDials[i]]) NFREE++;
  }

  if (NFREE == 0) {
    updateRWEngine(currentVals, currentNorms);
    thisFCN->ReconfigureAllEvents();
    return;
  }
  
  // Run Fix at Limit before fitter setup if required
  if (routine == "FixAtLim"){ FixAtLimit(); return; }
  
  // set fitter at the current start values
  setupFitter(routine);
  outputFile->cd();
     
  // choose what to do with the minimizer depending on routine.
  if      (!routine.compare("Migrad") or  
	   !routine.compare("Simplex") or
	   !routine.compare("Combined") or
	   !routine.compare("Brute") or
	   !routine.compare("Fumili") or
	   !routine.compare("ConjugateFR") or
	   !routine.compare("ConjugatePR") or
	   !routine.compare("BFGS") or
	   !routine.compare("BFGS2") or
	   !routine.compare("SteepDesc") or
	   //	   !routine.compare("GSLMulti") or
	   !routine.compare("GSLSimAn")) {

    std::cout<<"Starting routine "<<routine<<std::endl;
    minimizerObj->Minimize();
    std::cout<<"Getting State"<<std::endl;
    getMinimizerState();
  }

  // other otptions
  else if (!routine.compare("Chi2Scan1D")) Create1DScans();
  else if (!routine.compare("Chi2Scan2D")) Chi2Scan2D();
  else if (!routine.compare("Contour")) CreateContours();
}

//*************************************
void minimizerRoutine::getMinimizerState(){
//*************************************
  
  LOG(FIT) << "Minimizer State: "<<std::endl;
  // Get X and Err
  const double *values = minimizerObj->X();
  const double *errors = minimizerObj->Errors();
  int ipar = 0;
    
  LOG(FIT) << "  #  " << left << setw(30) << "Parameter " 
	   << " = "
	   << setw(10) << "Value" << " +- " 
	   << setw(10) << "Error" << " "
	   << setw(8) << "Units" << " (Sigma Variation) "<<std::endl;

  // loop through parameters and get the values
  for (UInt_t i = 0; i < params.size(); i++){
    std::string systString = params.at(i);

    currentVals.at(systString) = values[ipar];
    
    std::ostringstream curparstring;

    curparstring << " " << setw(2) << left << ipar << ". " << setw(30) << systString << " = ";
    std::string curunits = "";//rw->GetUnits(systString);
    std::ostringstream ss;
    
    curparstring << setw(10) << 0.0 ;//rw->ConvertSigmaToValue(systString, values[ipar]);

    if (fixVals.at(systString)){

      curparstring << "    " << setw(10) << "    " << setw(8) << curunits << "  (Fixed)";
      errorVals.at(systString) = 0.0;

    } else {
      double err = 0.0; //rw->ConvertSigmaToValue(systString, values[ipar]+errors[ipar]) - rw->ConvertSigmaToValue(systString, values[ipar]);
      curparstring << " +- " << setw(10) << err;
      curparstring << " " << setw(8) << curunits;
      if (curunits.compare("Sig.")) curparstring <<  " (" << values[ipar] << " +- "<< errors[ipar] <<")";

      errorVals.at(systString) = errors[ipar];
    }
      
    LOG(FIT) << curparstring.str() <<std::endl;

    ipar++;
  }
    
  for (UInt_t i = 0; i < sampleDials.size(); i++){
    std::string sampString = sampleDials.at(i);
    
    currentNorms.at(sampString) = values[ipar];

    std::ostringstream curparstring;
    curparstring << " " << left << setw(2) << ipar << ". " << setw(30) << sampString << " = ";
		
    curparstring << setw(10) << values[ipar];

    if (fixNorms.at(sampString)){
      
      curparstring << "    " << setw(10) << "    "<<setw(8) << "Frac. " << "  (Fixed)";
      errorNorms.at(sampString) = 0.0;

    } else {

      curparstring << " +- " << setw(10) << errors[ipar] << " " << setw(8) << "Frac. ";
      errorNorms.at(sampString) = errors[ipar];

    }
    LOG(FIT)<<curparstring.str()<<std::endl;
    ipar++;
  }
  LOG(FIT)<<"------------"<<std::endl;
  
  return;
};

//*************************************
void minimizerRoutine::LowStatRoutine(std::string routine){
//*************************************

  LOG(FIT) << "Running Low Statistics Routine: "<<routine<<std::endl;
  int lowstatsevents = FitPar::Config().GetParI("LOWSTATEVENTS");
  int maxevents      = FitPar::Config().GetParI("MAXEVENTS");
  int verbosity      = FitPar::Config().GetParI("VERBOSITY");

  std::string trueroutine = routine;
  std::string substring = "LowStat";
  trueroutine.erase( trueroutine.find(substring),
		     substring.length() );
    
  // Set MAX EVENTS=1000
  FitPar::Config().SetParI("MAXEVENTS",lowstatsevents);
  FitPar::Config().SetParI("VERBOSITY",3);
  setupFCN();
    
  RunFitRoutine(trueroutine);
     
  FitPar::Config().SetParI("MAXEVENTS",maxevents);
  setupFCN();

  FitPar::Config().SetParI("VERBOSITY",verbosity);
  return;
}

//*************************************
void minimizerRoutine::Create1DScans(){
//*************************************

  // At the current point create a 1D Scan for all parametes (Uncorrelated)
  for (UInt_t i = 0; i < params.size(); i++){

    if (fixVals[params[i]]) continue;
	
    double scanmiddlepoint = currentVals[params[i]];

    // Determine N points needed
    double limlow  = minVals[params[i]];
    double limhigh = maxVals[params[i]];
    double step    = stepVals[params[i]];
    
    int npoints = int( fabs(limhigh - limlow)/(step+0.) );
    int count = 0;
    
    double* xvals;
    double* yvals;
    xvals = new double[npoints];
    yvals = new double[npoints];
    
    // Set Start Point
    currentVals[params[i]] = limlow;
    updateRWEngine(currentVals, currentNorms);
    thisFCN->ReconfigureAllEvents();
    
    // Loop over scan points
    while (currentVals[params[i]] < limhigh){

      // Update Prediction
      updateRWEngine(currentVals, currentNorms);
      thisFCN->ReconfigureAllEvents();

      // Fill Graph Points
      yvals[count] = thisFCN->GetLikelihood();
      xvals[count] = currentVals[params[i]];

      std::cout<<"Current Vals = "<<currentVals[params[i]]<<" = "<<yvals[count]<<std::endl;
      
      // Get Next par set
      currentVals[params[i]] += step;
      count++;
    }

    // Save Graph
    TGraph* scanGraph = new TGraph(count, xvals, yvals);
    scanGraph->Write(params[i].c_str());

    // Reset Parameter
    currentVals[params[i]] = scanmiddlepoint;
    
    delete scanGraph;
    delete xvals;
    delete yvals;
  }

  return;
}

//*************************************                                                                                 
void minimizerRoutine::Chi2Scan2D(){
  //*************************************                                                                                 

  // Scan I                                                                                                             
  for (UInt_t i = 0; i < params.size(); i++){
    if (fixVals[params[i]]) continue;

    // Scan J                                                                                                           
    for (UInt_t j = 0; j < params.size(); j++){
      if (fixVals[params[j]]) continue;

      if (params[i] == params[j]) continue;

      double scanmid_i = currentVals[params[i]];
      double scanmid_j = currentVals[params[j]];

      double limlow_i  = minVals[params[i]];
      double limhigh_i = maxVals[params[i]];
      double step_i    = stepVals[params[i]];

      double limlow_j  = minVals[params[j]];
      double limhigh_j = maxVals[params[j]];
      double step_j    = stepVals[params[j]];

      int npoints_i = int( fabs(limhigh_i - limlow_i)/(step_i+0.) ) + 1;
      int npoints_j = int( fabs(limhigh_j - limlow_j)/(step_j+0.) ) + 1;

      TH2D* contour = new TH2D("countour","countour",
                               npoints_i, limlow_i, limhigh_i,
                               npoints_j, limlow_j, limhigh_j );

      contour->GetXaxis()->SetTitle(params[i].c_str());
      contour->GetYaxis()->SetTitle(params[j].c_str());

      cout<<"Running scan for "<<params[i]<<" "<<params[j]<<endl;

      // Fill bins                                                                                                      
      for (int x = 0; x < contour->GetNbinsX(); x++){

        // Set X Val                                                                                                    
        currentVals[params[i]] = contour->GetXaxis()->GetBinCenter(x+1);
        cout<<"Set Value i "<<currentVals[params[i]]<<endl;

        // Loop Y                                                                                                       
        for (int y = 0; y < contour->GetNbinsY(); y++){

          // Set Y Val                                                                                                  
          currentVals[params[j]] = contour->GetYaxis()->GetBinCenter(y+1);
          cout<<"Set Value j "<<currentVals[params[j]]<<endl;

          // Reconfigure                                                                                                
          updateRWEngine(currentVals, currentNorms);
          thisFCN->ReconfigureAllEvents();

          // Chi2                                                                                                       
          double chi2 = thisFCN->GetLikelihood();

          // Fill Contour                                                                                               
          contour->SetBinContent(x+1,y+1, chi2);

          cout<<"Filling "<<x<<" "<<y<<" "<<chi2<<endl;

          currentVals[params[j]] = scanmid_j;
        }

        currentVals[params[i]] = scanmid_i;
        currentVals[params[j]] = scanmid_j;
      }

      // Save contour                                                                                                   
      contour->Write();
    }
  }

  return;
}



//*************************************
void minimizerRoutine::CreateContours(){
//*************************************

  // Use MINUIT for this if possible

  
  return;
}

//*************************************
void minimizerRoutine::FixAtLimit(){
//*************************************
  
  for (UInt_t i = 0; i < params.size(); i++){
    std::string systString = params.at(i);
    double curVal = currentVals.at(systString);
    double minVal = minVals.at(systString);
    double maxVal = minVals.at(systString);

    if ((curVal - minVal) < 0.0001){
      currentVals.at(systString) = minVal;
    }

    if ((maxVal - curVal) < 0.0001){
      currentVals.at(systString) = maxVal;
    }
  }

  for (UInt_t i = 0; i < sampleDials.size(); i++){
    std::string sampString = sampleDials.at(i);
    double curVal = currentNorms.at(sampString);
    double minVal = 0.3;
    double maxVal = 2.0;

    if ((curVal - minVal) < 0.0001){
      currentVals.at(sampString) = minVal;
    }

    if ((maxVal - curVal) < 0.0001){
      currentVals.at(sampString) = maxVal;
    }
  }
  return;
}


/*
  Write Functions
*/
//*************************************
void minimizerRoutine::saveFitterOutput(std::string dir){
//*************************************

  if (!dir.empty()){
    TDirectory* nominalDIR =(TDirectory*) outputFile->mkdir(dir.c_str());
    nominalDIR->cd();
  }

  if (minimizerObj){
    saveMinimizerState();
    saveCurrentState();
  }

  saveCurrentState();

  outputFile->cd();
  
  return;
}

//*************************************
void minimizerRoutine::saveMinimizerState(){
//*************************************

  if (!minimizerObj) return;
  
  // Get Values and Errors
  getMinimizerState();
  outputFile->cd();
  
  // Save tree with fit status
  std::vector<std::string> nameVect;
  std::vector<double>      valVect;
  std::vector<double>      errVect;
  std::vector<double>      minVect;
  std::vector<double>      maxVect;
  std::vector<double>      startVect;
  std::vector<int>      endfixVect;
  std::vector<int>      startfixVect;

  int NFREEPARS = minimizerObj->NFree();
  int NPARS = minimizerObj->NDim();

  int ipar = 0;
  // Dial Values
  for (UInt_t i = 0; i < params.size(); i++){
    std::string name = params.at(i);

    nameVect    .push_back(name);
    valVect     .push_back(currentVals.at(name));
    errVect     .push_back(errorVals.at(name));
    minVect     .push_back(minVals.at(name));
    maxVect     .push_back(maxVals.at(name));
    startVect   .push_back(startVals.at(name));
    endfixVect  .push_back(fixVals.at(name));
    startfixVect.push_back(startFixVals.at(name));

    ipar++;
  }

  // Sample Norms
  for (UInt_t i = 0; i < sampleDials.size(); i++){
    std::string name = sampleDials.at(i);

    nameVect    .push_back(name);
    valVect     .push_back(currentNorms.at(name));
    errVect     .push_back(errorNorms.at(name));
    minVect     .push_back(0.3);
    maxVect     .push_back(1.7);
    startVect   .push_back(sampleNorms.at(name));
    endfixVect  .push_back(fixNorms.at(name));
    startfixVect.push_back(startFixNorms.at(name));

    ipar++;
  }


  int NFREE = minimizerObj->NFree();
  int NDIM  = minimizerObj->NDim();
  
  double CHI2 = thisFCN->GetLikelihood();
  int NBINS = thisFCN->GetNDOF();
  int NDOF = NBINS - NFREE;

  // Write fit results
  TTree* fit_tree = new TTree("fit_result","fit_result");
  fit_tree->Branch("parameter_names",&nameVect);
  fit_tree->Branch("parameter_values",&valVect);
  fit_tree->Branch("parameter_errors",&errVect);
  fit_tree->Branch("parameter_min",&minVect);
  fit_tree->Branch("parameter_max",&maxVect);
  fit_tree->Branch("parameter_start",&startVect);
  fit_tree->Branch("parameter_fix",&endfixVect);
  fit_tree->Branch("parameter_startfix",&startfixVect);
  fit_tree->Branch("CHI2",&CHI2,"CHI2/D");
  fit_tree->Branch("NDOF",&NDOF,"NDOF/I");
  fit_tree->Branch("NBINS",&NBINS,"NBINS/I");
  fit_tree->Branch("NDIM",&NDIM,"NDIM/I");
  fit_tree->Branch("NFREE",&NFREE,"NFREE/I");
  fit_tree->Fill();
  fit_tree->Write();

  // Make dial variables
  TH1D dialvar = TH1D("fit_dials","fit_dials",NPARS,0,NPARS);
  TH1D startvar = TH1D("start_dials","start_dials",NPARS,0,NPARS);
  TH1D minvar   = TH1D("min_dials","min_dials",NPARS,0,NPARS);
  TH1D maxvar   = TH1D("max_dials","max_dials",NPARS,0,NPARS);

  for (UInt_t i = 0; i < nameVect.size(); i++){
    std::string name = nameVect.at(i);
    
    dialvar.SetBinContent(i+1, valVect.at(i));
    dialvar.SetBinError(i+1, errVect.at(i));
    dialvar.GetXaxis()->SetBinLabel(i+1, name.c_str());
    
    startvar.SetBinContent(i+1, startVect.at(i));
    startvar.GetXaxis()->SetBinLabel(i+1, name.c_str());
    
    minvar.SetBinContent(i+1,   minVect.at(i));
    minvar.GetXaxis()->SetBinLabel(i+1, name.c_str());
    
    maxvar.SetBinContent(i+1,   maxVect.at(i));
    maxvar.GetXaxis()->SetBinLabel(i+1, name.c_str());
  }

  // Save Dial Plots
  dialvar.Write();
  startvar.Write();
  minvar.Write();
  maxvar.Write();
  
  // Save TString for cardFile
  
  // Save fit_status plot
  TH1D statusplot = TH1D("fit_status","fit_status",8,0,8);
  std::string fit_labels[8] = {"status", "cov_status",  \
			       "maxiter", "maxfunc",	\
			       "iter",    "func",	\
			       "precision", "tolerance"};
  double fit_vals[8];
  fit_vals[0] = minimizerObj->Status() + 0.;
  fit_vals[1] = minimizerObj->CovMatrixStatus() + 0.;
  fit_vals[2] = minimizerObj->MaxIterations() + 0.;
  fit_vals[3] = minimizerObj->MaxFunctionCalls()+ 0.;
  fit_vals[4] = minimizerObj->NIterations() + 0.;
  fit_vals[5] = minimizerObj->NCalls() + 0.;
  fit_vals[6] = minimizerObj->Precision() + 0.;
  fit_vals[7] = minimizerObj->Tolerance() + 0.;

  for (int i = 0; i < 8; i++){
    statusplot.SetBinContent(i+1, fit_vals[i]);
    statusplot.GetXaxis()->SetBinLabel(i+1, fit_labels[i].c_str());
  }

  statusplot.Write();
  
  // Sort Covariances
  SetupCovariance();

  if (minimizerObj->CovMatrixStatus() > 0){

    for (int i = 0; i < covarHist->GetNbinsX(); i++){
      for (int j = 0; j < covarHist->GetNbinsY(); j++){
	covarHist->SetBinContent(i+1,j+1, minimizerObj->CovMatrix(i,j));
      }
    }

    int freex = 0;
    int freey = 0;

    for (int i = 0; i < covarHist->GetNbinsX(); i++){

      freey = 0;
      if (minimizerObj->IsFixedVariable(i)) continue;

      for (int j = 0; j < covarHist->GetNbinsY(); j++){
	if (minimizerObj->IsFixedVariable(j)) continue;

	covarHist_Free->SetBinContent(freex+1,freey+1, minimizerObj->CovMatrix(i,j));
	freey++;
	
      }
    }
  }

  correlHist = PlotUtils::GetCorrelationPlot(covarHist,"correlation");
  decompHist = PlotUtils::GetDecompPlot(covarHist,"decomposition");

  if (NFREE > 0)correlHist_Free = PlotUtils::GetCorrelationPlot(covarHist_Free, "correlation_free");
  if (NFREE > 0)decompHist_Free = PlotUtils::GetDecompPlot(covarHist_Free,"decomposition_free");

  if (covarHist) covarHist->Write();
  if (covarHist_Free) covarHist_Free->Write();
  if (correlHist) correlHist->Write();
  if (correlHist_Free) correlHist_Free->Write();
  if (decompHist) decompHist->Write();
  if (decompHist_Free) decompHist_Free->Write();


  return;
}

//*************************************
void minimizerRoutine::saveCurrentState(std::string subdir){
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
void minimizerRoutine::saveNominal(){
//*************************************

  LOG(FIT)<<"Saving Neut Nominal Predictions (be cautious with this)" <<std::endl;
  
  //rw->ResetAll();
  FitBase::GetRW()->Reconfigure();
  saveCurrentState("nominal");
};

//*************************************
void minimizerRoutine::savePrefit(){
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
int minimizerRoutine::GetStatus(){
//*************************************
  
  return 0;
}

//************************************* 
void minimizerRoutine::SetupCovariance(){
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
void minimizerRoutine::readCovariance(std::string covarString){
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
void minimizerRoutine::ThrowCovariance(bool uniformly){
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
void minimizerRoutine::GenerateErrorBands(){
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
