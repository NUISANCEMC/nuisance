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
#include "BayesianRoutines.h"

void BayesianRoutines::Init() {

  fInputFile = "";
  fInputRootFile = NULL;

  fOutputFile = "";
  fOutputRootFile = NULL;

  fStrategy = "BayesianThrows";
  fRoutines.clear();
  fRoutines.push_back("BayesianThrows");

  fCardFile = "";

  fFakeDataInput = "";

  fSampleFCN    = NULL;

  fAllowedRoutines = ("f");

};

BayesianRoutines::~BayesianRoutines() {
};

BayesianRoutines::BayesianRoutines(int argc, char* argv[]) {

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
  ParserUtils::ParseArgument(args, "-t", fNThrows, false, false);
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
  if (maxevents.compare("-1")) {
    configuration.OverrideConfig("MAXEVENTS=" + maxevents);
  }

  // Finish configuration XML
  configuration.FinaliseConfig(fCompKey.GetS("outputfile") + ".xml");

  // Add Error Verbo Lines
  verbocount += Config::Get().GetParI("VERBOSITY");
  errorcount += Config::Get().GetParI("ERROR");
  std::cout << "[ NUISANCE ]: Setting VERBOSITY=" << verbocount << std::endl;
  std::cout << "[ NUISANCE ]: Setting ERROR=" << errorcount << std::endl;
  SETVERBOSITY(verbocount);

  // Proper Setup
  if (fStrategy.find("ErrorBands") != std::string::npos ||
      fStrategy.find("MergeErrors") != std::string::npos) {
    fOutputRootFile = new TFile(fCompKey.GetS("outputfile").c_str(), "RECREATE");
  }

  //  fOutputRootFile = new TFile(fCompKey.GetS("outputfile").c_str(), "RECREATE");
  SetupSystematicsFromXML();

  SetupRWEngine();
  SetupFCN();

  return;
};

void BayesianRoutines::SetupSystematicsFromXML() {

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
    if (!key.Has("state")) {
      key.SetS("state", "FIX");
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
void BayesianRoutines::SetupRWEngine() {
//*************************************

  for (UInt_t i = 0; i < fParams.size(); i++) {
    std::string name = fParams[i];
    FitBase::GetRW() -> IncludeDial(name, fTypeVals.at(name) );
  }
  UpdateRWEngine(fStartVals);

  return;
}

//*************************************
void BayesianRoutines::SetupFCN() {
//*************************************

  LOG(FIT) << "Making the jointFCN" << std::endl;
  if (fSampleFCN) delete fSampleFCN;
  fSampleFCN = new JointFCN(fOutputRootFile);

  fInputThrows = fSampleFCN->GetPullList();

  return;
}

/*
  Fitting Functions
*/
//*************************************
void BayesianRoutines::UpdateRWEngine(std::map<std::string, double>& updateVals) {
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
void BayesianRoutines::ThrowParameters() {
//*************************************

  // Set fThrownVals to all values in currentVals
  for (UInt_t i = 0; i < fParams.size(); i++) {
    std::string name = fParams.at(i);
    fThrownVals[name] = fCurVals[name];
  }

  for (PullListConstIter iter = fInputThrows.begin();
       iter != fInputThrows.end(); iter++) {
    ParamPull* pull = *iter;

    pull->ThrowCovariance();
    TH1D dialhist = pull->GetDataHist();

    for (int i = 0; i < dialhist.GetNbinsX(); i++) {
      std::string name = std::string(dialhist.GetXaxis()->GetBinLabel(i + 1));
      if (fCurVals.find(name) != fCurVals.end()) {
        fThrownVals[name] = dialhist.GetBinContent(i + 1);
      }
    }

    // Reset throw incase pulls are calculated.
    pull->ResetToy();

  }

  // Now update Parameters
  UpdateRWEngine(fThrownVals);

  // Update Pulls
  for (PullListConstIter iter = fInputThrows.begin();
       iter != fInputThrows.end(); iter++) {
    ParamPull* pull = *iter;
    pull->Reconfigure();
  }

  return;
};

//*************************************
void BayesianRoutines::Run() {
//*************************************

  std::cout << "Running routines " << std::endl;
  fRoutines = GeneralUtils::ParseToStr(fStrategy, ",");

  for (UInt_t i = 0; i < fRoutines.size(); i++) {

    std::string routine = fRoutines.at(i);
    LOG(FIT) << "Running Routine: " << routine << std::endl;

    if (!routine.compare("BayesianThrows")) GenerateThrows();
    else THROW("UNKNOWN ROUTINE " << routine);
  }

  return;
}

//*************************************
void BayesianRoutines::GenerateThrows() {
//*************************************

  // Create a new output file
  TFile* outfile = new TFile((fOutputFile + ".throws.root").c_str(), "RECREATE");
  outfile->cd();

  int nthrows = fNThrows;

  // Setting Seed
  // Matteo Mazzanti's Fix
  struct timeval mytime;
  gettimeofday(&mytime, NULL);
  Double_t seed = time(NULL) + int(getpid()) + (mytime.tv_sec * 1000.) + (mytime.tv_usec / 1000.);
  gRandom->SetSeed(seed);
  LOG(FIT) << "Using Seed : " << seed << std::endl;
  LOG(FIT) << "nthrows = " << nthrows << std::endl;

  // Run the Initial Reconfigure
  LOG(FIT) << "Making nominal prediction " << std::endl;
  TDirectory* nominal = (TDirectory*) outfile->mkdir("nominal");
  nominal->cd();
  UpdateRWEngine(fStartVals);
  fSampleFCN->ReconfigureUsingManager();
  fSampleFCN->Write();

  // Create an iteration tree inside SampleFCN
  fSampleFCN->CreateIterationTree("error_iterations", FitBase::GetRW());

  // Create a new iteration TTree
  TTree* LIKETREE = new TTree("likelihood", "likelihood");
  std::vector<std::string> likenames = fSampleFCN->GetAllNames();
  std::vector<double>      likevals  = fSampleFCN->GetAllLikelihoods();
  std::vector<int>         likendof  = fSampleFCN->GetAllNDOF();
  double* LIKEVALS = new double[likevals.size()];
  int* LIKENDOF = new int[likendof.size()];

  for (size_t i = 0; i < likendof.size(); i++) {
    LIKETREE->Branch( (likenames[i] + "_likelihood"  ).c_str(), &LIKEVALS[i], 
                      (likenames[i] + "_likelihood/D").c_str() );
    LIKETREE->Branch( (likenames[i] + "_ndof"  ).c_str(),       &LIKENDOF[i], 
                      (likenames[i] + "_ndof/I").c_str()       );
    LIKENDOF[i] = likendof[i];
  }

  likenames .clear();
  likevals  .clear();
  likendof  .clear();

  double* PARAMVALS = new double[fParams.size()];
  for (size_t i = 0; i < fParams.size(); i++){
    LIKETREE->Branch( fParams[i].c_str(), &PARAMVALS[i], (fParams[i] + "/D").c_str() );
  }

  // Run Throws and save
  for (Int_t i = 0; i < nthrows; i++) {

    // Skip the start throw
    if (i == 0) continue;
    LOG(FIT) << "Throw " << i << " ================================" << std::endl;

    // Throw Parameters
    ThrowParameters();
    FitBase::GetRW()->Print();

    // Get Parameter Values
    for (size_t i = 0; i < fParams.size(); i++){
      PARAMVALS[i] = fThrownVals[fParams[i]];
    }

    // Run Sample Prediction
    fSampleFCN->ReconfigureFastUsingManager();

    // Get vector of likelihoods/ndof
    std::vector<double> likevals = fSampleFCN->GetAllLikelihoods();
    for (size_t i = 0; i < likevals.size(); i++) {
      LIKEVALS[i] = likevals[i];
    }

    // Save to TTree
    LIKETREE->Fill();

    // Save the FCN
    // if (fSavePredictions){ SaveSamplePredictions(); }
    LOG(FIT) << "END OF THROW ================================" << std::endl;

  }

  // Finish up
  outfile->cd();
  LIKETREE->Write();
  outfile->Close();
  delete LIKEVALS;
  delete LIKENDOF;
  delete PARAMVALS;
}
