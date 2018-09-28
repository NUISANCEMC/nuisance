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
#include "SystematicRoutines.h"

void SystematicRoutines::Init(){

  fInputFile = "";
  fInputRootFile = NULL;

  fOutputFile = "";
  fOutputRootFile = NULL;

  fCovar  = fCovarFree  = NULL;
  fCorrel = fCorrelFree = NULL;
  fDecomp = fDecompFree = NULL;

  fStrategy = "ErrorBands";
  fRoutines.clear();
  fRoutines.push_back("ErrorBands");

  fCardFile = "";

  fFakeDataInput = "";

  fSampleFCN    = NULL;

  fAllowedRoutines = ("ErrorBands,PlotLimits");

};

SystematicRoutines::~SystematicRoutines(){
};

SystematicRoutines::SystematicRoutines(int argc, char* argv[]){

  // Initialise Defaults
  Init();
  nuisconfig configuration = Config::Get();

  // Default containers
  std::string cardfile = "";
  std::string maxevents = "-1";
  int errorcount = 0;
  int verbocount = 0;
  std::vector<std::string> xmlcmds;
  std::vector<std::string> configargs;
  fNThrows = 250;
  fStartThrows = 0;
  fThrowString = "";
  // Make easier to handle arguments.
  std::vector<std::string> args = GeneralUtils::LoadCharToVectStr(argc, argv);
  ParserUtils::ParseArgument(args, "-c", fCardFile, true);
  ParserUtils::ParseArgument(args, "-o", fOutputFile, false, false);
  ParserUtils::ParseArgument(args, "-n", maxevents, false, false);
  ParserUtils::ParseArgument(args, "-f", fStrategy, false, false);
  ParserUtils::ParseArgument(args, "-d", fFakeDataInput, false, false);
  ParserUtils::ParseArgument(args, "-s", fStartThrows, false, false);
  ParserUtils::ParseArgument(args, "-t", fNThrows, false, false);
  ParserUtils::ParseArgument(args, "-p", fThrowString, false, false);
  ParserUtils::ParseArgument(args, "-i", xmlcmds);
  ParserUtils::ParseArgument(args, "-q", configargs);
  ParserUtils::ParseCounter(args, "e", errorcount);
  ParserUtils::ParseCounter(args, "v", verbocount);
  ParserUtils::CheckBadArguments(args);

  // Add extra defaults if none given
  if (fCardFile.empty() and xmlcmds.empty()) {
    ERR(FTL) << "No input supplied!" << std::endl;
    throw;
  }

  if (fOutputFile.empty() and !fCardFile.empty()) {
    fOutputFile = fCardFile + ".root";
    ERR(WRN) << "No output supplied so saving it to: " << fOutputFile << std::endl;

  } else if (fOutputFile.empty()) {
    ERR(FTL) << "No output file or cardfile supplied!" << std::endl;
    throw;
  }

  // Configuration Setup =============================

  // Check no comp key is available
  if (Config::Get().GetNodes("nuiscomp").empty()) {
    fCompKey = Config::Get().CreateNode("nuiscomp");
  } else {
    fCompKey = Config::Get().GetNodes("nuiscomp")[0];
  }

  if (!fCardFile.empty())   fCompKey.Set("cardfile", fCardFile);
  if (!fOutputFile.empty()) fCompKey.Set("outputfile", fOutputFile);
  if (!fStrategy.empty())   fCompKey.Set("strategy", fStrategy);

  // Load XML Cardfile
  configuration.LoadSettings( fCompKey.GetS("cardfile"), "");

  // Add Config Args
  for (size_t i = 0; i < configargs.size(); i++) {
    configuration.OverrideConfig(configargs[i]);
  }
  if (maxevents.compare("-1")){
    configuration.OverrideConfig("MAXEVENTS=" + maxevents);
  }

  // Finish configuration XML
  configuration.FinaliseSettings(fCompKey.GetS("outputfile") + ".xml");

  // Add Error Verbo Lines
  verbocount += Config::GetParI("VERBOSITY");
  errorcount += Config::GetParI("ERROR");
  std::cout << "[ NUISANCE ]: Setting VERBOSITY=" << verbocount << std::endl;
  std::cout << "[ NUISANCE ]: Setting ERROR=" << errorcount << std::endl;
  SETVERBOSITY(verbocount);
  
  // Proper Setup
  if (fStrategy.find("ErrorBands") != std::string::npos ||
      fStrategy.find("MergeErrors") != std::string::npos){
    fOutputRootFile = new TFile(fCompKey.GetS("outputfile").c_str(), "RECREATE");    
  }

  //  fOutputRootFile = new TFile(fCompKey.GetS("outputfile").c_str(), "RECREATE");
  SetupSystematicsFromXML();

  SetupCovariance();
  SetupRWEngine();
  SetupFCN();
  GetCovarFromFCN();

  //  Run();

  return;
};

void SystematicRoutines::SetupSystematicsFromXML(){

  LOG(FIT) << "Setting up nuismin" << std::endl;

  // Setup Parameters ------------------------------------------
  std::vector<nuiskey> parkeys = Config::QueryKeys("parameter");
  if (!parkeys.empty()) {
    LOG(FIT) << "Number of parameters :  " << parkeys.size() << std::endl;
  }

  for (size_t i = 0; i < parkeys.size(); i++) {
    nuiskey key = parkeys.at(i);

    // Check for type,name,nom
    if (!key.Has("type")) {
      ERR(FTL) << "No type given for parameter " << i << std::endl;
      throw;
    } else if (!key.Has("name")) {
      ERR(FTL) << "No name given for parameter " << i << std::endl;
      throw;
    } else if (!key.Has("nominal")) {
      ERR(FTL) << "No nominal given for parameter " << i << std::endl;
      throw;
    }

    // Get Inputs
    std::string partype = key.GetS("type");
    std::string parname = key.GetS("name");
    double parnom  = key.GetD("nominal");
    double parlow  = parnom - 1;
    double parhigh = parnom + 1;
    double parstep = 1;


    // Override if state not given
    if (!key.Has("state")){
      key.SetS("state","FIX");
    }

    std::string parstate = key.GetS("state");

    // Extra limits
    if (key.Has("low")) {
      parlow  = key.GetD("low");
      parhigh = key.GetD("high");
      parstep = key.GetD("step");

      LOG(FIT) << "Read " << partype << " : "
               << parname << " = "
               << parnom << " : "
               << parlow << " < p < " << parhigh
               << " : " << parstate << std::endl;
    } else {
      LOG(FIT) << "Read " << partype << " : "
               << parname << " = "
               << parnom << " : "
               << parstate << std::endl;
    }

    // Run Parameter Conversion if needed
    if (parstate.find("ABS") != std::string::npos) {
      parnom  = FitBase::RWAbsToSigma( partype, parname, parnom  );
      parlow  = FitBase::RWAbsToSigma( partype, parname, parlow  );
      parhigh = FitBase::RWAbsToSigma( partype, parname, parhigh );
      parstep = FitBase::RWAbsToSigma( partype, parname, parstep );
    } else if (parstate.find("FRAC") != std::string::npos) {
      parnom  = FitBase::RWFracToSigma( partype, parname, parnom  );
      parlow  = FitBase::RWFracToSigma( partype, parname, parlow  );
      parhigh = FitBase::RWFracToSigma( partype, parname, parhigh );
      parstep = FitBase::RWFracToSigma( partype, parname, parstep );
    }

    // Push into vectors
    fParams.push_back(parname);

    fTypeVals[parname]  = FitBase::ConvDialType(partype);;
    fStartVals[parname] = parnom;
    fCurVals[parname]   = parnom;

    fErrorVals[parname] = 0.0;

    fStateVals[parname]    = parstate;
    bool fixstate = parstate.find("FIX") != std::string::npos;
    fFixVals[parname]      = fixstate;
    fStartFixVals[parname] = fFixVals[parname];

    fMinVals[parname]  = parlow;
    fMaxVals[parname]  = parhigh;
    fStepVals[parname] = parstep;

  }

  // Setup Samples ----------------------------------------------
  std::vector<nuiskey> samplekeys =  Config::QueryKeys("sample");
  if (!samplekeys.empty()) {
    LOG(FIT) << "Number of samples : " << samplekeys.size() << std::endl;
  }

  for (size_t i = 0; i < samplekeys.size(); i++) {
    nuiskey key = samplekeys.at(i);

    // Get Sample Options
    std::string samplename = key.GetS("name");
    std::string samplefile = key.GetS("input");

    std::string sampletype =
      key.Has("type") ? key.GetS("type") : "DEFAULT";

    double samplenorm =
      key.Has("norm") ? key.GetD("norm") : 1.0;

    // Print out
    LOG(FIT) << "Read sample info " << i << " : "
             << samplename << std::endl
             << "\t\t input -> " << samplefile  << std::endl
             << "\t\t state -> " << sampletype << std::endl
             << "\t\t norm  -> " << samplenorm << std::endl;

    // If FREE add to parameters otherwise continue
    if (sampletype.find("FREE") == std::string::npos) {
      continue;
    }

    // Form norm dial from samplename + sampletype + "_norm";
    std::string normname = samplename + "_norm";

    // Check normname not already present
    if (fTypeVals.find(normname) != fTypeVals.end()) {
      continue;
    }

    // Add new norm dial to list if its passed above checks
    fParams.push_back(normname);

    fTypeVals[normname] = kNORM;
    fStateVals[normname] = sampletype;
    fCurVals[normname] = samplenorm;

    fErrorVals[normname] = 0.0;

    fMinVals[normname]  = 0.1;
    fMaxVals[normname]  = 10.0;
    fStepVals[normname] = 0.5;

    bool state = sampletype.find("FREE") == std::string::npos;
    fFixVals[normname]      = state;
    fStartFixVals[normname] = state;
  }

  // Setup Fake Parameters -----------------------------
  std::vector<nuiskey> fakekeys = Config::QueryKeys("fakeparameter");
  if (!fakekeys.empty()) {
    LOG(FIT) << "Number of fake parameters : " << fakekeys.size() << std::endl;
  }

  for (size_t i = 0; i < fakekeys.size(); i++) {
    nuiskey key = fakekeys.at(i);

    // Check for type,name,nom
    if (!key.Has("name")) {
      ERR(FTL) << "No name given for fakeparameter " << i << std::endl;
      throw;
    } else if (!key.Has("nom")) {
      ERR(FTL) << "No nominal given for fakeparameter " << i << std::endl;
      throw;
    }

    // Get Inputs
    std::string parname = key.GetS("name");
    double parnom  = key.GetD("nom");

    // Push into vectors
    fFakeVals[parname] = parnom;
  }
}


/*
  Setup Functions
*/
//*************************************
void SystematicRoutines::SetupRWEngine(){
//*************************************

  for (UInt_t i = 0; i < fParams.size(); i++){
    std::string name = fParams[i];
    FitBase::GetRW() -> IncludeDial(name, fTypeVals.at(name) );
  }
  UpdateRWEngine(fStartVals);

  return;
}

//*************************************
void SystematicRoutines::SetupFCN(){
//*************************************

  LOG(FIT)<<"Making the jointFCN"<<std::endl;
  if (fSampleFCN) delete fSampleFCN;
  fSampleFCN = new JointFCN(fOutputRootFile);
  SetFakeData();

  return;
}


//*************************************
void SystematicRoutines::SetFakeData(){
//*************************************

  if (fFakeDataInput.empty()) return;

  if (fFakeDataInput.compare("MC") == 0){

    LOG(FIT)<<"Setting fake data from MC starting prediction." <<std::endl;
    UpdateRWEngine(fFakeVals);

    FitBase::GetRW()->Reconfigure();
    fSampleFCN->ReconfigureAllEvents();
    fSampleFCN->SetFakeData("MC");

    UpdateRWEngine(fCurVals);

    LOG(FIT)<<"Set all data to fake MC predictions."<<std::endl;
  } else {
    fSampleFCN->SetFakeData(fFakeDataInput);
  }

  return;
}

//*****************************************
void SystematicRoutines::GetCovarFromFCN(){
//*****************************************
  LOG(FIT) << "Loading ParamPull objects from FCN to build covar" << std::endl;

  // Make helperstring
  std::ostringstream helperstr;

  // Keep track of what is being thrown
  std::map<std::string, std::string> dialthrowhandle;

  // Get Covariance Objects from FCN
  std::list<ParamPull*> inputpulls = fSampleFCN->GetPullList();
  for (PullListConstIter iter = inputpulls.begin();
       iter != inputpulls.end(); iter++){

    ParamPull* pull = (*iter);

    if (pull->GetType().find("THROW")){
      fInputThrows.push_back(pull);
      fInputCovar.push_back(pull->GetFullCovarMatrix());
      fInputDials.push_back(pull->GetDataHist());

      LOG(FIT) << "Read ParamPull: " << pull->GetName() << " " << pull->GetType() << std::endl;
    }

    TH1D dialhist = pull->GetDataHist();
    TH1D minhist  = pull->GetMinHist();
    TH1D maxhist  = pull->GetMaxHist();
    TH1I typehist = pull->GetDialTypes();

    for (int i = 0; i < dialhist.GetNbinsX(); i++){
      std::string name = std::string(dialhist.GetXaxis()->GetBinLabel(i+1));
      dialthrowhandle[name] = pull->GetName();

      if (fCurVals.find(name) == fCurVals.end()){

      	// Add to Containers
      	fParams.push_back(name);
      	fCurVals[name]      = dialhist.GetBinContent(i+1);
      	fStartVals[name]    = dialhist.GetBinContent(i+1);
      	fMinVals[name]      = minhist.GetBinContent(i+1);
      	fMaxVals[name]      = maxhist.GetBinContent(i+1);
      	fStepVals[name]     = 1.0;
      	fFixVals[name]      = false;
      	fStartFixVals[name] = false;
      	fTypeVals[name]     = typehist.GetBinContent(i+1);
      	fStateVals[name]    = "FREE" + pull->GetType();

      	// Maker Helper
      	helperstr << std::string(16, ' ' ) << FitBase::ConvDialType(fTypeVals[name]) << " "
      		  << name << " " << fMinVals[name] << " "
      		  << fMaxVals[name] << " " << fStepVals[name] << " " << fStateVals[name]
		  << std::endl;
      }
    }
  }

  // Check if no throws given
  if (fInputThrows.empty()){

    ERR(WRN) << "No covariances given to nuissyst" << std::endl;
    ERR(WRN) << "Pushing back an uncorrelated gaussian throw error for each free parameter using step size" << std::endl;

    for (UInt_t i = 0; i < fParams.size(); i++){
      std::string syst     = fParams[i];
      if (fFixVals[syst]) continue;

      // Make Terms
      std::string name     = syst + "_pull";

      std::ostringstream pullterm;
      pullterm << "DIAL:" << syst << ";"
	       << fStartVals[syst] << ";"
	       << fStepVals[syst];

      std::string type = "GAUSTHROW/NEUT";

      // Push Back Pulls
      ParamPull* pull = new ParamPull( name, pullterm.str(), type );
      fInputThrows.push_back(pull);
      fInputCovar.push_back(pull->GetFullCovarMatrix());
      fInputDials.push_back(pull->GetDataHist());

      // Print Whats added
      ERR(WRN) << "Added ParamPull : " << name << " " << pullterm.str() << " " << type << std::endl;

      // Add helper string for future fits
      helperstr << std::string(16, ' ' ) << "covar " << name << " " << pullterm.str() << " " << type << std::endl;

      // Keep Track of Throws
      dialthrowhandle[syst] = pull->GetName();
    }
  }

  // Print Helper String
  if (!helperstr.str().empty()){
    LOG(FIT) << "To remove these statements in future studies, add the lines below to your card:" << std::endl;
    // Can't use the logger properly because this can be multi-line. Use cout and added spaces to look better!
    std::cout << helperstr.str();
    sleep(2);
  }



  // Print Throw State
  for (UInt_t i = 0; i < fParams.size(); i++){
    std::string syst = fParams[i];
    if (dialthrowhandle.find(syst) != dialthrowhandle.end()){
      LOG(FIT) << "Dial " << i << ". " << setw(40) << syst << " = THROWING with " << dialthrowhandle[syst] << std::endl;
    } else {
      LOG(FIT) << "Dial " << i << ". " << setw(40) << syst << " = FIXED" << std::endl;
    }
  }

  // Pause anyway
  sleep(1);
  return;
}




/*
  Fitting Functions
*/
//*************************************
void SystematicRoutines::UpdateRWEngine(std::map<std::string,double>& updateVals){
//*************************************

  for (UInt_t i = 0; i < fParams.size(); i++){
    std::string name = fParams[i];

    if (updateVals.find(name) == updateVals.end()) continue;
    FitBase::GetRW()->SetDialValue(name,updateVals.at(name));
  }

  FitBase::GetRW()->Reconfigure();
  return;
}

//*************************************
void SystematicRoutines::PrintState(){
//*************************************
  LOG(FIT)<<"------------"<<std::endl;

  // Count max size
  int maxcount = 0;
  for (UInt_t i = 0; i < fParams.size(); i++){
    maxcount = max(int(fParams[i].size()), maxcount);
  }

  // Header
  LOG(FIT) << " #    " << left << setw(maxcount) << "Parameter "
	   << " = "
	   << setw(10) << "Value"     << " +- "
	   << setw(10) << "Error"     << " "
	   << setw(8)  << "(Units)"   << " "
	   << setw(10) << "Conv. Val" << " +- "
	   << setw(10) << "Conv. Err" << " "
	   << setw(8)  << "(Units)"   << std::endl;

  // Parameters
  for (UInt_t i = 0; i < fParams.size(); i++){
    std::string syst = fParams.at(i);

    std::string typestr  = FitBase::ConvDialType(fTypeVals[syst]);
    std::string curunits = "(sig.)";
    double      curval   = fCurVals[syst];
    double      curerr   = fErrorVals[syst];

    if (fStateVals[syst].find("ABS") != std::string::npos){
      curval = FitBase::RWSigmaToAbs(typestr, syst, curval);
      curerr = (FitBase::RWSigmaToAbs(typestr, syst, curerr) -
		FitBase::RWSigmaToAbs(typestr, syst, 0.0));
      curunits = "(Abs.)";
    } else if (fStateVals[syst].find("FRAC") != std::string::npos){
      curval = FitBase::RWSigmaToFrac(typestr, syst, curval);
      curerr = (FitBase::RWSigmaToFrac(typestr, syst, curerr) -
		FitBase::RWSigmaToFrac(typestr, syst, 0.0));
      curunits = "(Frac)";
    }

    std::string convunits = "(" + FitBase::GetRWUnits(typestr, syst) + ")";
    double      convval   = FitBase::RWSigmaToAbs(typestr, syst, curval);
    double      converr   = (FitBase::RWSigmaToAbs(typestr, syst, curerr) -
			     FitBase::RWSigmaToAbs(typestr, syst, 0.0));

    std::ostringstream curparstring;

    curparstring << " " << setw(3) << left
		 << i << ". "
		 << setw(maxcount) << syst << " = "
		 << setw(10) << curval     << " +- "
		 << setw(10) << curerr     << " "
		 << setw(8)  << curunits   << " "
                 << setw(10) << convval    << " +- "
                 << setw(10) << converr    << " "
                 << setw(8)  << convunits;


    LOG(FIT) << curparstring.str() << std::endl;
  }

  LOG(FIT)<<"------------"<<std::endl;
  double like = fSampleFCN->GetLikelihood();
  LOG(FIT) << std::left << std::setw(46) << "Likelihood for JointFCN: " << like << std::endl;
  LOG(FIT)<<"------------"<<std::endl;
}



/*
  Write Functions
*/
//*************************************
void SystematicRoutines::SaveResults(){
//*************************************
  if (!fOutputRootFile)
    fOutputRootFile = new TFile(fCompKey.GetS("outputfile").c_str(), "RECREATE");  

  fOutputRootFile->cd();

  SaveCurrentState();

}

//*************************************
void SystematicRoutines::SaveCurrentState(std::string subdir){
//*************************************

  LOG(FIT)<<"Saving current full FCN predictions" <<std::endl;

  // Setup DIRS
  TDirectory* curdir = gDirectory;
  if (!subdir.empty()){
    TDirectory* newdir =(TDirectory*) gDirectory->mkdir(subdir.c_str());
    newdir->cd();
  }

  FitBase::GetRW()->Reconfigure();
  fSampleFCN->ReconfigureAllEvents();
  fSampleFCN->Write();

  // Change back to current DIR
  curdir->cd();

  return;
}

//*************************************
void SystematicRoutines::SaveNominal(){
//*************************************
  if (!fOutputRootFile)
    fOutputRootFile = new TFile(fCompKey.GetS("outputfile").c_str(), "RECREATE");

  fOutputRootFile->cd();

  LOG(FIT)<<"Saving Nominal Predictions (be cautious with this)" <<std::endl;
  FitBase::GetRW()->Reconfigure();
  SaveCurrentState("nominal");

};

//*************************************
void SystematicRoutines::SavePrefit(){
//*************************************
  if (!fOutputRootFile)
    fOutputRootFile = new TFile(fCompKey.GetS("outputfile").c_str(), "RECREATE");

  fOutputRootFile->cd();

  LOG(FIT)<<"Saving Prefit Predictions"<<std::endl;
  UpdateRWEngine(fStartVals);
  SaveCurrentState("prefit");
  UpdateRWEngine(fCurVals);

};


/*
  MISC Functions
*/
//*************************************
int SystematicRoutines::GetStatus(){
//*************************************

  return 0;
}

//*************************************
void SystematicRoutines::SetupCovariance(){
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
  NDIM = fParams.size();
  for (UInt_t i = 0; i < fParams.size(); i++){
    if (!fFixVals[fParams[i]]) NFREE++;
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
void SystematicRoutines::ThrowCovariance(bool uniformly){
//*************************************

  // Set fThrownVals to all values in currentVals
  for (UInt_t i = 0; i < fParams.size(); i++){
    std::string name = fParams.at(i);
    fThrownVals[name] = fCurVals[name];
  }

  for (PullListConstIter iter = fInputThrows.begin();
       iter != fInputThrows.end(); iter++){
    ParamPull* pull = *iter;

    pull->ThrowCovariance();
    TH1D dialhist = pull->GetDataHist();

    for (int i = 0; i < dialhist.GetNbinsX(); i++){
      std::string name = std::string(dialhist.GetXaxis()->GetBinLabel(i+1));
      if (fCurVals.find(name) != fCurVals.end()){
	fThrownVals[name] = dialhist.GetBinContent(i+1);
      }
    }

    // Reset throw incase pulls are calculated.
    pull->ResetToy();

  }

  return;
};

//*************************************
void SystematicRoutines::PlotLimits(){
//*************************************
  std::cout << "Plotting Limits" << std::endl;
  if (!fOutputRootFile)
    fOutputRootFile = new TFile(fCompKey.GetS("outputfile").c_str(), "RECREATE");

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
    std::string syst = fParams[i];
    std::cout << "Starting Param " << syst << std::endl;
    if (fFixVals[syst]) continue;

    // Loop Downwards
    while (fCurVals[syst] > fMinVals[syst]){
      fCurVals[syst] = fCurVals[syst] - fStepVals[syst];

      // Check Limit
      if (fCurVals[syst] < fMinVals[syst])
	fCurVals[syst] = fMinVals[syst];

      // Check folder exists
      std::string curvalstring = std::string( Form( (syst + "_%f").c_str(), fCurVals[syst] ) );
      if (std::find(allfolders.begin(), allfolders.end(), curvalstring) != allfolders.end())
	break;

      // Make new folder for variation
      TDirectory* minfolder = (TDirectory*) limfolder->mkdir(Form( (syst + "_%f").c_str(), fCurVals[syst] ) );
      minfolder->cd();

      allfolders.push_back(curvalstring);

      // Update Iterations
      double *vals = FitUtils::GetArrayFromMap( fParams, fCurVals );
      fSampleFCN->DoEval( vals );
      delete vals;

      // Save to folder
      fSampleFCN->Write();
    }

    // Reset before next loop
    fCurVals[syst] = fStartVals[syst];

    // Loop Upwards now
    while (fCurVals[syst] < fMaxVals[syst]){
      fCurVals[syst] = fCurVals[syst] + fStepVals[syst];

      // Check Limit
      if (fCurVals[syst] > fMaxVals[syst])
	fCurVals[syst] = fMaxVals[syst];

      // Check folder exists
      std::string curvalstring = std::string( Form( (syst + "_%f").c_str(), fCurVals[syst] ) );
      if (std::find(allfolders.begin(), allfolders.end(), curvalstring) != allfolders.end())
	break;

      // Make new folder
      TDirectory* maxfolder = (TDirectory*) limfolder->mkdir(Form( (syst + "_%f").c_str(), fCurVals[syst] ) );
      maxfolder->cd();

      allfolders.push_back(curvalstring);

      // Update Iterations
      double *vals = FitUtils::GetArrayFromMap( fParams, fCurVals );
      fSampleFCN->DoEval( vals );
      delete vals;

      // Save to file
      fSampleFCN->Write();
    }

    // Reset before leaving
    fCurVals[syst] = fStartVals[syst];
    UpdateRWEngine(fCurVals);
  }

  return;
}

//*************************************
void SystematicRoutines::Run(){
//*************************************

  std::cout << "Running routines "<< std::endl;
  fRoutines = GeneralUtils::ParseToStr(fStrategy,",");

  for (UInt_t i = 0; i < fRoutines.size(); i++){

    std::string routine = fRoutines.at(i);
    int fitstate = kFitUnfinished;
    LOG(FIT)<<"Running Routine: "<<routine<<std::endl;

    if (routine.compare("PlotLimits") == 0) PlotLimits();
    else if (routine.compare("ErrorBands") == 0) GenerateErrorBands();
    else if (routine.compare("ThrowErrors") == 0) GenerateThrows();
    else if (routine.compare("MergeErrors") == 0) MergeThrows();
    else if (routine.compare("EigenErrors") == 0) EigenErrors();
    else {
      std::cout << "Unknown ROUTINE : " << routine << std::endl;
    }

    // If ending early break here
    if (fitstate == kFitFinished || fitstate == kNoChange){
      LOG(FIT) << "Ending fit routines loop." << std::endl;
      break;
    }
  }

  return;
}

void SystematicRoutines::GenerateErrorBands(){
  GenerateThrows();
  MergeThrows();
}

//*************************************
void SystematicRoutines::GenerateThrows(){
//*************************************


  TFile* tempfile = new TFile((fOutputFile + ".throws.root").c_str(),"RECREATE");
  tempfile->cd();

  int nthrows = fNThrows;
  int startthrows = fStartThrows;
  int endthrows = startthrows + nthrows;

  if (nthrows < 0) nthrows = endthrows;
  if (startthrows < 0) startthrows = 0;
  if (endthrows < 0) endthrows = startthrows + nthrows;

  // Setting Seed
  // Matteo Mazzanti's Fix
  struct timeval mytime;
  gettimeofday(&mytime, NULL);
  Double_t seed = time(NULL) + int(getpid())+ (mytime.tv_sec * 1000.) + (mytime.tv_usec / 1000.);
  gRandom->SetSeed(seed);

  //  int seed = (gRandom->Uniform(0.0,1.0)*100000 + 100000000*(startthrows + endthrows) + time(NULL) + int(getpid()) );
  //  gRandom->SetSeed(seed);
  LOG(FIT) << "Using Seed : " << seed << std::endl;
  LOG(FIT) << "nthrows = " << nthrows << std::endl;
  LOG(FIT) << "startthrows = " << startthrows << std::endl;
  LOG(FIT) << "endthrows = " << endthrows << std::endl;



  UpdateRWEngine(fCurVals);
  fSampleFCN->ReconfigureAllEvents();

  if (startthrows == 0){
    LOG(FIT) << "Making nominal " << std::endl;
    TDirectory* nominal = (TDirectory*) tempfile->mkdir("nominal");
    nominal->cd();
    fSampleFCN->Write();
  }

  LOG(SAM) << "nthrows = " << nthrows << std::endl;
  LOG(SAM) << "startthrows = " << startthrows << std::endl;
  LOG(SAM) << "endthrows = " << endthrows << std::endl;

  TTree* parameterTree = new TTree("throws","throws");
  double chi2;
  for (UInt_t i = 0; i < fParams.size(); i++)
    parameterTree->Branch(fParams[i].c_str(), &fThrownVals[fParams[i]], (fParams[i] + "/D").c_str());
  parameterTree->Branch("chi2",&chi2,"chi2/D");
  fSampleFCN->CreateIterationTree("error_iterations", FitBase::GetRW());

  // Would anybody actually want to do uniform throws of any parameter??
  bool uniformly = FitPar::Config().GetParB("error_uniform");

  // Run Throws and save
  for (Int_t i = 0; i < endthrows+1; i++){

    LOG(FIT) << "Loop " << i << std::endl;
    ThrowCovariance(uniformly);
    if (i < startthrows) continue;
    if (i == 0) continue;
    LOG(FIT) << "Throw " << i << " ================================" << std::endl;
    // Generate Random Parameter Throw
    //    ThrowCovariance(uniformly);

    TDirectory* throwfolder = (TDirectory*)tempfile->mkdir(Form("throw_%i",i));
    throwfolder->cd();

    // Run Eval
    double *vals = FitUtils::GetArrayFromMap( fParams, fThrownVals );
    chi2 = fSampleFCN->DoEval( vals );
    delete vals;

    // Save the FCN
    fSampleFCN->Write();

    parameterTree->Fill();
  }

  tempfile->cd();
  fSampleFCN->WriteIterationTree();

  tempfile->Close();
}

void SystematicRoutines::MergeThrows(){


    fOutputRootFile = new TFile(fCompKey.GetS("outputfile").c_str(), "RECREATE");
    fOutputRootFile->cd();


  // Make a container folder
  TDirectory* errorDIR = (TDirectory*) fOutputRootFile->mkdir("error_bands");
  errorDIR->cd();

  TDirectory* outnominal = (TDirectory*) fOutputRootFile->mkdir("nominal_throw");
  outnominal->cd();

  // Split Input Files
  if (!fThrowString.empty()) fThrowList = GeneralUtils::ParseToStr(fThrowString,",");

  // Add default if no throwlist given
  if (fThrowList.size() < 1) fThrowList.push_back( fOutputFile + ".throws.root" ); 

  /// Save location of file containing nominal
  std::string nominalfile;
  bool nominalfound;

  // Loop over files and check they exist.
  for (uint i = 0; i < fThrowList.size(); i++){
    std::string file = fThrowList[i];
    bool found = false;

    // normal
    std::string newfile = file;
    TFile* throwfile = new TFile(file.c_str(),"READ");
    if (throwfile and !throwfile->IsZombie()){ 
        found = true;
    }

    // normal.throws.root
    if (!found){
      newfile = file + ".throws.root";
      throwfile = new TFile((file + ".throws.root").c_str(),"READ");
      if (throwfile and !throwfile->IsZombie()) {
        found = true;
      }
    }

    // If its found save to throwlist, else save empty.
    // Also search for nominal
    if (found){
      fThrowList[i] = newfile;
      
      LOG(FIT) << "Throws File :" << newfile << std::endl;

      // Find input which contains nominal
      if (throwfile->Get("nominal")){
        nominalfound = true;
        nominalfile = newfile;
      }

      throwfile->Close();

    } else {
      fThrowList[i] = "";
    }

    delete throwfile;
  }

  // Make sure we have a nominal file
  if (!nominalfound or nominalfile.empty()){
    ERR(FTL) << "No nominal found when mergining! Exiting!" << std::endl;
    throw;  
  }

  

    // Get the nominal throws file
  TFile* tempfile = new TFile((nominalfile).c_str(),"READ");
  tempfile->cd();
  TDirectory* nominal = (TDirectory*)tempfile->Get("nominal");
  // int nthrows = FitPar::Config().GetParI("error_throws");
  bool uniformly = FitPar::Config().GetParB("error_uniform");

  // Check percentage of bad files is okay.
  int badfilecount = 0;
  for (uint i = 0; i < fThrowList.size(); i++){
    if (!fThrowList[i].empty()){
      LOG(FIT) << "Loading Throws From File " << i << " : " 
	       << fThrowList[i] << std::endl;
    } else {
      badfilecount++;
    }
  }

  // Check we have at least one good file
  if ((uint)badfilecount == fThrowList.size()){
    ERR(FTL) << "Found no good throw files for MergeThrows" << std::endl;
    throw;
  } else if (badfilecount > fThrowList.size()*0.25){
    ERR(WRN) << "Over 25% of your throw files are dodgy. Please check this is okay!" << std::endl;
    ERR(WRN) << "Will continue for the time being..." << std::endl;
    sleep(5);
  }

  // Now go through the keys in the temporary file and look for TH1D, and TH2D plots
  TIter next(nominal->GetListOfKeys());
  TKey *key;
  while ((key = (TKey*)next())) {
    TClass *cl = gROOT->GetClass(key->GetClassName());
    if (!cl->InheritsFrom("TH1D") and !cl->InheritsFrom("TH2D")) continue;
    TH1* baseplot = (TH1D*)key->ReadObj();
    std::string plotname = std::string(baseplot->GetName());
    LOG(FIT) << "Creating error bands for " << plotname;
    if (LOG_LEVEL(FIT)){
      if (!uniformly) std::cout << " : Using COVARIANCE Throws! " << std::endl;
      else std::cout << " : Using UNIFORM THROWS!!! " << std::endl;
    }

    int nbins = 0;
    if (cl->InheritsFrom("TH1D")) nbins = ((TH1D*)baseplot)->GetNbinsX();
    else nbins = ((TH1D*)baseplot)->GetNbinsX()* ((TH1D*)baseplot)->GetNbinsY();

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

    // Make new throw plot
    TH1* newplot;

    // Run Throw Merging.
    for (UInt_t i = 0; i < fThrowList.size(); i++){

      TFile* throwfile = new TFile(fThrowList[i].c_str(), "READ");

      // Loop over all throws in a folder
      TIter nextthrow(throwfile->GetListOfKeys());
      TKey *throwkey;
      while ((throwkey = (TKey*)nextthrow())) {
        
        // Skip non throw folders
        if (std::string(throwkey->GetName()).find("throw_") == std::string::npos) continue;

        // Get Throw DIR
        TDirectory* throwdir = (TDirectory*)throwkey->ReadObj();

        // Get Plot From Throw
        newplot = (TH1*)throwdir->Get(plotname.c_str());
        if (!newplot) continue;

        // Loop Over Plot
        for (Int_t j = 0; j < nbins; j++){
          tprof->Fill(j+0.5, newplot->GetBinContent(j+1));
          bincontents[j] = newplot->GetBinContent(j+1);

          if (bincontents[j] < binlowest[j] or i == 0) binlowest[j] = bincontents[j];
          if (bincontents[j] > binhighest[j] or i == 0) binhighest[j] = bincontents[j];
        }

        errorDIR->cd();
        bintree->Fill();
      }

      throwfile->Close();
      delete throwfile;
    }

    errorDIR->cd();

    if (uniformly){
      LOG(FIT) << "Uniformly Calculating Plot Errors!" << std::endl;
    }

    TH1* statplot = (TH1*) baseplot->Clone();

    for (Int_t j = 0; j < nbins; j++){

      if (!uniformly){
//	if ((baseplot->GetBinError(j+1)/baseplot->GetBinContent(j+1)) < 1.0) {
	  //	  baseplot->SetBinError(j+1,sqrt(pow(tprof->GetBinError(j+1),2) + pow(baseplot->GetBinError(j+1),2)));
	//	} else {
	//baseplot->SetBinContent(j+1,tprof->GetBinContent(j+1));
	baseplot->SetBinError(j+1,tprof->GetBinError(j+1));
	  //	}
      } else {
      	baseplot->SetBinContent(j+1, 0.0);//(binlowest[j] + binhighest[j]) / 2.0);
        baseplot->SetBinError(j+1, 0.0); //(binhighest[j] - binlowest[j])/2.0);
      }
    }

    errorDIR->cd();
    baseplot->Write();
    tprof->Write();
    bintree->Write();

    outnominal->cd();
    for (int i = 0; i < nbins; i++){
      baseplot->SetBinError(i+1, sqrt(pow(statplot->GetBinError(i+1),2) + pow(baseplot->GetBinError(i+1),2)));
    }
    baseplot->Write();
    
    delete statplot;
    delete baseplot;
    delete tprof;
    delete bintree;
    delete [] bincontents;
  }

  return;
};

void SystematicRoutines::EigenErrors(){


    fOutputRootFile = new TFile(fCompKey.GetS("outputfile").c_str(), "RECREATE");
    fOutputRootFile->cd();

  // Make Covariance
  TMatrixDSym* fullcovar = new TMatrixDSym( fParams.size() );

  // Extract covariance from all loaded ParamPulls
  for (PullListConstIter iter = fInputThrows.begin();
       iter != fInputThrows.end(); iter++){
    ParamPull* pull = *iter;

    // Check pull is actualyl Gaussian
    std::string pulltype = pull->GetType();
    if (pulltype.find("GAUSTHROW") == std::string::npos){
      THROW("Can only calculate EigenErrors for Gaussian pulls!");
    }

    // Get data and covariances
    TH1D dialhist = pull->GetDataHist();
    TH2D covhist  = pull->GetFullCovar();

    // Loop over all dials and compare names
    for (size_t pari = 0; pari < fParams.size(); pari++){
      for (size_t parj = 0; parj < fParams.size(); parj++){
	
	std::string name_pari = fParams[pari];
	std::string name_parj = fParams[parj];

	// Compare names to those in the pull
	for (int pulli = 0; pulli < dialhist.GetNbinsX(); pulli++){
	  for (int pullj = 0; pullj < dialhist.GetNbinsX(); pullj++){
	    
	    std::string name_pulli = dialhist.GetXaxis()->GetBinLabel(pulli+1);
	    std::string name_pullj = dialhist.GetXaxis()->GetBinLabel(pullj+1);

	    if (name_pulli == name_pari && name_pullj == name_parj){
	      (*fullcovar)[pari][parj] = covhist.GetBinContent(pulli+1, pullj+1);
	      fCurVals[name_pari] = dialhist.GetBinContent(pulli+1);
	      fCurVals[name_parj] = dialhist.GetBinContent(pullj+1);
	    }
	    
	  }
	}
	
      }
    }
  }

  /*
  TFile* test = new TFile("testingcovar.root","RECREATE");
  test->cd();
  TH2D* joinedcov = new TH2D("COVAR","COVAR",
			     fullcovar->GetNrows(), 0.0, float(fullcovar->GetNrows()), 
			     fullcovar->GetNrows(), 0.0, float(fullcovar->GetNrows()));
  for (int i = 0; i < fullcovar->GetNrows(); i++){
    for (int j = 0; j < fullcovar->GetNcols(); j++){
      joinedcov->SetBinContent(i+1, j+1, (*fullcovar)[i][j]);
    }
  }
  joinedcov->Write("COVAR");
  test->Close();
  */

  // Calculator all EigenVectors and EigenValues
  TMatrixDSymEigen* eigen = new TMatrixDSymEigen(*fullcovar);
  const TVectorD eigenVals = eigen->GetEigenValues();
  const TMatrixD eigenVect = eigen->GetEigenVectors();
  eigenVals.Print();
  eigenVect.Print();

  TDirectory* outnominal = (TDirectory*) fOutputRootFile->mkdir("nominal");
  outnominal->cd();

  double *valst = FitUtils::GetArrayFromMap( fParams, fCurVals );
  //double chi2 = fSampleFCN->DoEval( valst );
  delete valst;
  fSampleFCN->Write();

  // Loop over all throws
  TDirectory* throwsdir = (TDirectory*) fOutputRootFile->mkdir("throws");
  throwsdir->cd();

  int count = 0;
  // Produce all error throws.
  for (int i = 0; i < eigenVect.GetNrows(); i++){

    TDirectory* throwfolder = (TDirectory*)throwsdir->mkdir(Form("throw_%i",count));
    throwfolder->cd();

    // Get New Parameter Vector
    LOG(FIT) << "Parameter Set " << count << std::endl;
    for (int j = 0; j < eigenVect.GetNrows(); j++){
      std::string param = fParams[j];
      LOG(FIT) << " " << j << ". " << param << " : " << fCurVals[param] + sqrt(eigenVals[i]) * eigenVect[j][i] << std::endl;
      fThrownVals[param] = fCurVals[param] + sqrt(eigenVals[i]) * eigenVect[j][i];
    }

    // Run Eval
    double *vals = FitUtils::GetArrayFromMap( fParams, fThrownVals );
    double chi2 = fSampleFCN->DoEval( vals );
    delete vals;
    count++;

    fSampleFCN->Write();

    
    throwfolder = (TDirectory*)throwsdir->mkdir(Form("throw_%i",count));
    throwfolder->cd();

    // Get New Parameter Vector
    LOG(FIT) << "Parameter Set " << count << std::endl;
    for (int j = 0; j < eigenVect.GetNrows(); j++){
      std::string param = fParams[j];
      LOG(FIT) << " " << j << ". " << param << " : " <<fCurVals[param] - sqrt(eigenVals[i]) * eigenVect[j][i] << std::endl;
      fThrownVals[param] = fCurVals[param] - sqrt(eigenVals[i]) * eigenVect[j][i];
    }

    // Run Eval
    double *vals2 = FitUtils::GetArrayFromMap( fParams, fThrownVals );
    chi2 = fSampleFCN->DoEval( vals2 );
    delete vals2;
    count++;
    
    // Save the FCN
    fSampleFCN->Write();
    
  }

  fOutputRootFile->Close();  
  fOutputRootFile = new TFile(fCompKey.GetS("outputfile").c_str(), "UPDATE");
  fOutputRootFile->cd();
  throwsdir = (TDirectory*) fOutputRootFile->Get("throws");
  outnominal = (TDirectory*) fOutputRootFile->Get("nominal");

  // Loop through Error DIR
  TDirectory* outerr = (TDirectory*) fOutputRootFile->mkdir("errors");
  outerr->cd();
  TIter next(outnominal->GetListOfKeys());
  TKey *key;
  while ((key = (TKey*)next())) {
    
    TClass *cl = gROOT->GetClass(key->GetClassName());
    if (!cl->InheritsFrom("TH1D") and !cl->InheritsFrom("TH2D")) continue;
    
    LOG(FIT) << "Creating error bands for " << key->GetName() << std::endl;
    std::string plotname = std::string(key->GetName());

    if (plotname.find("_EVT") != std::string::npos) continue;
    if (plotname.find("_FLUX") != std::string::npos) continue;
    if (plotname.find("_FLX") != std::string::npos) continue;

    TH1* baseplot = (TH1D*)key->ReadObj()->Clone(Form("%s_ORIGINAL",key->GetName()));
    TH1* errorplot_upper = (TH1D*)baseplot->Clone(Form("%s_ERROR_UPPER",key->GetName()));
    TH1* errorplot_lower = (TH1D*)baseplot->Clone(Form("%s_ERROR_LOWER", key->GetName()));
    TH1* meanplot = (TH1D*)baseplot->Clone(Form("%s_SET_MEAN", key->GetName()));
    TH1* systplot = (TH1D*)baseplot->Clone(Form("%s_SYST", key->GetName()));
    TH1* statplot = (TH1D*)baseplot->Clone(Form("%s_STAT", key->GetName()));
    TH1* totlplot = (TH1D*)baseplot->Clone(Form("%s_TOTAL", key->GetName()));

    int nbins = 0;
    if (cl->InheritsFrom("TH1D")) nbins = ((TH1D*)baseplot)->GetNbinsX();
    else nbins = ((TH1D*)baseplot)->GetNbinsX()* ((TH1D*)baseplot)->GetNbinsY();

    meanplot->Reset();
    errorplot_upper->Reset();
    errorplot_lower->Reset();

    for (int j = 0; j < nbins; j++){
      errorplot_upper->SetBinError(j+1, 0.0);
      errorplot_lower->SetBinError(j+1, 0.0);
    }

    // Loop over throws and calculate mean and error for +- throws
    int addcount = 0;

    // Add baseplot first to slightly bias to central value
    meanplot->Add(baseplot);
    addcount++;

    for (int i = 0; i < count; i++){
      TH1* newplot = (TH1D*) throwsdir->Get(Form("throw_%i/%s",i,plotname.c_str()));
      if (!newplot){
	ERR(WRN) << "Cannot find new plot : " << Form("throw_%i/%s",i,plotname.c_str()) << std::endl;
	ERR(WRN) << "This plot will not have the correct errors!" << std::endl;
	continue;
      }
      newplot->SetDirectory(0);
      nbins = newplot->GetNbinsX();
    
      for (int j = 0; j < nbins; j++){
	if (i % 2 == 0){
	  //	  std::cout << plotname<< " : upper " << errorplot_upper->GetBinContent(j+1) << " adding " << pow(baseplot->GetBinContent(j+1) - newplot->GetBinContent(j+1),2) << std::endl;
	  //	  std::cout << " -> " << baseplot->GetBinContent(j+1) << " " <<newplot->GetBinContent(j+1) << std::endl;
	  errorplot_upper->SetBinContent(j+1, errorplot_upper->GetBinContent(j+1) + 
					 pow(baseplot->GetBinContent(j+1) - newplot->GetBinContent(j+1),2));
	  //	  newplot->Print();
	} else {
	  //	  std::cout << plotname << " : lower " << errorplot_lower->GetBinContent(j+1) << " adding " << pow(baseplot->GetBinContent(j+1) - newplot->GetBinContent(j+1),2) << std::endl;
	  //	  std::cout << " -> " << baseplot->GetBinContent(j+1) << " " << newplot->GetBinContent(j+1) << std::endl;
	  errorplot_lower->SetBinContent(j+1, errorplot_lower->GetBinContent(j+1) +
                                         pow(baseplot->GetBinContent(j+1) - newplot->GetBinContent(j+1),2));
	  //	  newplot->Print();
	}
	meanplot->SetBinContent(j+1, meanplot->GetBinContent(j+1) + baseplot->GetBinContent(j+1));
      }
      delete newplot;
      addcount++;
    }
    
    // Get mean Average
    for (int j = 0; j < nbins; j++){
      meanplot->SetBinContent(j+1, meanplot->GetBinContent(j+1)/double(addcount));
    }

    for (int j = 0; j < nbins; j++){
      errorplot_upper->SetBinContent(j+1, sqrt(errorplot_upper->GetBinContent(j+1)));
      errorplot_lower->SetBinContent(j+1, sqrt(errorplot_lower->GetBinContent(j+1)));
      
      statplot->SetBinError(j+1, baseplot->GetBinError(j+1) );
      systplot->SetBinError(j+1, (errorplot_upper->GetBinContent(j+1) + errorplot_lower->GetBinContent(j+1))/2.0);
      totlplot->SetBinError(j+1, sqrt( pow(statplot->GetBinError(j+1),2) + pow(systplot->GetBinError(j+1),2) ) );

      meanplot->SetBinError(j+1, sqrt( pow(statplot->GetBinError(j+1),2) + pow(systplot->GetBinError(j+1),2) ) );
     
    }

    outerr->cd();
    errorplot_upper->Write();
    errorplot_lower->Write();
    baseplot->Write();
    meanplot->Write();

    statplot->Write();
    systplot->Write();
    totlplot->Write();

    delete errorplot_upper;
    delete errorplot_lower;
    delete baseplot;
    delete meanplot;
    delete statplot;
    delete systplot;
    delete totlplot;
  }

}
