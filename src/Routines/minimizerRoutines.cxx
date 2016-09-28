// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NUISANCE.
*
*    NUISANCE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NUISANCE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NUISANCE.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include "StatusMessage.h"

#include "minimizerRoutines.h"

/*
  Constructor/Destructor
*/
//************************
minimizerRoutines::minimizerRoutines(int argc, char* argv[]){
//************************

  fInputFile = "";
  fInputRootFile = NULL;
  
  fOutputFile = "";
  fOutputRootFile = NULL;

  fCovar  = fCovarFree  = NULL;
  fCorrel = fCorrelFree = NULL;
  fDecomp = fDecompFree = NULL;
  
  fStrategy = "Migrad,FixAtLim";
  fRoutines.clear();

  fCardFile = "";
  
  fFakeDataInput = "";

  fSampleFCN    = NULL;
  
  fMinimizer    = NULL;
  fMinimizerFCN = NULL;
  fCallFunctor  = NULL;
  
  ParseArgs(argc, argv);

};

//*************************************
minimizerRoutines::~minimizerRoutines(){
//*************************************
};

/*
  Input Functions
*/
//*************************************
void minimizerRoutines::ParseArgs(int argc, char* argv[]){
//*************************************

  std::string maxevents_flag = "";
  int verbosity_flag = 0;
  int error_flag = 0;

  // If No Arguments print commands
  for (int i = 1; i< argc; ++i){
    if (i+1 != argc){

      // Cardfile
      if (!std::strcmp(argv[i], "-c")) { fCardFile=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-o")) { fOutputFile=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-f")) { fStrategy=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-q")) { configCmdFix.push_back(argv[i+1]); ++i;}
      else if (!std::strcmp(argv[i], "-n")) { maxevents_flag=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-v")) { verbosity_flag -= 1; }
      else if (!std::strcmp(argv[i], "+v")) { verbosity_flag += 1; }
      else if (!std::strcmp(argv[i], "-e")) { error_flag -= 1; }
      else if (!std::strcmp(argv[i], "+e")) { error_flag += 1; }
    } else std::cerr << "ERROR: unknown command line option given! - '" <<argv[i]<<" "<<argv[i+1]<<"'"<< std::endl;
  }

  if (fOutputFile.empty())  std::cerr << "ERROR: output file not specified." << std::endl;
  if (fCardFile.empty()) std::cerr << "ERROR: card file not specified."   << std::endl;

  // Parse fit routine
  std::string token;
  std::istringstream stream(fStrategy);

  LOG(FIT)<< "Fit Routine = " << fStrategy <<std::endl;
  while(std::getline(stream, token, ',')){
    fRoutines.push_back(token);
  }

  ReadCard();
  SetupConfig();
  
  if (!maxevents_flag.empty()) FitPar::Config().SetParI("MAXEVENTS", atoi(maxevents_flag.c_str()));
  if (verbosity_flag != 0) FitPar::Config().SetParI("VERBOSITY", FitPar::Config().GetParI("VERBOSITY") + verbosity_flag);
  if (error_flag != 0) FitPar::Config().SetParI("ERROR", FitPar::Config().GetParI("ERROR") + error_flag);

  return;
};

//*************************************
void minimizerRoutines::InitialSetup(){
//*************************************

  SetupCovariance();

  // output file open
  fOutputRootFile = new TFile(fOutputFile.c_str(),"RECREATE");
  FitPar::Config().Write();

  // setup RW and FCN
  SetupRWEngine();
  SetupFCN();

  return;
}

//*************************************
void minimizerRoutines::ReadCard(){
//*************************************

  std::string line;
  std::ifstream card(this->fCardFile.c_str(), ifstream::in);

  while(std::getline(card, line, '\n')){
    std::istringstream stream(line);

    FitPar::Config().cardLines.push_back(line);

    ReadParameters(line);
    ReadFakeDataPars(line);
    ReadSamples(line);
  }
  card.close();
 
  return;
};

//*****************************************
void minimizerRoutines::ReadParameters(std::string parstring){
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
      fStartFixVals[parname] = true;
      fFixVals[parname] = true;
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

      if (!token.compare("neut_parameter")) partype = kNEUT;
      else if (!token.compare("niwg_parameter")) partype = kNIWG;
      else if (!token.compare("genie_parameter")) partype = kGENIE;
      else if (!token.compare("nuwro_parameter")) partype = kNUWRO;
      else if (!token.compare("custom_parameter")) partype = kCUSTOM;
      else if (!token.compare("t2k_parameter")) partype = kT2K;

    } else if (val == 1) {
      fParams.push_back(token);
      parname = token;

      // Set Type
      fTypeVals[parname] = partype;

      // Defaults
      fStartVals[parname]   = 0.0;
      fCurVals[parname] = 0.0;
      fErrorVals[parname]   = 0.0;

      fFixVals[parname] = true;
      fStartFixVals[parname] = true;
      fMinVals[parname] = -1.0;
      fMaxVals[parname] = 1.0;
      fStepVals[parname] = 0.0;

    } else if (val == 2){  // Nominal
      fStartVals[parname]   = entry;
      fCurVals[parname] = entry;
      fErrorVals[parname]   = 0.0;

      fMinVals[parname] = entry - 1.0;
      fMaxVals[parname] = entry + 1.0;
      
    } else if (val == 3){  fMinVals[parname] = entry;  // min
    } else if (val == 4){  fMaxVals[parname] = entry;  // max
    } else if (val == 5){  fStepVals[parname] = entry; // step
    } else if (val == 6){ // type
      fStartFixVals[parname] = ( token.find("FIX") != std::string::npos );
      fFixVals[parname] = ( token.find("FIX") != std::string::npos );

      curparstate = token;

    } else break;

    val++;
  }

  // Run Dial Conversions

  // ABSOLUTE CONVERSION
  if (curparstate.find("ABS") != std::string::npos){

    LOG(MIN)<<"Converting abs dial "<<parname<<" : "<<fStartVals[parname];
    fStartVals[parname]   = FitBase::RWAbsToSigma(partype_str, parname, fStartVals[parname]);
    fCurVals[parname] = FitBase::RWAbsToSigma(partype_str, parname, fCurVals[parname]);
    fMinVals[parname]     = FitBase::RWAbsToSigma(partype_str, parname, fMinVals[parname]);
    fMaxVals[parname]     = FitBase::RWAbsToSigma(partype_str, parname, fMaxVals[parname]);
    fStepVals[parname]    = fabs((FitBase::RWAbsToSigma(partype_str, parname, fStartVals[parname] + fStepVals[parname])
				 - FitBase::RWAbsToSigma(partype_str, parname, fStartVals[parname])));

    LOG(MIN)<<" -> "<<fStartVals[parname]<<std::endl;

    // FRACTION CONVERSION
  } else if (curparstate.find("FRAC") != std::string::npos){

    LOG(FIT)<<"Converting frac dial "<<parname<<" : "<<fStartVals[parname];
    fStartVals[parname]   = FitBase::RWFracToSigma(partype_str, parname, fStartVals[parname]);
    fCurVals[parname] = FitBase::RWFracToSigma(partype_str, parname, fCurVals[parname]);
    fMinVals[parname]     = FitBase::RWFracToSigma(partype_str, parname, fMinVals[parname]);
    fMaxVals[parname]     = FitBase::RWFracToSigma(partype_str, parname, fMaxVals[parname]);
    fStepVals[parname]    = (FitBase::RWFracToSigma(partype_str, parname, fStepVals[parname]));

    LOG(MIN)<<" -> "<<fStartVals[parname]<<std::endl;
  }

  return;
}

void minimizerRoutines::PlotLimits(){

  TDirectory* limfolder = (TDirectory*) fOutputRootFile->mkdir("Limits");
  limfolder->cd();

  // Set all parameters at their starting values
  for (UInt_t i = 0; i < fParams.size(); i++){
    fCurVals[fParams[i]] = fStartVals[fParams[i]];
  }

  TDirectory* nomfolder = (TDirectory*) limfolder->mkdir("nominal");
  nomfolder->cd();

  UpdateRWEngine(fCurVals);
  fSampleFCN->ReconfigureAllEvents();
  fSampleFCN->Write();

  limfolder->cd();
  std::vector<std::string> allfolders;

  // Loop through each parameter
  for (UInt_t i = 0; i < fParams.size(); i++){

    if (fFixVals[fParams[i]]) continue;

    while (fCurVals[fParams[i]] > fMinVals[fParams[i]]){

      fCurVals[fParams[i]] = fCurVals[fParams[i]] - fStepVals[fParams[i]];

      if (fCurVals[fParams[i]] < fMinVals[fParams[i]])
	fCurVals[fParams[i]] = fMinVals[fParams[i]];

      std::string curvalstring = std::string( Form( (fParams[i] + "_%f").c_str(), fCurVals[fParams[i]] ) );
      if (std::find(allfolders.begin(), allfolders.end(), curvalstring) != allfolders.end())
	break;

      TDirectory* minfolder = (TDirectory*) limfolder->mkdir(Form( (fParams[i] + "_%f").c_str(), fCurVals[fParams[i]] ) );
      minfolder->cd();

      allfolders.push_back(curvalstring);

      UpdateRWEngine(fCurVals);
      fSampleFCN->ReconfigureAllEvents();

      fSampleFCN->Write();
    }
    fCurVals[fParams[i]] = fStartVals[fParams[i]];

    while (fCurVals[fParams[i]] < fMaxVals[fParams[i]]){

      fCurVals[fParams[i]] = fCurVals[fParams[i]] + fStepVals[fParams[i]];

      if (fCurVals[fParams[i]] > fMaxVals[fParams[i]])
	fCurVals[fParams[i]] = fMaxVals[fParams[i]];

      std::string curvalstring = std::string( Form( (fParams[i] + "_%f").c_str(), fCurVals[fParams[i]] ) );
      if (std::find(allfolders.begin(), allfolders.end(), curvalstring) != allfolders.end())
	break;

      TDirectory* maxfolder = (TDirectory*) limfolder->mkdir(Form( (fParams[i] + "_%f").c_str(), fCurVals[fParams[i]] ) );

      maxfolder->cd();
      allfolders.push_back(curvalstring);


      UpdateRWEngine(fCurVals);
      fSampleFCN->ReconfigureAllEvents();

      fSampleFCN->Write();
    }

    fCurVals[fParams[i]] = fStartVals[fParams[i]];
    UpdateRWEngine(fCurVals);
  }

}

//*******************************************
void minimizerRoutines::ReadFakeDataPars(std::string parstring){
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
      if(token.compare("fake_parameter") != 0 &&
	 token.compare("fake_norm") != 0) return;
     
    } else if (val == 1){      
      parname = token;
      
    } else if (val == 2){
      fFakeVals[parname] = entry;

    } else {
      break;
    }

    val++;
  }
  
  return;
}



//******************************************
void minimizerRoutines::ReadSamples(std::string sampleString){
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
      fParams.push_back(samplename);
      
      fTypeVals[samplename] = kNORM;      
      fFixVals[samplename] = true;
      fStartFixVals[samplename] = true;
      fStartVals[samplename]  = 1.0;
      fCurVals[samplename] = 1.0;
      fErrorVals[samplename]   = 1.0;
      fMinVals[samplename] = 0.1;
      fMaxVals[samplename] = 10.0;
      fStepVals[samplename] = 0.5;
	
    } else if (val == 3) {

      bool fixed = (token.find("FREE") == std::string::npos);

      fFixVals[samplename]      = fixed; //fixed;
      fStartFixVals[samplename] = fixed; //fixed;

    } else if (val == 2) { 
    } else if (val == 4){
      if (entry > 0.3 and entry < 1.7) {
	fStartVals[samplename]  = entry;
	fCurVals[samplename] = entry;
	fErrorVals[samplename]   = entry;
      }
    }

    val++;
  }
  return;
}

/*
  Setup Functions
*/
//*************************************
void minimizerRoutines::SetupConfig(){
//*************************************

  std::string par_dir =  std::string(std::getenv("EXT_FIT"))+"/parameters/";
  FitPar::Config().ReadParamFile( par_dir + "config.list.dat" );
  FitPar::Config().ReadParamFile( fCardFile );

  for (unsigned int iter = 0; iter < configCmdFix.size(); iter++)
    FitPar::Config().ForceParam(configCmdFix[iter]);

  LOG_VERB(FitPar::Config().GetParS("VERBOSITY"));
  ERR_VERB(FitPar::Config().GetParS("VERBOSITY"));

  return;
};

//*************************************
void minimizerRoutines::SetupRWEngine(){
//*************************************

  for (UInt_t i = 0; i < fParams.size(); i++){
    std::string name = fParams[i];
    FitBase::GetRW() -> IncludeDial(name, fTypeVals.at(name) );
  }
  FitBase::GetRW()->Reconfigure();

  UpdateRWEngine(fStartVals);

  std::cout<<"RW Engines updated"<<std::endl;

  return;
}

//*************************************
void minimizerRoutines::SetupFCN(){
//*************************************

  LOG(FIT)<<"Making the jointFCN"<<std::endl;
  if (fSampleFCN) delete fSampleFCN;
  fSampleFCN = new JointFCN(fCardFile, fOutputRootFile);
  SetFakeData();
  
  fMinimizerFCN = new MinimizerFCN( fSampleFCN );
  fCallFunctor  = new ROOT::Math::Functor( *fMinimizerFCN, fParams.size() );

  fSampleFCN->CreateIterationTree( "fit_iterations", FitBase::GetRW() );
  
  return;
}


//******************************************
void minimizerRoutines::SetupFitter(std::string routine){
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
  if (fMinimizer) delete fMinimizer;
  fMinimizer = ROOT::Math::Factory::CreateMinimizer(fitclass, fittype);

  fMinimizer->SetMaxFunctionCalls(FitPar::Config().GetParI("minimizer.maxcalls"));

  if (!routine.compare("Brute")){
    fMinimizer->SetMaxFunctionCalls(fParams.size() * fParams.size()*4);
    fMinimizer->SetMaxIterations(fParams.size() * fParams.size()*4);
  }

  fMinimizer->SetMaxIterations(FitPar::Config().GetParI("minimizer.maxiterations"));
  fMinimizer->SetTolerance(FitPar::Config().GetParD("minimizer.tolerance"));
  fMinimizer->SetStrategy(FitPar::Config().GetParI("minimizer.strategy"));
  fMinimizer->SetFunction(*fCallFunctor);

  int ipar = 0;
  //Add Fit Parameters
  for (UInt_t i = 0; i < fParams.size(); i++){
    std::string systString = fParams.at(i);

    bool fixed = true;
    double vstart, vstep, vlow, vhigh;
    vstart = vstep = vlow = vhigh = 0.0;

    if (fCurVals.find(systString) != fCurVals.end()) vstart = fCurVals.at(systString);
    if (fMinVals.find(systString)  != fMinVals.end() ) vlow   = fMinVals.at(systString);
    if (fMaxVals.find(systString)  != fMaxVals.end() ) vhigh  = fMaxVals.at(systString);
    if (fStepVals.find(systString) != fStepVals.end()) vstep  = fStepVals.at(systString);
    if (fFixVals.find(systString)  != fFixVals.end() ) fixed  = fFixVals.at(systString);

    // fix for errors
    if (vhigh == vlow) vhigh += 1.0;

    fMinimizer->SetVariable(ipar, systString, vstart, vstep);
    fMinimizer->SetVariableLimits(ipar,vlow,vhigh);
    if (fixed) {
      fMinimizer->FixVariable(ipar);
      LOG(FIT) << "Fixed Param: "<<systString<<std::endl;
    } else {
      LOG(FIT) << "Free  Param: "<<systString<<" Start:"<<vstart<<" Range:"<<vlow<<" to "<<vhigh<<" Step:"<<vstep<<std::endl;
    }

    ipar++;
  }

  LOG(FIT) << "Setup Minimizer: "<<fMinimizer->NDim()<<"(NDim) "<<fMinimizer->NFree()<<"(NFree)"<<std::endl;

  return;
}

//*************************************
void minimizerRoutines::SetFakeData(){
//*************************************

  if (fFakeDataInput.empty()) return;

  if (fFakeDataInput.compare("MC") == 0){

    LOG(FIT)<<"Setting fake data from MC starting prediction." <<std::endl;
    UpdateRWEngine(fFakeVals);

    FitBase::GetRW()->Reconfigure();
    fSampleFCN->ReconfigureAllEvents();
    //fSampleFCN->SetFakeData("MC");

    UpdateRWEngine(fCurVals);

    LOG(FIT)<<"Set all data to fake MC predictions."<<std::endl;
  } else {
    //    fSampleFCN->SetFakeData(fFakeDataInput);
  }

  return;
}

/*
  Fitting Functions
*/
//*************************************
void minimizerRoutines::UpdateRWEngine(std::map<std::string,double>& updateVals){
//*************************************

  for (UInt_t i = 0; i < fParams.size(); i++){
    std::string name = fParams[i];

    if (updateVals.find(name) == updateVals.end()) continue;
    FitBase::GetRW()->SetDialValue(name,updateVals.at(name));
  }

  FitBase::GetRW()->Reconfigure();
  return;
}

//****************************
void minimizerRoutines::Run(){
//*************

  for (UInt_t i = 0; i < fRoutines.size(); i++){

    std::string routine = fRoutines.at(i);

    LOG(FIT)<<"Running Routine: "<<routine<<std::endl;
    if (routine.find("LowStat") != std::string::npos) LowStatRoutine(routine);
    else if (routine.find("PlotLimits") != std::string::npos) PlotLimits();
    else if (routine.find("ErrorBands") != std::string::npos) GenerateErrorBands();
    else RunFitRoutine(routine);

  }

  return;
}

//*************************************
void minimizerRoutines::RunFitRoutine(std::string routine){
//*************************************

  // Run Fix at Limit before fitter setup if required
  if (routine == "FixAtLim"){ FixAtLimit(); return; }

  // set fitter at the current start values
  SetupFitter(routine);
  fOutputRootFile->cd();

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

    if (fMinimizer->NFree() > 0){
      std::cout << StatusMessage(fMinimizer->Minimize()) << std::endl;
      GetMinimizerState();
    }
  }

  // other otptions
  else if (!routine.compare("Chi2Scan1D")) Create1DScans();
  else if (!routine.compare("Chi2Scan2D")) Chi2Scan2D();
  else if (!routine.compare("Contour")) CreateContours();
}

//*************************************
void minimizerRoutines::GetMinimizerState(){
//*************************************

  LOG(FIT) << "Minimizer State: "<<std::endl;
  // Get X and Err
  const double *values = fMinimizer->X();
  const double *errors = fMinimizer->Errors();
  int ipar = 0;

  LOG(FIT) << "  #  " << left << setw(30) << "Parameter "
	   << " = "
	   << setw(10) << "Value" << " +- "
	   << setw(10) << "Error" << " "
	   << setw(8) << "Units" << " (Sigma Variation) "<<std::endl;

  // loop through parameters and get the values
  for (UInt_t i = 0; i < fParams.size(); i++){
    std::string systString = fParams.at(i);

    fCurVals.at(systString) = values[ipar];

    std::ostringstream curparstring;

    curparstring << " " << setw(2) << left
		 << ipar << ". "
		 << setw(30) << systString << " = ";
    std::string curunits = "";//rw->GetUnits(systString);
    std::ostringstream ss;

    curparstring << setw(10) << 0.0 ;//rw->ConvertSigmaToValue(systString, values[ipar]);

    if (fFixVals.at(systString)){

      curparstring << "    " << setw(10) << "    " << setw(8) << curunits << "  (Fixed)";
      fErrorVals.at(systString) = 0.0;

    } else {
      double err = 0.0; //rw->ConvertSigmaToValue(systString, values[ipar]+errors[ipar]) - rw->ConvertSigmaToValue(systString, values[ipar]);
      curparstring << " +- " << setw(10) << err;
      curparstring << " " << setw(8) << curunits;
      if (curunits.compare("Sig.")) curparstring <<  " (" << values[ipar] << " +- "<< errors[ipar] <<")";

      fErrorVals.at(systString) = errors[ipar];
    }

    LOG(FIT) << curparstring.str() <<std::endl;

    ipar++;
  }

  LOG(FIT)<<"------------"<<std::endl;

  return;
};

//*************************************
void minimizerRoutines::LowStatRoutine(std::string routine){
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
  SetupFCN();

  RunFitRoutine(trueroutine);

  FitPar::Config().SetParI("MAXEVENTS",maxevents);
  SetupFCN();

  FitPar::Config().SetParI("VERBOSITY",verbosity);
  return;
}

//*************************************
void minimizerRoutines::Create1DScans(){
//*************************************

  // At the current point create a 1D Scan for all parametes (Uncorrelated)
  for (UInt_t i = 0; i < fParams.size(); i++){

    if (fFixVals[fParams[i]]) continue;

    double scanmiddlepoint = fCurVals[fParams[i]];

    // Determine N points needed
    double limlow  = fMinVals[fParams[i]];
    double limhigh = fMaxVals[fParams[i]];
    double step    = fStepVals[fParams[i]];

    int npoints = int( fabs(limhigh - limlow)/(step+0.) );
    int count = 0;

    double* xvals;
    double* yvals;
    xvals = new double[npoints];
    yvals = new double[npoints];

    // Set Start Point
    fCurVals[fParams[i]] = limlow;
    UpdateRWEngine(fCurVals);
    fSampleFCN->ReconfigureAllEvents();

    // Loop over scan points
    while (fCurVals[fParams[i]] < limhigh){

      // Update Prediction
      UpdateRWEngine(fCurVals);
      fSampleFCN->ReconfigureAllEvents();

      // Fill Graph Points
      yvals[count] = fSampleFCN->GetLikelihood();
      xvals[count] = fCurVals[fParams[i]];

      std::cout<<"Current Vals = "<<fCurVals[fParams[i]]<<" = "<<yvals[count]<<std::endl;

      // Get Next par set
      fCurVals[fParams[i]] += step;
      count++;
    }

    // Save Graph
    TGraph* scanGraph = new TGraph(count, xvals, yvals);
    scanGraph->Write(fParams[i].c_str());

    // Reset Parameter
    fCurVals[fParams[i]] = scanmiddlepoint;

    delete scanGraph;
    delete xvals;
    delete yvals;
  }

  return;
}

//*************************************
void minimizerRoutines::Chi2Scan2D(){
  //*************************************

  // Scan I
  for (UInt_t i = 0; i < fParams.size(); i++){
    if (fFixVals[fParams[i]]) continue;

    // Scan J
    for (UInt_t j = 0; j < fParams.size(); j++){
      if (fFixVals[fParams[j]]) continue;

      if (fParams[i] == fParams[j]) continue;

      double scanmid_i = fCurVals[fParams[i]];
      double scanmid_j = fCurVals[fParams[j]];

      double limlow_i  = fMinVals[fParams[i]];
      double limhigh_i = fMaxVals[fParams[i]];
      double step_i    = fStepVals[fParams[i]];

      double limlow_j  = fMinVals[fParams[j]];
      double limhigh_j = fMaxVals[fParams[j]];
      double step_j    = fStepVals[fParams[j]];

      int npoints_i = int( fabs(limhigh_i - limlow_i)/(step_i+0.) ) + 1;
      int npoints_j = int( fabs(limhigh_j - limlow_j)/(step_j+0.) ) + 1;

      TH2D* contour = new TH2D("countour","countour",
                               npoints_i, limlow_i, limhigh_i,
                               npoints_j, limlow_j, limhigh_j );

      contour->GetXaxis()->SetTitle(fParams[i].c_str());
      contour->GetYaxis()->SetTitle(fParams[j].c_str());

      cout<<"Running scan for "<<fParams[i]<<" "<<fParams[j]<<endl;

      // Fill bins
      for (int x = 0; x < contour->GetNbinsX(); x++){

        // Set X Val
        fCurVals[fParams[i]] = contour->GetXaxis()->GetBinCenter(x+1);
        cout<<"Set Value i "<<fCurVals[fParams[i]]<<endl;

        // Loop Y
        for (int y = 0; y < contour->GetNbinsY(); y++){

          // Set Y Val
          fCurVals[fParams[j]] = contour->GetYaxis()->GetBinCenter(y+1);
          cout<<"Set Value j "<<fCurVals[fParams[j]]<<endl;

          // Reconfigure
          UpdateRWEngine(fCurVals);
          fSampleFCN->ReconfigureAllEvents();

          // Chi2
          double chi2 = fSampleFCN->GetLikelihood();

          // Fill Contour
          contour->SetBinContent(x+1,y+1, chi2);

          cout<<"Filling "<<x<<" "<<y<<" "<<chi2<<endl;

          fCurVals[fParams[j]] = scanmid_j;
        }

        fCurVals[fParams[i]] = scanmid_i;
        fCurVals[fParams[j]] = scanmid_j;
      }

      // Save contour
      contour->Write();
    }
  }

  return;
}



//*************************************
void minimizerRoutines::CreateContours(){
//*************************************

  // Use MINUIT for this if possible


  return;
}

//*************************************
void minimizerRoutines::FixAtLimit(){
//*************************************

  for (UInt_t i = 0; i < fParams.size(); i++){
    std::string systString = fParams.at(i);
    if (fFixVals[systString]) continue;

    double curVal = fCurVals.at(systString);
    double minVal = fMinVals.at(systString);
    double maxVal = fMinVals.at(systString);

    if (fabs(curVal - minVal) < 0.0001){
      fCurVals.at(systString) = minVal;
    }

    if (fabs(maxVal - curVal) < 0.0001){
      fCurVals.at(systString) = maxVal;
    }
  }

  return;
}


/*
  Write Functions
*/
//*************************************
void minimizerRoutines::SaveFitterOutput(std::string dir){
//*************************************

  if (!dir.empty()){
    TDirectory* nominalDIR =(TDirectory*) fOutputRootFile->mkdir(dir.c_str());
    nominalDIR->cd();
  }

  if (fMinimizer){
    SaveMinimizerState();
  }

  SaveCurrentState();

  fOutputRootFile->cd();

  return;
}

//*************************************
void minimizerRoutines::SaveMinimizerState(){
//*************************************

  if (!fMinimizer) return;

  fSampleFCN->WriteIterationTree();
  
  // Get Vals and Errors
  GetMinimizerState();
  fOutputRootFile->cd();

  // Save tree with fit status
  std::vector<std::string> nameVect;
  std::vector<double>      valVect;
  std::vector<double>      errVect;
  std::vector<double>      minVect;
  std::vector<double>      maxVect;
  std::vector<double>      startVect;
  std::vector<int>      endfixVect;
  std::vector<int>      startfixVect;

  //  int NFREEPARS = fMinimizer->NFree();
  int NPARS = fMinimizer->NDim();

  int ipar = 0;
  // Dial Vals
  for (UInt_t i = 0; i < fParams.size(); i++){
    std::string name = fParams.at(i);

    nameVect    .push_back( name );

    cout << name << endl;
    valVect     .push_back( fCurVals.at(name)   );
    errVect     .push_back( fErrorVals.at(name) );
    minVect     .push_back( fMinVals.at(name)   );
    maxVect     .push_back( fMaxVals.at(name)   );

    cout << "MAX " << name << endl;
    startVect   .push_back( fStartVals.at(name) );
    endfixVect  .push_back( fFixVals.at(name)      );
    startfixVect.push_back( fStartFixVals.at(name) );

    ipar++;
  }



  int NFREE = fMinimizer->NFree();
  int NDIM  = fMinimizer->NDim();

  double CHI2 = fSampleFCN->GetLikelihood();
  int NBINS = fSampleFCN->GetNDOF();
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

  // Save TString for fCardFile

  // Save fit_status plot
  TH1D statusplot = TH1D("fit_status","fit_status",8,0,8);
  std::string fit_labels[8] = {"status", "cov_status",  \
			       "maxiter", "maxfunc",	\
			       "iter",    "func",	\
			       "precision", "tolerance"};
  double fit_vals[8];
  fit_vals[0] = fMinimizer->Status() + 0.;
  fit_vals[1] = fMinimizer->CovMatrixStatus() + 0.;
  fit_vals[2] = fMinimizer->MaxIterations() + 0.;
  fit_vals[3] = fMinimizer->MaxFunctionCalls()+ 0.;
  fit_vals[4] = fMinimizer->NIterations() + 0.;
  fit_vals[5] = fMinimizer->NCalls() + 0.;
  fit_vals[6] = fMinimizer->Precision() + 0.;
  fit_vals[7] = fMinimizer->Tolerance() + 0.;

  for (int i = 0; i < 8; i++){
    statusplot.SetBinContent(i+1, fit_vals[i]);
    statusplot.GetXaxis()->SetBinLabel(i+1, fit_labels[i].c_str());
  }

  statusplot.Write();

  if (fMinimizer->NFree() == 0) return;
  
  // Sort Covariances
  SetupCovariance();

  if (fMinimizer->CovMatrixStatus() > 0){

    for (int i = 0; i < fCovar->GetNbinsX(); i++){
      for (int j = 0; j < fCovar->GetNbinsY(); j++){
	fCovar->SetBinContent(i+1,j+1, fMinimizer->CovMatrix(i,j));
      }
    }

    int freex = 0;
    int freey = 0;

    for (int i = 0; i < fCovar->GetNbinsX(); i++){

      freey = 0;
      if (fMinimizer->IsFixedVariable(i)) continue;

      for (int j = 0; j < fCovar->GetNbinsY(); j++){
	if (fMinimizer->IsFixedVariable(j)) continue;

	fCovarFree->SetBinContent(freex+1,freey+1, fMinimizer->CovMatrix(i,j));
	freey++;

      }
      freex++;
    }
  }

  fCorrel = PlotUtils::GetCorrelationPlot(fCovar,"correlation");
  fDecomp = PlotUtils::GetDecompPlot(fCovar,"decomposition");

  if (NFREE > 0)fCorrelFree = PlotUtils::GetCorrelationPlot(fCovarFree, "correlation_free");
  if (NFREE > 0)fDecompFree = PlotUtils::GetDecompPlot(fCovarFree,"decomposition_free");

  if (fCovar) fCovar->Write();
  if (fCovarFree) fCovarFree->Write();
  if (fCorrel) fCorrel->Write();
  if (fCorrelFree) fCorrelFree->Write();
  if (fDecomp) fDecomp->Write();
  if (fDecompFree) fDecompFree->Write();

  return;
}

//*************************************
void minimizerRoutines::SaveCurrentState(std::string subdir){
//*************************************

  LOG(FIT)<<"Saving current FCN predictions" <<std::endl;

  fOutputRootFile->cd();
  FitBase::GetRW()->Reconfigure();
  fSampleFCN->ReconfigureAllEvents();

  if (!subdir.empty()){
    TDirectory* nominalDIR =(TDirectory*) fOutputRootFile->mkdir("nominal");
    nominalDIR->cd();
  }

  fSampleFCN->Write();

  fOutputRootFile->cd();

  return;
}

//*************************************
void minimizerRoutines::SaveNominal(){
//*************************************

  LOG(FIT)<<"Saving Neut Nominal Predictions (be cautious with this)" <<std::endl;
  FitBase::GetRW()->Reconfigure();
  SaveCurrentState("nominal");
  
};

//*************************************
void minimizerRoutines::SavePrefit(){
//*************************************

  LOG(FIT)<<"Saving Prefit Predictions"<<std::endl;
  UpdateRWEngine(fStartVals);
  SaveCurrentState("prefit");
  UpdateRWEngine(fCurVals);

};


/*
  MISC Functions
*/
//*************************************
int minimizerRoutines::GetStatus(){
//*************************************

  return 0;
}

//*************************************
void minimizerRoutines::SetupCovariance(){
//*************************************

  // Remove covares if they exist
  if (fCovar) delete fCovar;
  if (fCovarFree) delete fCovarFree;
  if (fCorrel) delete fCorrel;
  if (fCorrelFree) delete fCorrelFree;
  if (fDecomp) delete fDecomp;
  if (fDecompFree) delete fDecompFree;

  int NFREE = 0;
  int NDIM = 0;

  // Get NFREE from min or from vals (for cases when doing throws)
  if (fMinimizer){
    NFREE = fMinimizer->NFree();
    NDIM  = fMinimizer->NDim();
  } else {
    NDIM = fParams.size();
    for (UInt_t i = 0; i < fParams.size(); i++){
      if (!fFixVals[fParams[i]]) NFREE++;
    }
  }

  if (NDIM == 0) return;

  fCovar = new TH2D("covariance","covariance",NDIM,0,NDIM,NDIM,0,NDIM);
  if (NFREE > 0){
    fCovarFree = new TH2D("covariance_free",
			      "covariance_free",
			      NFREE,0,NFREE,
			      NFREE,0,NFREE);
  }

  // Set Bin Labels
  int countall = 0;
  int countfree = 0;
  for (UInt_t i = 0; i < fParams.size(); i++){

    fCovar->GetXaxis()->SetBinLabel(countall+1,fParams[i].c_str());
    fCovar->GetYaxis()->SetBinLabel(countall+1,fParams[i].c_str());
    countall++;

    if (!fFixVals[fParams[i]] and NFREE > 0){
      fCovarFree->GetXaxis()->SetBinLabel(countfree+1,fParams[i].c_str());
      fCovarFree->GetYaxis()->SetBinLabel(countfree+1,fParams[i].c_str());
      countfree++;
    }
  }

  fCorrel = PlotUtils::GetCorrelationPlot(fCovar,"correlation");
  fDecomp = PlotUtils::GetDecompPlot(fCovar,"decomposition");

  if (NFREE > 0)fCorrelFree = PlotUtils::GetCorrelationPlot(fCovarFree, "correlation_free");
  if (NFREE > 0)fDecompFree = PlotUtils::GetDecompPlot(fCovarFree,"decomposition_free");

  return;
};

//*************************************
void minimizerRoutines::ThrowCovariance(bool uniformly){
//*************************************

  std::vector<double> rands;

  if (!fDecompFree) {
    ERR(WRN) << "Trying to throw 0 free parameters"<<std::endl;
    return;
  }

  for (Int_t i = 0; i < fDecompFree->GetNbinsX(); i++){
    rands.push_back(gRandom->Gaus(0.0,1.0));
  }

  for (UInt_t i = 0; i < fParams.size(); i++){
    fThrownVals[fParams[i]] = fCurVals[fParams[i]];
  }

  for (Int_t i = 0; i < fDecompFree->GetNbinsX(); i++){

    std::string parname = std::string(fDecompFree->GetXaxis()->GetBinLabel(i+1));
    double mod = 0.0;

    if (!uniformly){
      for (Int_t j = 0; j < fDecompFree->GetNbinsY(); j++){
	mod += rands[j] * fDecompFree->GetBinContent(j+1,i+1);
      }
    }

    if (fCurVals.find(parname) != fCurVals.end()) {

      if (uniformly) fThrownVals[parname] = gRandom->Uniform(fMinVals[parname],fMaxVals[parname]);
      else {  fThrownVals[parname] = 	  fCurVals[parname] + mod; }

    }
  }

  for (UInt_t i = 0; i < fParams.size(); i++){
    if (fThrownVals[fParams[i]] < fMinVals[fParams[i]]) fThrownVals[fParams[i]] = fMinVals[fParams[i]];
    if (fThrownVals[fParams[i]] > fMaxVals[fParams[i]]) fThrownVals[fParams[i]] = fMaxVals[fParams[i]];
  }

  return;
};

//*************************************
void minimizerRoutines::GenerateErrorBands(){
//*************************************

  TDirectory* errorDIR = (TDirectory*) fOutputRootFile->mkdir("error_bands");
  errorDIR->cd();

  TFile* tempfile = new TFile((fOutputFile + ".throws.root").c_str(),"RECREATE");
  tempfile->cd();
  int nthrows = FitPar::Config().GetParI("error_throws");

  UpdateRWEngine(fCurVals);
  fSampleFCN->ReconfigureAllEvents();

  TDirectory* nominal = (TDirectory*) tempfile->mkdir("nominal");
  nominal->cd();
  fSampleFCN->Write();


  TDirectory* outnominal = (TDirectory*) fOutputRootFile->mkdir("nominal_throw");
  outnominal->cd();
  fSampleFCN->Write();


  errorDIR->cd();
  TTree* parameterTree = new TTree("throws","throws");
  double chi2;
  for (UInt_t i = 0; i < fParams.size(); i++)
    parameterTree->Branch(fParams[i].c_str(), &fThrownVals[fParams[i]], (fParams[i] + "/D").c_str());
  parameterTree->Branch("chi2",&chi2,"chi2/D");


  bool uniformly = FitPar::Config().GetParB("error_uniform");

  // Run Throws and save
  for (Int_t i = 0; i < nthrows; i++){

    TDirectory* throwfolder = (TDirectory*)tempfile->mkdir(Form("throw_%i",i));
    throwfolder->cd();

    ThrowCovariance(uniformly);
    UpdateRWEngine(fThrownVals);
    fSampleFCN->ReconfigureAllEvents();
    fSampleFCN->Write();
    chi2 = fSampleFCN->GetLikelihood();

    parameterTree->Fill();
  }

  errorDIR->cd();
  fDecompFree->Write();
  fCovarFree->Write();
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
