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

#include "MinimizerRoutines.h"

/*
  Constructor/Destructor
*/
//************************
void MinimizerRoutines::Init() {
//************************

  fInputFile = "";
  fInputRootFile = NULL;

  fOutputFile = "";
  fOutputRootFile = NULL;

  fCovar      = NULL;
  fCovFree  = NULL;
  fCorrel     = NULL;
  fCorFree = NULL;
  fDecomp     = NULL;
  fDecFree = NULL;

  fStrategy = "Migrad,FixAtLimBreak,Migrad";
  fRoutines.clear();

  fCardFile = "";

  fFakeDataInput = "";

  fSampleFCN    = NULL;

  fMinimizer    = NULL;
  fMinimizerFCN = NULL;
  fCallFunctor  = NULL;

  fAllowedRoutines = ("Migrad,Simplex,Combined,"
                      "Brute,Fumili,ConjugateFR,"
                      "ConjugatePR,BFGS,BFGS2,"
                      "SteepDesc,GSLSimAn,FixAtLim,FixAtLimBreak"
                      "Chi2Scan1D,Chi2Scan2D,Contours,ErrorBands");
};

//*************************************
MinimizerRoutines::~MinimizerRoutines() {
//*************************************
};

/*
  Input Functions
*/
//*************************************
MinimizerRoutines::MinimizerRoutines(int argc, char* argv[]) {
//*************************************

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

  // Make easier to handle arguments.
  std::vector<std::string> args = GeneralUtils::LoadCharToVectStr(argc, argv);
  ParserUtils::ParseArgument(args, "-c", fCardFile, true);
  ParserUtils::ParseArgument(args, "-o", fOutputFile, false, false);
  ParserUtils::ParseArgument(args, "-n", maxevents, false, false);
  ParserUtils::ParseArgument(args, "-f", fStrategy, false, false);
  ParserUtils::ParseArgument(args, "-d", fFakeDataInput, false, false);
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
  nuiskey fCompKey;
  if (Config::Get().GetNodes("nuiscomp").empty()) {
    fCompKey = Config::Get().CreateNode("nuiscomp");
  } else {
    fCompKey = Config::Get().GetNodes("nuiscomp")[0];
  }

  if (!fCardFile.empty())   fCompKey.AddS("cardfile", fCardFile);
  if (!fOutputFile.empty()) fCompKey.AddS("outputfile", fOutputFile);
  if (!fStrategy.empty())   fCompKey.AddS("strategy", fStrategy);

  // Load XML Cardfile
  configuration.LoadConfig( fCompKey.GetS("cardfile"), "");

  // Add CMD XML Structs
  for (size_t i = 0; i < xmlcmds.size(); i++) {
    configuration.AddXMLLine(xmlcmds[i]);
  }

  // Add Config Args
  for (size_t i = 0; i < configargs.size(); i++) {
    configuration.OverrideConfig(configargs[i]);
  }
  if (maxevents.compare("-1")){
    configuration.OverrideConfig("MAXEVENTS=" + maxevents);
  }

  // Finish configuration XML
  configuration.FinaliseConfig(fCompKey.GetS("outputfile") + ".xml");

  // Add Error Verbo Lines
  verbocount += Config::Get().GetParI("VERBOSITY");
  errorcount += Config::Get().GetParI("ERROR");
  std::cout << "[ NUISANCE ]: Setting VERBOSITY=" << verbocount << std::endl;
  std::cout << "[ NUISANCE ]: Setting ERROR=" << errorcount << std::endl;
  // FitPar::log_verb = verbocount;
  SETVERBOSITY(verbocount);
  // ERR_VERB(errorcount);

  // Minimizer Setup ========================================
  fOutputRootFile = new TFile(fCompKey.GetS("outputfile").c_str(), "RECREATE");
  SetupMinimizerFromXML();

  SetupCovariance();
  SetupRWEngine();
  SetupFCN();

  return;
};

//*************************************
void MinimizerRoutines::SetupMinimizerFromXML() {
//*************************************

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

    // Override state if none given
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
    fStartVals[normname] = samplenorm;
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
void MinimizerRoutines::SetupRWEngine() {
//*************************************

  for (UInt_t i = 0; i < fParams.size(); i++) {
    std::string name = fParams[i];
    FitBase::GetRW() -> IncludeDial(name, fTypeVals.at(name) );
  }
  UpdateRWEngine(fStartVals);

  return;
}

//*************************************
void MinimizerRoutines::SetupFCN() {
//*************************************

  LOG(FIT) << "Making the jointFCN" << std::endl;
  if (fSampleFCN) delete fSampleFCN;
  // fSampleFCN = new JointFCN(fCardFile, fOutputRootFile);
  fSampleFCN = new JointFCN(fOutputRootFile);
  
  SetFakeData();

  fMinimizerFCN = new MinimizerFCN( fSampleFCN );
  fCallFunctor  = new ROOT::Math::Functor( *fMinimizerFCN, fParams.size() );

  fSampleFCN->CreateIterationTree( "fit_iterations", FitBase::GetRW() );

  return;
}


//******************************************
void MinimizerRoutines::SetupFitter(std::string routine) {
//******************************************

  // Make the fitter
  std::string fitclass = "";
  std::string fittype  = "";

  // Get correct types
  if      (!routine.compare("Migrad"))      {
    fitclass = "Minuit2"; fittype = "Migrad";
  } else if (!routine.compare("Simplex"))     {
    fitclass = "Minuit2"; fittype = "Simplex";
  } else if (!routine.compare("Combined"))    {
    fitclass = "Minuit2"; fittype = "Combined";
  } else if (!routine.compare("Brute"))       {
    fitclass = "Minuit2"; fittype = "Scan";
  } else if (!routine.compare("Fumili"))      {
    fitclass = "Minuit2"; fittype = "Fumili";
  } else if (!routine.compare("ConjugateFR")) {
    fitclass = "GSLMultiMin"; fittype = "ConjugateFR";
  } else if (!routine.compare("ConjugatePR")) {
    fitclass = "GSLMultiMin"; fittype = "ConjugatePR";
  } else if (!routine.compare("BFGS"))        {
    fitclass = "GSLMultiMin"; fittype = "BFGS";
  } else if (!routine.compare("BFGS2"))       {
    fitclass = "GSLMultiMin"; fittype = "BFGS2";
  } else if (!routine.compare("SteepDesc"))   {
    fitclass = "GSLMultiMin"; fittype = "SteepestDescent";
    //  } else if (!routine.compare("GSLMulti"))    { fitclass = "GSLMultiFit"; fittype = "";         // Doesn't work out of the box
  } else if (!routine.compare("GSLSimAn"))    { fitclass = "GSLSimAn"; fittype = "";   }

  // make minimizer
  if (fMinimizer) delete fMinimizer;
  fMinimizer = ROOT::Math::Factory::CreateMinimizer(fitclass, fittype);

  fMinimizer->SetMaxFunctionCalls(FitPar::Config().GetParI("minimizer.maxcalls"));

  if (!routine.compare("Brute")) {
    fMinimizer->SetMaxFunctionCalls(fParams.size() * fParams.size() * 4);
    fMinimizer->SetMaxIterations(fParams.size() * fParams.size() * 4);
  }

  fMinimizer->SetMaxIterations(FitPar::Config().GetParI("minimizer.maxiterations"));
  fMinimizer->SetTolerance(FitPar::Config().GetParD("minimizer.tolerance"));
  fMinimizer->SetStrategy(FitPar::Config().GetParI("minimizer.strategy"));
  fMinimizer->SetFunction(*fCallFunctor);

  int ipar = 0;
  //Add Fit Parameters
  for (UInt_t i = 0; i < fParams.size(); i++) {
    std::string syst = fParams.at(i);

    bool fixed = true;
    double vstart, vstep, vlow, vhigh;
    vstart = vstep = vlow = vhigh = 0.0;

    if (fCurVals.find(syst) != fCurVals.end()  ) vstart = fCurVals.at(syst);
    if (fMinVals.find(syst)  != fMinVals.end() ) vlow   = fMinVals.at(syst);
    if (fMaxVals.find(syst)  != fMaxVals.end() ) vhigh  = fMaxVals.at(syst);
    if (fStepVals.find(syst) != fStepVals.end()) vstep  = fStepVals.at(syst);
    if (fFixVals.find(syst)  != fFixVals.end() ) fixed  = fFixVals.at(syst);

    // fix for errors
    if (vhigh == vlow) vhigh += 1.0;

    fMinimizer->SetVariable(ipar, syst, vstart, vstep);
    fMinimizer->SetVariableLimits(ipar, vlow, vhigh);

    if (fixed) {

      fMinimizer->FixVariable(ipar);
      LOG(FIT) << "Fixed Param: " << syst << std::endl;

    } else {

      LOG(FIT) << "Free  Param: " << syst
               << " Start:" << vstart
               << " Range:" << vlow << " to " << vhigh
               << " Step:" << vstep << std::endl;
    }

    ipar++;
  }

  LOG(FIT) << "Setup Minimizer: " << fMinimizer->NDim() << "(NDim) " << fMinimizer->NFree() << "(NFree)" << std::endl;

  return;
}

//*************************************
// Set fake data from user input
void MinimizerRoutines::SetFakeData() {
//*************************************

  // If the fake data input field (-d) isn't provided, return to caller
  if (fFakeDataInput.empty()) return;

  // If user specifies -d MC we set the data to the MC
  // User can also specify fake data parameters to reweight by doing "fake_parameter" in input card file
  // "fake_parameter" gets read in ReadCard function (reads to fFakeVals)
  if (fFakeDataInput.compare("MC") == 0) {

    LOG(FIT) << "Setting fake data from MC starting prediction." << std::endl;
    // fFakeVals get read in in ReadCard
    UpdateRWEngine(fFakeVals);

    // Reconfigure the reweight engine
    FitBase::GetRW()->Reconfigure();
    // Reconfigure all the samples to the new reweight
    fSampleFCN->ReconfigureAllEvents();
    // Feed on and set the fake-data in each measurement class
    fSampleFCN->SetFakeData("MC");

    // Changed the reweight engine values back to the current values
    // So we start the fit at a different value than what we set the fake-data to
    UpdateRWEngine(fCurVals);

    LOG(FIT) << "Set all data to fake MC predictions." << std::endl;
  } else {
    fSampleFCN->SetFakeData(fFakeDataInput);
  }

  return;
}

/*
  Fitting Functions
*/
//*************************************
void MinimizerRoutines::UpdateRWEngine(std::map<std::string, double>& updateVals) {
//*************************************

  for (UInt_t i = 0; i < fParams.size(); i++) {
    std::string name = fParams[i];

    if (updateVals.find(name) == updateVals.end()) continue;
    FitBase::GetRW()->SetDialValue(name, updateVals.at(name));
  }

  FitBase::GetRW()->Reconfigure();
  return;
}

//*************************************
void MinimizerRoutines::Run() {
//*************************************

  LOG(FIT) << "Running MinimizerRoutines : " << fStrategy << std::endl;
  if (FitPar::Config().GetParB("save_nominal")) {
    SaveNominal();
  }

  // Parse given routines
  fRoutines = GeneralUtils::ParseToStr(fStrategy,",");
  if (fRoutines.empty()){
    ERR(FTL) << "Trying to run MinimizerRoutines with no routines given!" << std::endl;
    throw;
  }

  for (UInt_t i = 0; i < fRoutines.size(); i++) {

    std::string routine = fRoutines.at(i);
    int fitstate = kFitUnfinished;
    LOG(FIT) << "Running Routine: " << routine << std::endl;

    // Try Routines
    if (routine.find("LowStat") != std::string::npos) LowStatRoutine(routine);
    else if (routine == "FixAtLim")  FixAtLimit();
    else if (routine == "FixAtLimBreak") fitstate = FixAtLimit();
    else if (routine.find("ErrorBands") != std::string::npos) GenerateErrorBands();
    else if (!routine.compare("Chi2Scan1D")) Create1DScans();
    else if (!routine.compare("Chi2Scan2D")) Chi2Scan2D();
    else fitstate = RunFitRoutine(routine);

    // If ending early break here
    if (fitstate == kFitFinished || fitstate == kNoChange) {
      LOG(FIT) << "Ending fit routines loop." << std::endl;
      break;
    }
  }

  return;
}

//*************************************
int MinimizerRoutines::RunFitRoutine(std::string routine) {
//*************************************
  int endfits = kFitUnfinished;

  // set fitter at the current start values
  fOutputRootFile->cd();
  SetupFitter(routine);

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
           //    !routine.compare("GSLMulti") or
           !routine.compare("GSLSimAn")) {

    if (fMinimizer->NFree() > 0) {
      LOG(FIT) << fMinimizer->Minimize() << std::endl;
      GetMinimizerState();
    }
  }

  // other otptions
  else if (!routine.compare("Contour")) {
    CreateContours();
  }

  return endfits;
}

//*************************************
void MinimizerRoutines::PrintState() {
//*************************************
  LOG(FIT) << "------------" << std::endl;

  // Count max size
  int maxcount = 0;
  for (UInt_t i = 0; i < fParams.size(); i++) {
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
  for (UInt_t i = 0; i < fParams.size(); i++) {
    std::string syst = fParams.at(i);

    std::string typestr  = FitBase::ConvDialType(fTypeVals[syst]);
    std::string curunits = "(sig.)";
    double      curval   = fCurVals[syst];
    double      curerr   = fErrorVals[syst];

    if (fStateVals[syst].find("ABS") != std::string::npos) {
      curval = FitBase::RWSigmaToAbs(typestr, syst, curval);
      curerr = (FitBase::RWSigmaToAbs(typestr, syst, curerr) -
                FitBase::RWSigmaToAbs(typestr, syst, 0.0));
      curunits = "(Abs.)";
    } else if (fStateVals[syst].find("FRAC") != std::string::npos) {
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

  LOG(FIT) << "------------" << std::endl;
  double like = fSampleFCN->GetLikelihood();
  LOG(FIT) << std::left << std::setw(46) << "Likelihood for JointFCN: " << like << std::endl;
  LOG(FIT) << "------------" << std::endl;
}

//*************************************
void MinimizerRoutines::GetMinimizerState() {
//*************************************

  LOG(FIT) << "Minimizer State: " << std::endl;
  // Get X and Err
  const double *values = fMinimizer->X();
  const double *errors = fMinimizer->Errors();
  //  int ipar = 0;

  for (UInt_t i = 0; i < fParams.size(); i++) {
    std::string syst = fParams.at(i);

    fCurVals[syst]   = values[i];
    fErrorVals[syst] = errors[i];
  }

  PrintState();

  // Covar
  SetupCovariance();
  if (fMinimizer->CovMatrixStatus() > 0) {

    // Fill Full Covar
    std::cout << "Filling covariance" << std::endl;
    for (int i = 0; i < fCovar->GetNbinsX(); i++) {
      for (int j = 0; j < fCovar->GetNbinsY(); j++) {
        fCovar->SetBinContent(i + 1, j + 1, fMinimizer->CovMatrix(i, j));
      }
    }

    int freex = 0;
    int freey = 0;
    for (int i = 0; i < fCovar->GetNbinsX(); i++) {
      if (fMinimizer->IsFixedVariable(i)) continue;
      freey = 0;

      for (int j = 0; j < fCovar->GetNbinsY(); j++) {
        if (fMinimizer->IsFixedVariable(j)) continue;

        fCovFree->SetBinContent(freex + 1, freey + 1, fMinimizer->CovMatrix(i, j));
        freey++;
      }
      freex++;
    }

    fCorrel     = PlotUtils::GetCorrelationPlot(fCovar, "correlation");
    fDecomp     = PlotUtils::GetDecompPlot(fCovar, "decomposition");
    if (fMinimizer->NFree() > 0) {
      fCorFree = PlotUtils::GetCorrelationPlot(fCovFree, "correlation_free");
      fDecFree = PlotUtils::GetDecompPlot(fCovFree, "decomposition_free");
    }
  }

  std::cout << "Got STATE" << std::endl;

  return;
};

//*************************************
void MinimizerRoutines::LowStatRoutine(std::string routine) {
//*************************************

  LOG(FIT) << "Running Low Statistics Routine: " << routine << std::endl;
  int lowstatsevents = FitPar::Config().GetParI("minimizer.lowstatevents");
  int maxevents      = FitPar::Config().GetParI("input.maxevents");
  int verbosity      = FitPar::Config().GetParI("VERBOSITY");

  std::string trueroutine = routine;
  std::string substring = "LowStat";
  trueroutine.erase( trueroutine.find(substring),
                     substring.length() );

  // Set MAX EVENTS=1000
  FitPar::Config().SetParI("input.maxevents", lowstatsevents);
  FitPar::Config().SetParI("VERBOSITY", 3);
  SetupFCN();

  RunFitRoutine(trueroutine);

  FitPar::Config().SetParI("input.maxevents", maxevents);
  SetupFCN();

  FitPar::Config().SetParI("VERBOSITY", verbosity);
  return;
}

//*************************************
void MinimizerRoutines::Create1DScans() {
//*************************************

  // 1D Scan Routine
  // Steps through all free parameters about nominal using the step size
  // Creates a graph for each free parameter

  // At the current point create a 1D Scan for all parametes (Uncorrelated)
  for (UInt_t i = 0; i < fParams.size(); i++) {

    if (fFixVals[fParams[i]]) continue;

    LOG(FIT) << "Running 1D Scan for " << fParams[i] << std::endl;
    fSampleFCN->CreateIterationTree(fParams[i] +
                                    "_scan1D_iterations",
                                    FitBase::GetRW());

    double scanmiddlepoint = fCurVals[fParams[i]];

    // Determine N points needed
    double limlow  = fMinVals[fParams[i]];
    double limhigh = fMaxVals[fParams[i]];
    double step    = fStepVals[fParams[i]];

    int npoints = int( fabs(limhigh - limlow) / (step + 0.) );

    TH1D* contour = new TH1D(("Chi2Scan1D_" + fParams[i]).c_str(),
                             ("Chi2Scan1D_" + fParams[i] +
                              ";" + fParams[i]).c_str(),
                             npoints, limlow, limhigh);

    // Fill bins
    for (int x = 0; x < contour->GetNbinsX(); x++) {

      // Set X Val
      fCurVals[fParams[i]] = contour->GetXaxis()->GetBinCenter(x + 1);

      // Run Eval
      double *vals = FitUtils::GetArrayFromMap( fParams, fCurVals );
      double  chi2 = fSampleFCN->DoEval( vals );
      delete vals;

      // Fill Contour
      contour->SetBinContent(x + 1, chi2);
    }

    // Save contour
    contour->Write();

    // Reset Parameter
    fCurVals[fParams[i]] = scanmiddlepoint;

    // Save TTree
    fSampleFCN->WriteIterationTree();
  }

  return;
}

//*************************************
void MinimizerRoutines::Chi2Scan2D() {
//*************************************

  // Chi2 Scan 2D
  // Creates a 2D chi2 scan by stepping through all free parameters
  // Works for all pairwise combos of free parameters

  // Scan I
  for (UInt_t i = 0; i < fParams.size(); i++) {
    if (fFixVals[fParams[i]]) continue;

    // Scan J
    for (UInt_t j = 0; j < i; j++) {
      if (fFixVals[fParams[j]]) continue;

      fSampleFCN->CreateIterationTree( fParams[i] + "_" +
                                       fParams[j] + "_" +
                                       "scan2D_iterations",
                                       FitBase::GetRW() );

      double scanmid_i = fCurVals[fParams[i]];
      double scanmid_j = fCurVals[fParams[j]];

      double limlow_i  = fMinVals[fParams[i]];
      double limhigh_i = fMaxVals[fParams[i]];
      double step_i    = fStepVals[fParams[i]];

      double limlow_j  = fMinVals[fParams[j]];
      double limhigh_j = fMaxVals[fParams[j]];
      double step_j    = fStepVals[fParams[j]];

      int npoints_i = int( fabs(limhigh_i - limlow_i) / (step_i + 0.) ) + 1;
      int npoints_j = int( fabs(limhigh_j - limlow_j) / (step_j + 0.) ) + 1;

      TH2D* contour = new TH2D(("Chi2Scan2D_" + fParams[i] + "_" + fParams[j]).c_str(),
                               ("Chi2Scan2D_" + fParams[i] + "_" + fParams[j] +
                                ";" + fParams[i] + ";" + fParams[j]).c_str(),
                               npoints_i, limlow_i, limhigh_i,
                               npoints_j, limlow_j, limhigh_j );

      // Begin Scan
      LOG(FIT) << "Running scan for " << fParams[i] << " " << fParams[j] << std::endl;

      // Fill bins
      for (int x = 0; x < contour->GetNbinsX(); x++) {

        // Set X Val
        fCurVals[fParams[i]] = contour->GetXaxis()->GetBinCenter(x + 1);

        // Loop Y
        for (int y = 0; y < contour->GetNbinsY(); y++) {

          // Set Y Val
          fCurVals[fParams[j]] = contour->GetYaxis()->GetBinCenter(y + 1);

          // Run Eval
          double *vals = FitUtils::GetArrayFromMap( fParams, fCurVals );
          double  chi2 = fSampleFCN->DoEval( vals );
          delete vals;

          // Fill Contour
          contour->SetBinContent(x + 1, y + 1, chi2);

          fCurVals[fParams[j]] = scanmid_j;
        }

        fCurVals[fParams[i]] = scanmid_i;
        fCurVals[fParams[j]] = scanmid_j;
      }

      // Save contour
      contour->Write();

      // Save Iterations
      fSampleFCN->WriteIterationTree();

    }
  }

  return;
}

//*************************************
void MinimizerRoutines::CreateContours() {
//*************************************

  // Use MINUIT for this if possible
  ERR(FTL) << " Contours not yet implemented as it is really slow!" << std::endl;
  throw;

  return;
}

//*************************************
int MinimizerRoutines::FixAtLimit() {
//*************************************

  bool fixedparam = false;
  for (UInt_t i = 0; i < fParams.size(); i++) {
    std::string syst = fParams.at(i);
    if (fFixVals[syst]) continue;

    double curVal = fCurVals.at(syst);
    double minVal = fMinVals.at(syst);
    double maxVal = fMinVals.at(syst);

    if (fabs(curVal - minVal) < 0.0001) {
      fCurVals[syst] = minVal;
      fFixVals[syst] = true;
      fixedparam = true;
    }

    if (fabs(maxVal - curVal) < 0.0001) {
      fCurVals[syst] = maxVal;
      fFixVals[syst] = true;
      fixedparam = true;
    }
  }

  if (!fixedparam) {
    LOG(FIT) << "No dials needed fixing!" << std::endl;
    return kNoChange;
  } else return kStateChange;
}


/*
  Write Functions
*/
//*************************************
void MinimizerRoutines::SaveResults() {
//*************************************

  fOutputRootFile->cd();

  if (fMinimizer) {
    SetupCovariance();
    SaveMinimizerState();
  }

  SaveCurrentState();

}

//*************************************
void MinimizerRoutines::SaveMinimizerState() {
//*************************************

  std::cout << "Saving Minimizer State" << std::endl;
  if (!fMinimizer) {
    ERR(FTL) << "Can't save minimizer state without min object" << std::endl;
    throw;
  }

  // Save main fit tree
  fSampleFCN->WriteIterationTree();
  
  // Get Vals and Errors
  GetMinimizerState();

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
  for (UInt_t i = 0; i < fParams.size(); i++) {
    std::string name = fParams.at(i);
    nameVect    .push_back( name );

    valVect     .push_back( fCurVals.at(name)   );

    errVect     .push_back( fErrorVals.at(name) );

    minVect     .push_back( fMinVals.at(name)   );

    maxVect     .push_back( fMaxVals.at(name)   );

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
  TTree* fit_tree = new TTree("fit_result", "fit_result");
  fit_tree->Branch("parameter_names", &nameVect);
  fit_tree->Branch("parameter_values", &valVect);
  fit_tree->Branch("parameter_errors", &errVect);
  fit_tree->Branch("parameter_min", &minVect);
  fit_tree->Branch("parameter_max", &maxVect);
  fit_tree->Branch("parameter_start", &startVect);
  fit_tree->Branch("parameter_fix", &endfixVect);
  fit_tree->Branch("parameter_startfix", &startfixVect);
  fit_tree->Branch("CHI2", &CHI2, "CHI2/D");
  fit_tree->Branch("NDOF", &NDOF, "NDOF/I");
  fit_tree->Branch("NBINS", &NBINS, "NBINS/I");
  fit_tree->Branch("NDIM", &NDIM, "NDIM/I");
  fit_tree->Branch("NFREE", &NFREE, "NFREE/I");
  fit_tree->Fill();
  fit_tree->Write();

  // Make dial variables
  TH1D dialvar  = TH1D("fit_dials", "fit_dials", NPARS, 0, NPARS);
  TH1D startvar = TH1D("start_dials", "start_dials", NPARS, 0, NPARS);
  TH1D minvar   = TH1D("min_dials", "min_dials", NPARS, 0, NPARS);
  TH1D maxvar   = TH1D("max_dials", "max_dials", NPARS, 0, NPARS);

  TH1D dialvarfree  = TH1D("fit_dials_free", "fit_dials_free", NFREE, 0, NFREE);
  TH1D startvarfree = TH1D("start_dials_free", "start_dials_free", NFREE, 0, NFREE);
  TH1D minvarfree   = TH1D("min_dials_free", "min_dials_free", NFREE, 0, NFREE);
  TH1D maxvarfree   = TH1D("max_dials_free", "max_dials_free", NFREE, 0, NFREE);

  int freecount = 0;

  for (UInt_t i = 0; i < nameVect.size(); i++) {
    std::string name = nameVect.at(i);

    dialvar.SetBinContent(i + 1, valVect.at(i));
    dialvar.SetBinError(i + 1, errVect.at(i));
    dialvar.GetXaxis()->SetBinLabel(i + 1, name.c_str());

    startvar.SetBinContent(i + 1, startVect.at(i));
    startvar.GetXaxis()->SetBinLabel(i + 1, name.c_str());

    minvar.SetBinContent(i + 1,   minVect.at(i));
    minvar.GetXaxis()->SetBinLabel(i + 1, name.c_str());

    maxvar.SetBinContent(i + 1,   maxVect.at(i));
    maxvar.GetXaxis()->SetBinLabel(i + 1, name.c_str());

    if (NFREE > 0) {
      if (!startfixVect.at(i)) {
        freecount++;

        dialvarfree.SetBinContent(freecount, valVect.at(i));
        dialvarfree.SetBinError(freecount, errVect.at(i));
        dialvarfree.GetXaxis()->SetBinLabel(freecount, name.c_str());

        startvarfree.SetBinContent(freecount, startVect.at(i));
        startvarfree.GetXaxis()->SetBinLabel(freecount, name.c_str());

        minvarfree.SetBinContent(freecount,   minVect.at(i));
        minvarfree.GetXaxis()->SetBinLabel(freecount, name.c_str());

        maxvarfree.SetBinContent(freecount,   maxVect.at(i));
        maxvarfree.GetXaxis()->SetBinLabel(freecount, name.c_str());

      }
    }
  }

  // Save Dial Plots
  dialvar.Write();
  startvar.Write();
  minvar.Write();
  maxvar.Write();

  if (NFREE > 0) {
    dialvarfree.Write();
    startvarfree.Write();
    minvarfree.Write();
    maxvarfree.Write();
  }

  // Save fit_status plot
  TH1D statusplot = TH1D("fit_status", "fit_status", 8, 0, 8);
  std::string fit_labels[8] = {"status", "cov_status",  \
                               "maxiter", "maxfunc",  \
                               "iter",    "func", \
                               "precision", "tolerance"
                              };
  double fit_vals[8];
  fit_vals[0] = fMinimizer->Status() + 0.;
  fit_vals[1] = fMinimizer->CovMatrixStatus() + 0.;
  fit_vals[2] = fMinimizer->MaxIterations() + 0.;
  fit_vals[3] = fMinimizer->MaxFunctionCalls() + 0.;
  fit_vals[4] = fMinimizer->NIterations() + 0.;
  fit_vals[5] = fMinimizer->NCalls() + 0.;
  fit_vals[6] = fMinimizer->Precision() + 0.;
  fit_vals[7] = fMinimizer->Tolerance() + 0.;

  for (int i = 0; i < 8; i++) {
    statusplot.SetBinContent(i + 1, fit_vals[i]);
    statusplot.GetXaxis()->SetBinLabel(i + 1, fit_labels[i].c_str());
  }

  statusplot.Write();

  // Save Covars
  if (fCovar) fCovar->Write();
  if (fCovFree) fCovFree->Write();
  if (fCorrel) fCorrel->Write();
  if (fCorFree) fCorFree->Write();
  if (fDecomp) fDecomp->Write();
  if (fDecFree) fDecFree->Write();

  return;
}

//*************************************
void MinimizerRoutines::SaveCurrentState(std::string subdir) {
//*************************************

  LOG(FIT) << "Saving current full FCN predictions" << std::endl;

  // Setup DIRS
  TDirectory* curdir = gDirectory;
  if (!subdir.empty()) {
    TDirectory* newdir = (TDirectory*) gDirectory->mkdir(subdir.c_str());
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
void MinimizerRoutines::SaveNominal() {
//*************************************

  fOutputRootFile->cd();

  LOG(FIT) << "Saving Nominal Predictions (be cautious with this)" << std::endl;
  FitBase::GetRW()->Reconfigure();
  SaveCurrentState("nominal");

};

//*************************************
void MinimizerRoutines::SavePrefit() {
//*************************************

  fOutputRootFile->cd();

  LOG(FIT) << "Saving Prefit Predictions" << std::endl;
  UpdateRWEngine(fStartVals);
  SaveCurrentState("prefit");
  UpdateRWEngine(fCurVals);

};


/*
  MISC Functions
*/
//*************************************
int MinimizerRoutines::GetStatus() {
//*************************************

  return 0;
}

//*************************************
void MinimizerRoutines::SetupCovariance() {
//*************************************

  // Remove covares if they exist
  if (fCovar) delete fCovar;
  if (fCovFree) delete fCovFree;
  if (fCorrel) delete fCorrel;
  if (fCorFree) delete fCorFree;
  if (fDecomp) delete fDecomp;
  if (fDecFree) delete fDecFree;

  LOG(FIT) << "Building covariance matrix.." << std::endl;

  int NFREE = 0;
  int NDIM = 0;


  // Get NFREE from min or from vals (for cases when doing throws)
  if (fMinimizer) {
    std::cout << "NFREE FROM MINIMIZER" << std::endl;
    NFREE = fMinimizer->NFree();
    NDIM  = fMinimizer->NDim();
  } else {
    NDIM = fParams.size();
    for (UInt_t i = 0; i < fParams.size(); i++) {
      std::cout << "Getting Param " << fParams[i] << std::endl;

      if (!fFixVals[fParams[i]]) NFREE++;
    }
  }

  if (NDIM == 0) return;
  LOG(FIT) << "NFREE == " << NFREE << std::endl;
  fCovar = new TH2D("covariance", "covariance", NDIM, 0, NDIM, NDIM, 0, NDIM);
  if (NFREE > 0) {
    fCovFree = new TH2D("covariance_free",
                        "covariance_free",
                        NFREE, 0, NFREE,
                        NFREE, 0, NFREE);
  } else {
    fCovFree = NULL;
  }

  // Set Bin Labels
  int countall = 0;
  int countfree = 0;
  for (UInt_t i = 0; i < fParams.size(); i++) {

    std::cout << "Getting Param " << i << std::endl;
    std::cout << "ParamI = " << fParams[i] << std::endl;

    fCovar->GetXaxis()->SetBinLabel(countall + 1, fParams[i].c_str());
    fCovar->GetYaxis()->SetBinLabel(countall + 1, fParams[i].c_str());
    countall++;

    if (!fFixVals[fParams[i]] and NFREE > 0) {
      fCovFree->GetXaxis()->SetBinLabel(countfree + 1, fParams[i].c_str());
      fCovFree->GetYaxis()->SetBinLabel(countfree + 1, fParams[i].c_str());
      countfree++;
    }
  }

  std::cout << "Filling Matrices" << std::endl;

  fCorrel = PlotUtils::GetCorrelationPlot(fCovar, "correlation");
  fDecomp = PlotUtils::GetDecompPlot(fCovar, "decomposition");

  if (NFREE > 0) {
    fCorFree = PlotUtils::GetCorrelationPlot(fCovFree, "correlation_free");
    fDecFree = PlotUtils::GetDecompPlot(fCovFree, "decomposition_free");
  } else {
    fCorFree = NULL;
    fDecFree = NULL;
  }

  std::cout << " Set the covariance" << std::endl;
  return;
};

//*************************************
void MinimizerRoutines::ThrowCovariance(bool uniformly) {
//*************************************
  std::vector<double> rands;

  if (!fDecFree) {
    ERR(WRN) << "Trying to throw 0 free parameters" << std::endl;
    return;
  }

  // Generate Random Gaussians
  for (Int_t i = 0; i < fDecFree->GetNbinsX(); i++) {
    rands.push_back(gRandom->Gaus(0.0, 1.0));
  }

  // Reset Thrown Values
  for (UInt_t i = 0; i < fParams.size(); i++) {
    fThrownVals[fParams[i]] = fCurVals[fParams[i]];
  }

  // Loop and get decomp
  for (Int_t i = 0; i < fDecFree->GetNbinsX(); i++) {

    std::string parname = std::string(fDecFree->GetXaxis()->GetBinLabel(i + 1));
    double mod = 0.0;

    if (!uniformly) {
      for (Int_t j = 0; j < fDecFree->GetNbinsY(); j++) {
        mod += rands[j] * fDecFree->GetBinContent(j + 1, i + 1);
      }
    }

    if (fCurVals.find(parname) != fCurVals.end()) {

      if (uniformly) fThrownVals[parname] = gRandom->Uniform(fMinVals[parname], fMaxVals[parname]);
      else {  fThrownVals[parname] =    fCurVals[parname] + mod; }

    }
  }

  // Check Limits
  for (UInt_t i = 0; i < fParams.size(); i++) {
    std::string syst = fParams[i];
    if (fFixVals[syst]) continue;
    if (fThrownVals[syst] < fMinVals[syst]) fThrownVals[syst] = fMinVals[syst];
    if (fThrownVals[syst] > fMaxVals[syst]) fThrownVals[syst] = fMaxVals[syst];
  }

  return;
};

//*************************************
void MinimizerRoutines::GenerateErrorBands() {
//*************************************

  TDirectory* errorDIR = (TDirectory*) fOutputRootFile->mkdir("error_bands");
  errorDIR->cd();

  // Make a second file to store throws 
  std::string tempFileName = fOutputFile;
  if (tempFileName.find(".root") != std::string::npos) tempFileName.erase(tempFileName.find(".root"), 5);
  tempFileName += ".throws.root";
  TFile* tempfile = new TFile(tempFileName.c_str(),"RECREATE");

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
  TTree* parameterTree = new TTree("throws", "throws");
  double chi2;
  for (UInt_t i = 0; i < fParams.size(); i++)
    parameterTree->Branch(fParams[i].c_str(), &fThrownVals[fParams[i]], (fParams[i] + "/D").c_str());
  parameterTree->Branch("chi2", &chi2, "chi2/D");


  bool uniformly = FitPar::Config().GetParB("error_uniform");

  // Run Throws and save
  for (Int_t i = 0; i < nthrows; i++) {

    TDirectory* throwfolder = (TDirectory*)tempfile->mkdir(Form("throw_%i", i));
    throwfolder->cd();

    // Generate Random Parameter Throw
    ThrowCovariance(uniformly);

    // Run Eval
    double *vals = FitUtils::GetArrayFromMap( fParams, fThrownVals );
    chi2 = fSampleFCN->DoEval( vals );
    delete vals;

    // Save the FCN
    fSampleFCN->Write();

    parameterTree->Fill();
  }

  errorDIR->cd();
  fDecFree->Write();
  fCovFree->Write();
  parameterTree->Write();

  delete parameterTree;

  // Now go through the keys in the temporary file and look for TH1D, and TH2D plots
  TIter next(nominal->GetListOfKeys());
  TKey *key;
  while ((key = (TKey*)next())) {
    TClass *cl = gROOT->GetClass(key->GetClassName());
    if (!cl->InheritsFrom("TH1D") and !cl->InheritsFrom("TH2D")) continue;
    TH1D *baseplot = (TH1D*)key->ReadObj();
    std::string plotname = std::string(baseplot->GetName());

    int nbins = baseplot->GetNbinsX() * baseplot->GetNbinsY();

    // Setup TProfile with RMS option
    TProfile* tprof = new TProfile((plotname + "_prof").c_str(), (plotname + "_prof").c_str(), nbins, 0, nbins, "S");

    // Setup The TTREE
    double* bincontents;
    bincontents = new double[nbins];

    double* binlowest;
    binlowest = new double[nbins];

    double* binhighest;
    binhighest = new double[nbins];

    errorDIR->cd();
    TTree* bintree = new TTree((plotname + "_tree").c_str(), (plotname + "_tree").c_str());
    for (Int_t i = 0; i < nbins; i++) {
      bincontents[i] = 0.0;
      binhighest[i] = 0.0;
      binlowest[i] = 0.0;
      bintree->Branch(Form("content_%i", i), &bincontents[i], Form("content_%i/D", i));
    }

    for (Int_t i = 0; i < nthrows; i++) {
      TH1* newplot = (TH1*)tempfile->Get(Form(("throw_%i/" + plotname).c_str(), i));

      for (Int_t j = 0; j < nbins; j++) {
        tprof->Fill(j + 0.5, newplot->GetBinContent(j + 1));
        bincontents[j] = newplot->GetBinContent(j + 1);

        if (bincontents[j] < binlowest[j] or i == 0) binlowest[j] = bincontents[j];
        if (bincontents[j] > binhighest[j] or i == 0) binhighest[j] = bincontents[j];
      }

      errorDIR->cd();
      bintree->Fill();

      delete newplot;
    }

    errorDIR->cd();

    for (Int_t j = 0; j < nbins; j++) {

      if (!uniformly) {
        baseplot->SetBinError(j + 1, tprof->GetBinError(j + 1));

      } else {
        baseplot->SetBinContent(j + 1, (binlowest[j] + binhighest[j]) / 2.0);
        baseplot->SetBinError(j + 1, (binhighest[j] - binlowest[j]) / 2.0);
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
