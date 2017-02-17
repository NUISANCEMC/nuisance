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

#include "ComparisonRoutines.h"

/*
  Constructor/Destructor
*/
//************************
void ComparisonRoutines::Init() {
  //************************

  fInputFile = "";
  fInputRootFile = NULL;

  fOutputFile = "";
  fOutputRootFile = NULL;

  fStrategy = "Compare";
  fRoutines.clear();

  fCardFile = "";

  fFakeDataInput = "";

  fSampleFCN = NULL;

  fAllowedRoutines = ("Compare");
};

//*************************************
ComparisonRoutines::~ComparisonRoutines(){
    //*************************************
};

/*
  Input Functions
*/
//*************************************
ComparisonRoutines::ComparisonRoutines(int argc, char* argv[]) {
  //*************************************

  // Set everything to defaults
  Init();
  std::vector<std::string> configs_cmd;
  std::string maxevents_flag = "";
  std::string xmlinput = "";
  int verbosity_flag = 0;
  int error_flag = 0;

  // If No Arguments print commands
  for (int i = 1; i < argc; ++i) {
    if (i + 1 != argc) {
      // Cardfile
      if (!std::strcmp(argv[i], "-c")) {
        fCardFile = argv[i + 1];
        ++i;
      } else if (!std::strcmp(argv[i], "-o")) {
        fOutputFile = argv[i + 1];
        ++i;
      } else if (!std::strcmp(argv[i], "-f")) {
        fStrategy = argv[i + 1];
        ++i;
      } else if (!std::strcmp(argv[i], "-q")) {
        configs_cmd.push_back(argv[i + 1]);
        ++i;
      } else if (!std::strcmp(argv[i], "-n")) {
        maxevents_flag = argv[i + 1];
        ++i;
      } else if (!std::strcmp(argv[i], "-v")) {
        verbosity_flag -= 1;
      } else if (!std::strcmp(argv[i], "+v")) {
        verbosity_flag += 1;
      } else if (!std::strcmp(argv[i], "-e")) {
        error_flag -= 1;
      } else if (!std::strcmp(argv[i], "+e")) {
        error_flag += 1;
      } else if (!std::strcmp(argv[i], "-x")) {
	      xmlinput = argv[i + 1];
        ++i;
      } else {
        ERR(FTL) << "ERROR: unknown command line option given! - '" << argv[i]
                 << " " << argv[i + 1] << "'" << std::endl;
        throw;
      }
    }
  }

  // Fill fit routines and check they are good
  fRoutines = GeneralUtils::ParseToStr(fStrategy, ",");
  for (UInt_t i = 0; i < fRoutines.size(); i++) {
    if (fAllowedRoutines.find(fRoutines[i]) == std::string::npos) {
      ERR(FTL) << "Unknown fit routine given! "
               << "Must be provided as a comma seperated list." << std::endl;
      ERR(FTL) << "Allowed Routines: " << fAllowedRoutines << std::endl;
      throw;
    }
  }

  // CONFIG
  // ---------------------------
  // Get Config()
  nuisconfig conf = Config::Get();
  if (!xmlinput.empty()){
    conf.LoadConfig( xmlinput, "xmlinput" );
  }

  std::string par_dir = GeneralUtils::GetTopLevelDir() + "/parameters/";
  FitPar::Config().ReadParamFile(par_dir + "config.list.dat");
  if (!fCardFile.empty()) FitPar::Config().ReadParamFile(fCardFile);

  for (UInt_t iter = 0; iter < configs_cmd.size(); iter++) {
    FitPar::Config().ForceParam(configs_cmd[iter]);
  }

  if (!maxevents_flag.empty()) {
    FitPar::Config().SetParI("input.maxevents", atoi(maxevents_flag.c_str()));
  }

  if (verbosity_flag != 0) {
    int curverb = FitPar::Config().GetParI("VERBOSITY");
    FitPar::Config().SetParI("VERBOSITY", curverb + verbosity_flag);
  }

  if (error_flag != 0) {
    int curwarn = FitPar::Config().GetParI("ERROR");
    FitPar::Config().SetParI("ERROR", curwarn + error_flag);
  }

  LOG_VERB(FitPar::Config().GetParI("VERBOSITY"));
  ERR_VERB(FitPar::Config().GetParI("ERROR"));

  // CARD
  // ---------------------------
  // Parse Card Options
  if (!fCardFile.empty()) ReadCard(fCardFile);
  if (!xmlinput.empty())  ReadXML(xmlinput);
  conf.WriteConfig( fOutputFile + ".xml" );

  // Outputs
  // ---------------------------
  // Save Configs to output file
  fOutputRootFile = new TFile(fOutputFile.c_str(), "RECREATE");
  FitPar::Config().Write();

  // Starting Setup
  // ---------------------------
  SetupRWEngine();
  SetupFCN();

  return;
};

//*************************************  
void ComparisonRoutines::ReadXML(std::string cardfile){
//*************************************  

  // Setup Parameters
  std::vector<nuiskey> parkeys = Config::QueryKeys("parameter");
  for (int i = 0; i < parkeys.size(); i++){
    nuiskey key = parkeys.at(i);

    // Get Inputs
    std::string partype = key.GetS("type");
    std::string parname = key.GetS("name");
    double parnom = key.GetD("nom");
    double parlow = key.GetD("low");
    double parhig = key.GetD("high");
      
    std::cout << "Read Parameter " << partype << " " << parname << " " << parnom << " " << parlow << " " << parhig << std::endl;

  }

  // Setup Samples
  std::vector<nuiskey> samplekeys =  Config::QueryKeys("sample");
  for (size_t i = 0; i < samplekeys.size(); i++){
    nuiskey key = samplekeys.at(i);

    // Get Sample Options
    std::string samplename = key.GetS("name");
    std::string samplefile = key.GetS("input");

    std::string sampletype = 
      key.Has("type") ? key.GetS("type") : "DEFAULT";

    double samplenorm = 
      key.Has("norm") ? key.GetD("norm") : 1.0;

    // If FREE add to parameters otherwise continue
    if (sampletype.find("FREE") == std::string::npos){
      continue;
    }

    // Form norm dial from samplename + sampletype + "_norm";
    std::string normname = samplename + sampletype + "_norm";

    // Check normname not already present
    if (fTypeVals.find("normname") != fTypeVals.end()){
      continue;
    }

    // Add new norm dial to list if its passed above checks
    fParams.push_back(normname);

    fTypeVals[normname] = kNORM;
    fStateVals[normname] = sampletype;
    fCurVals[normname] = samplenorm;

  }

}

//*************************************
void ComparisonRoutines::ReadCard(std::string cardfile) {
//*************************************
  Config::ConvertAndLoadCardToXMLFormat(cardfile);
}
/*
  // If a card file is provided add input to global config

  // Read cardlines into vector
  std::vector<std::string> cardlines =
      GeneralUtils::ParseFileToStr(cardfile, "\n");
  FitPar::Config().cardLines = cardlines;

  // Read Samples first (norm params can be overridden)
  int linecount = 0;
  for (std::vector<std::string>::iterator iter = cardlines.begin();
       iter != cardlines.end(); iter++) {
    std::string line = (*iter);
    linecount++;

    // Skip Comments
    if (line.empty()) continue;
    if (line.c_str()[0] == '#') continue;

    // Read Valid Samples
    int samstatus = ReadSamples(line);

    // Show line if bad to help user
    if (samstatus == kErrorStatus) {
      ERR(FTL) << "Bad Input in cardfile " << fCardFile << " at line "
               << linecount << "!" << endl;
      ERR(FTL) << line << endl;
      throw;
    }
  }

  // Read Parameters second
  linecount = 0;
  for (std::vector<std::string>::iterator iter = cardlines.begin();
       iter != cardlines.end(); iter++) {
    std::string line = (*iter);
    linecount++;

    // Skip Comments
    if (line.empty()) continue;
    if (line.c_str()[0] == '#') continue;

    // Try Parameter Reads
    int parstatus = ReadParameters(line);
    int fakstatus = ReadFakeDataPars(line);

    // Show line if bad to help user
    if (parstatus == kErrorStatus || fakstatus == kErrorStatus) {
      ERR(FTL) << "Bad Parameter Input in cardfile " << fCardFile << " at line "
               << linecount << "!" << endl;
      ERR(FTL) << line << endl;
      throw;
    }
  }
*/

//*****************************************
int ComparisonRoutines::ReadParameters(std::string parstring) {
  //******************************************

  std::string inputspec =
      "RW Dial Inputs Syntax \n"
      "free input w/ limits: TYPE  NAME  START  MIN  MAX  STEP  [STATE] \n"
      "fix  input: TYPE  NAME  VALUE  [STATE] \n"
      "free input w/o limits: TYPE  NAME  START  FREE,[STATE] \n"
      "Allowed Types: \n"
      "neut_parameter,niwg_parameter,t2k_parameter,"
      "nuwro_parameter,gibuu_parameter";

  // Check sample input
  if (parstring.find("parameter") == std::string::npos) return kGoodStatus;

  // Parse inputs
  std::vector<std::string> strvct = GeneralUtils::ParseToStr(parstring, " ");

  // Skip if comment or parameter somewhere later in line
  if (strvct[0].c_str()[0] == '#' ||
      strvct[0].find("parameter") == std::string::npos) {
    return kGoodStatus;
  }

  // Check length
  if (strvct.size() < 3) {
    ERR(FTL) << "Input rw dials need to provide at least 3 inputs."
             << std::endl;
    std::cout << inputspec << std::endl;
    return kErrorStatus;
  }

  // Setup default inputs
  std::string partype = strvct[0];
  std::string parname = strvct[1];
  double parval = GeneralUtils::StrToDbl(strvct[2]);
  std::string state = "FIX";  //[DEFAULT]

  // Check Type
  if (FitBase::ConvDialType(partype) == kUNKNOWN) {
    ERR(FTL) << "Unknown parameter type! " << partype << endl;
    std::cout << inputspec << std::endl;
    return kErrorStatus;
  }

  // Check Parameter Name
  if (FitBase::GetDialEnum(partype, parname) == -1) {
    ERR(FTL) << "Bad RW parameter name! " << partype << " " << parname << endl;
    std::cout << inputspec << std::endl;
    return kErrorStatus;
  }

  // Option Extra (No Limits)
  if (strvct.size() == 4) {
    state = strvct[3];
  }

  // Run Parameter Conversion if needed
  if (state.find("ABS") != std::string::npos) {
    parval = FitBase::RWAbsToSigma(partype, parname, parval);
  } else if (state.find("FRAC") != std::string::npos) {
    parval = FitBase::RWFracToSigma(partype, parname, parval);
  }

  // Check no repeat params
  if (std::find(fParams.begin(), fParams.end(), parname) != fParams.end()) {
    ERR(FTL) << "Duplicate parameter names given for " << parname << endl;
    throw;
  }

  // Setup Containers
  fParams.push_back(parname);

  fTypeVals[parname] = FitBase::ConvDialType(partype);
  fCurVals[parname] = parval;
  fStateVals[parname] = state;

  // Print the parameter
  LOG(MIN) << "Read Parameter " << parname << " " << parval << " " << state
           << std::endl;

  // Tell reader its all good
  return kGoodStatus;
}

//*******************************************
int ComparisonRoutines::ReadFakeDataPars(std::string parstring) {
  //******************************************

  std::string inputspec =
      "Fake Data Dial Inputs Syntax \n"
      "fake value: fake_parameter  NAME  VALUE  \n"
      "Name should match dialnames given in actual dial specification.";

  // Check sample input
  if (parstring.find("fake_parameter") == std::string::npos) return kGoodStatus;

  // Parse inputs
  std::vector<std::string> strvct = GeneralUtils::ParseToStr(parstring, " ");

  // Skip if comment or parameter somewhere later in line
  if (strvct[0].c_str()[0] == '#' || strvct[0] == "fake_parameter") {
    return kGoodStatus;
  }

  // Check length
  if (strvct.size() < 3) {
    ERR(FTL) << "Fake dials need to provide at least 3 inputs." << std::endl;
    std::cout << inputspec << std::endl;
    return kErrorStatus;
  }

  // Read Inputs
  std::string parname = strvct[1];
  double parval = GeneralUtils::StrToDbl(strvct[2]);

  // Setup Container
  fFakeVals[parname] = parval;

  // Print the fake parameter
  LOG(MIN) << "Read Fake Parameter " << parname << " " << parval << std::endl;

  // Tell reader its all good
  return kGoodStatus;
}

//******************************************
int ComparisonRoutines::ReadSamples(std::string samstring) {
  //******************************************

  const static std::string inputspec =
      "\tsample <sample_name> <input_type>:inputfile.root [OPTS] "
      "[norm]\nsample_name: Name "
      "of sample to include. e.g. MiniBooNE_CCQE_XSec_1DQ2_nu\ninput_type: The "
      "input event format. e.g. NEUT, GENIE, EVSPLN, ...\nOPTS: Additional, "
      "optional sample options.\nnorm: Additional, optional sample "
      "normalisation factor.";

  // Check sample input
  if (samstring.find("sample") == std::string::npos) return kGoodStatus;

  // Parse inputs
  std::vector<std::string> strvct = GeneralUtils::ParseToStr(samstring, " ");

  // Skip if comment or parameter somewhere later in line
  if (strvct[0].c_str()[0] == '#' || strvct[0] != "sample") {
    return kGoodStatus;
  }

  // Check length
  if (strvct.size() < 3) {
    ERR(FTL) << "Sample need to provide at least 3 inputs." << std::endl;
    std::cout << inputspec << std::endl;
    return kErrorStatus;
  }

  // Setup default inputs
  std::string samname = strvct[1];
  std::string samfile = strvct[2];

  if (samfile == "FIX") {
    ERR(FTL) << "Input filename was \"FIX\", this line is probably malformed "
                "in the input card file. Line:\'"
             << samstring << "\'" << std::endl;
    ERR(FTL) << "Expect sample lines to look like:\n\t" << inputspec
             << std::endl;

    throw;
  }

  std::string samtype = "DEFAULT";
  double samnorm = 1.0;

  // Optional Type
  if (strvct.size() > 3) {
    samtype = strvct[3];
    // Append the sample type to the normalsiation name
    samname += "_"+samtype;
    // Also get rid of the / and replace it with underscore because it might not be supported character
    while (samname.find("/") != std::string::npos) {
      samname.replace(samname.find("/"), 1, std::string("_"));
    }
  }

  // Optional Norm
  if (strvct.size() > 4) samnorm = GeneralUtils::StrToDbl(strvct[4]);

  // Add Sample Names as Norm Dials
  std::string normname = samname + "_norm";

  // Now match and check there are no repeated parameter names
  if (std::find(fParams.begin(), fParams.end(), normname) != fParams.end()) {
    ERR(FTL) << "Duplicate samples given for " << samname << endl;
    throw;
  }

  fParams.push_back(normname);

  fTypeVals[normname] = kNORM;
  fStateVals[normname] = samtype;
  fCurVals[normname] = samnorm;

  // Print read in
  LOG(MIN) << "Read sample " << samname << " " << samfile << " " << samtype
           << " " << samnorm << endl;

  // Tell reader its all good
  return kGoodStatus;
}

/*
  Setup Functions
*/
//*************************************
void ComparisonRoutines::SetupRWEngine() {
  //*************************************

  for (UInt_t i = 0; i < fParams.size(); i++) {
    std::string name = fParams[i];
    FitBase::GetRW()->IncludeDial(name, fTypeVals.at(name));
  }

  return;
}

//*************************************
void ComparisonRoutines::SetupFCN() {
  //*************************************

  LOG(FIT) << "Making the jointFCN" << std::endl;
  if (fSampleFCN) delete fSampleFCN;
  FitPar::Config().out = fOutputRootFile;
  fOutputRootFile->cd();
  fSampleFCN = new JointFCN(fOutputRootFile);
  SetFakeData();

  return;
}

//*************************************
void ComparisonRoutines::SetFakeData() {
  //*************************************

  if (fFakeDataInput.empty()) return;

  if (fFakeDataInput.compare("MC") == 0) {
    LOG(FIT) << "Setting fake data from MC starting prediction." << std::endl;
    UpdateRWEngine(fFakeVals);

    FitBase::GetRW()->Reconfigure();
    fSampleFCN->ReconfigureAllEvents();
    fSampleFCN->SetFakeData("MC");

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
void ComparisonRoutines::UpdateRWEngine(
    std::map<std::string, double>& updateVals) {
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
void ComparisonRoutines::Run() {
  //*************************************

  for (UInt_t i = 0; i < fRoutines.size(); i++) {
    std::string routine = fRoutines.at(i);
    // int fitstate = kFitUnfinished;

    LOG(FIT) << "Running Routine: " << routine << std::endl;
    if (routine == "Compare") {
      UpdateRWEngine(fCurVals);
      GenerateComparison();
      PrintState();
    }
  }

  return;
}

//*************************************
void ComparisonRoutines::GenerateComparison() {
  //*************************************

  // Main Event Loop from event Manager
  bool using_evtmanager = FitPar::Config().GetParB("EventManager");

  if (using_evtmanager and false) {
    LOG(FIT) << "Using Comparison Routines Event Manager" << endl;

    std::list<MeasurementBase*> samchain = fSampleFCN->GetSampleList();
    std::list<MeasurementBase*>::const_iterator iterSam = samchain.begin();

    std::map<int, InputHandler*> fInputs = FitBase::EvtManager().GetInputs();
    std::map<int, InputHandler*>::const_iterator iterInp = fInputs.begin();

    int timestart = time(NULL);

    for (; iterInp != fInputs.end(); iterInp++) {
      int input_id = (iterInp->first);
      InputHandler* cur_input = (iterInp->second);
      FitEvent* cust_event = cur_input->GetEventPointer();
      int fNEvents = cur_input->GetNEvents();
      int countwidth = (fNEvents / 10);

      // MAIN EVENT LOOP
      for (int i = 0; i < fNEvents; i++) {
        // Get Event from input list
        cust_event = FitBase::EvtManager().GetEvent(input_id, i);

        // Get Weight
        double Weight = (FitBase::GetRW()->CalcWeight(cust_event) *
                         cust_event->InputWeight);

        // Skip if dodgy weight
        if (fabs(cust_event->Mode) > 60 || cust_event->Mode == 0 ||
            Weight > 200.0 || Weight <= 0.0)
          continue;

        // Loop over samples and fill histograms
        iterSam = samchain.begin();
        for (; iterSam != samchain.end(); iterSam++) {
          MeasurementBase* exp = (*iterSam);
          if (exp->GetInputID() != input_id) continue;

          exp->FillEventVariables(cust_event);
          exp->SetMode(cust_event->Mode);
          exp->SetSignal(cust_event);
          exp->SetWeight(Weight);
          exp->FillHistograms();
        }

        // Print Out
        if (LOG_LEVEL(REC) and i % countwidth == 0)
          LOG(REC) << "Reconfigured " << i << " total events. W=" << Weight
                   << std::endl;
      }
    }

    // Convert Binned events
    iterSam = samchain.begin();
    for (; iterSam != samchain.end(); iterSam++) {
      MeasurementBase* exp = (*iterSam);
      exp->ConvertEventRates();
    }

    LOG(FIT) << "Time Taken = " << time(NULL) - timestart << std::endl;
    LOG(FIT) << "Finished reconfiguring all events" << std::endl;
  } else {
    fSampleFCN->ReconfigureAllEvents();
  }
}

//*************************************
void ComparisonRoutines::PrintState() {
  //*************************************
  LOG(FIT) << "------------" << std::endl;

  // Count max size
  int maxcount = 0;
  for (UInt_t i = 0; i < fParams.size(); i++) {
    maxcount = max(int(fParams[i].size()), maxcount);
  }

  // Header
  LOG(FIT) << " #    " << left << setw(maxcount) << "Parameter "
           << " = " << setw(10) << "Value"
           << " +- " << setw(10) << "Error"
           << " " << setw(8) << "(Units)"
           << " " << setw(10) << "Conv. Val"
           << " +- " << setw(10) << "Conv. Err"
           << " " << setw(8) << "(Units)" << std::endl;

  // Parameters
  for (UInt_t i = 0; i < fParams.size(); i++) {
    std::string syst = fParams.at(i);

    std::string typestr = FitBase::ConvDialType(fTypeVals[syst]);
    std::string curunits = "(sig.)";
    double curval = fCurVals[syst];
    double curerr = 0.0;

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
    double convval = FitBase::RWSigmaToAbs(typestr, syst, curval);
    double converr = (FitBase::RWSigmaToAbs(typestr, syst, curerr) -
                      FitBase::RWSigmaToAbs(typestr, syst, 0.0));

    std::ostringstream curparstring;

    curparstring << " " << setw(3) << left << i << ". " << setw(maxcount)
                 << syst << " = " << setw(10) << curval << " +- " << setw(10)
                 << curerr << " " << setw(8) << curunits << " " << setw(10)
                 << convval << " +- " << setw(10) << converr << " " << setw(8)
                 << convunits;

    LOG(FIT) << curparstring.str() << endl;
  }

  LOG(FIT) << "------------" << std::endl;
  double like = fSampleFCN->GetLikelihood();
  LOG(FIT) << std::left << std::setw(46) << "Likelihood for JointFCN: " << like << endl;
  LOG(FIT) << "------------" << std::endl;
}

/*
  Write Functions
*/
//*************************************
void ComparisonRoutines::SaveCurrentState(std::string subdir) {
  //*************************************

  LOG(FIT) << "Saving current full FCN predictions" << std::endl;

  // Setup DIRS
  TDirectory* curdir = gDirectory;
  if (!subdir.empty()) {
    TDirectory* newdir = (TDirectory*)gDirectory->mkdir(subdir.c_str());
    newdir->cd();
  }

  fSampleFCN->Write();

  // Change back to current DIR
  curdir->cd();

  return;
}

//*************************************
void ComparisonRoutines::SaveNominal() {
  //*************************************

  fOutputRootFile->cd();

  LOG(FIT) << "Saving Nominal Predictions (be cautious with this)" << std::endl;
  FitBase::GetRW()->Reconfigure();
  GenerateComparison();
  SaveCurrentState("nominal");
};

/*
  MISC Functions
*/
//*************************************
int ComparisonRoutines::GetStatus() {
  //*************************************

  return 0;
}
