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

#include "comparisonRoutines.h"

/*
  Constructor/Destructor
*/
//************************
comparisonRoutines::comparisonRoutines(int argc, char* argv[]) {
//************************

  // Set Defaults
  inputFileName = "";
  outputFileName = "";

  inputFile = NULL;
  outputFile = NULL;
  fitStrategy = "Compare";
  fakeDataFile = "";

  thisFCN = NULL;

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
void comparisonRoutines::parseArgs(int argc, char* argv[]) {
  //*************************************

  std::string maxevents_flag = "";
  int verbosity_flag = 0;
  int error_flag = 0;

  // If No Arguments print commands
  for (int i = 1; i < argc; ++i) {
    if (i + 1 != argc) {
      // Cardfile
      if (!std::strcmp(argv[i], "-c")) {
        cardFile = argv[i + 1];
        ++i;
      } else if (!std::strcmp(argv[i], "-o")) {
        outputFileName = argv[i + 1];
        ++i;
      } else if (!std::strcmp(argv[i], "-f")) {
        fitStrategy = argv[i + 1];
        ++i;
      } else if (!std::strcmp(argv[i], "-i")) {
        inputFileName = argv[i + 1];
        ++i;
      } else if (!std::strcmp(argv[i], "-q")) {
        configCmdFix.push_back(argv[i + 1]);
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
      }
    } else
      std::cerr << "ERROR: unknown command line option given! - '" << argv[i]
                << " " << argv[i + 1] << "'" << std::endl;
  }

  if (outputFileName.empty())
    std::cerr << "ERROR: output file not specified." << std::endl;
  if (cardFile.empty())
    std::cerr << "ERROR: card file not specified." << std::endl;

  // Parse fit routine
  std::string token;
  std::istringstream stream(fitStrategy);

  LOG(FIT) << "Fit Routine = " << fitStrategy << std::endl;
  while (std::getline(stream, token, ',')) {
    fit_routines.push_back(token);
  }

  readCard();
  setupConfig();

  if (!maxevents_flag.empty())
    FitPar::Config().SetParI("MAXEVENTS", atoi(maxevents_flag.c_str()));
  if (verbosity_flag != 0)
    FitPar::Config().SetParI(
        "VERBOSITY", FitPar::Config().GetParI("VERBOSITY") + verbosity_flag);
  if (error_flag != 0)
    FitPar::Config().SetParI("ERROR",
                             FitPar::Config().GetParI("ERROR") + error_flag);

  return;
};

//*************************************
void comparisonRoutines::initialSetup() {
  //*************************************

  // output file open
  outputFile = new TFile(outputFileName.c_str(), "RECREATE");
  FitPar::Config().Write();

  // setup RW and FCN
  setupRWEngine();
  setupFCN();

  return;
}

//*************************************
void comparisonRoutines::readCard() {
//*************************************
  cout << "Parsing input card..." << endl;
  
  std::string line;
  std::ifstream card(this->cardFile.c_str(), ifstream::in);
  int linecount = 0;
  
  while (std::getline(card, line, '\n')) {
    std::istringstream stream(line);
    linecount += 1;

    // Skip comment lines
    if (line.empty()) continue;
    if (line.c_str()[0] == '#') continue;

    // Save card to params
    FitPar::Config().cardLines.push_back(line);

    // Read in RW Parameters
    int readstatus = readParameters(line);
    if (readstatus != 0){
      ERR(FTL) << "Bad parameter read! " << endl;
      ERR(FTL) << "Line " << linecount << " : '" << line << "'" << endl;
      ERR(FTL) << "Correct Formats: " << endl;
      cout << "neut_parameter     PARAM_NAME   PARAM_VALUE" << endl;
      cout << "niwg_parameter     PARAM_NAME   PARAM_VALUE" << endl;
      cout << "genie_parameter    PARAM_NAME   PARAM_VALUE" << endl;
      cout << "nuwro_parameter    PARAM_NAME   PARAM_VALUE" << endl;
      cout << "t2k_parameter      PARAM_NAME   PARAM_VALUE" << endl;
      cout << "custom_parameter   PARAM_NAME   PARAM_VALUE" << endl;
      cout << "modenorm_parameter PARAM_NAME   PARAM_VALUE" << endl;
      throw;
    }

    // Read in FAKE MC Parameters
    readFakeDataPars(line);

    // Read in Sample Config
    readstatus += readSamples(line);
    if (readstatus != 0){
      ERR(FTL) << "Bad sample read! " << endl;
      ERR(FTL) << "Line " << linecount << " : '" << line << "'" << endl;
      ERR(FTL) << "Correct Format [OPTIONAL]: " << endl;
      cout << "'sample   SAMPLE_ID  TYPE:INPUT_FILE  [STATE   NORM]'" << endl;
      ERR(FTL) << "SAMPLE_ID : Sample Name. Look in src/SampleList.cxx" << endl;
      ERR(FTL) << "TYPE : Specify file type "
	       << "(NEUT,GENIE,GST,NUWRO,GIBUU,NUANCE) " << endl;
      ERR(FTL) << "INPUT_FILE : full path to input file " << endl;
      ERR(FTL) << "STATE : Sample options, "
	       << "see source code, or put DEFAULT" << endl;
      ERR(FTL) << "NORM : Scaling term to normalise MC to 1/chosen value"
	       << endl;
      ERR(FTL) << "e.g. MiniBooNE CCQE Data comparison with "
	       << "NEUT xsec scaled to 1.25 nominal value:" << endl;
      cout     << "sample  MiniBooNE_CCQE_XSec_1DQ2_nu "
	       << "NEUT:/path/to/neutvect.root DEFAULT 0.8" << endl;
      throw;
    }
  }
  card.close();
  return;
};

//*****************************************
int comparisonRoutines::readParameters(std::string parstring) {
//******************************************

  // Parse line
  // *_parameter    parname    nom  [low  high step state]
  std::vector<std::string> parvect = PlotUtils::FillVectorSFromString(parstring," ");

  // Check line is long enough
  if (parvect.empty()) return 0;

  // Check for parameter
  if (parvect[0].find("parameter") == std::string::npos) return 0;

  // Check for missing entries
  if (parvect.size() < 2){
    ERR(FTL) << parvect[1] << " specified with no parameter id!" << endl;
    return -1;
  }

  if (parvect.size() < 3){
    ERR(WRN) << parvect[0] << " " << parvect[1]
	     << " specified with out a value!" << endl;
    ERR(WRN) << "Assuming a value of 0.0 sigma!" << endl;
    sleep(1);
  }

  // Parse the string also into doubles
  std::vector<double> pardoub = PlotUtils::FillVectorDFromString(parstring, " ");

  // Set parameter type
  std::string partype_str = parvect[0];
  int partype;
  if (!partype_str.compare("neut_parameter"))
    partype = kNEUT;
  else if (!partype_str.compare("niwg_parameter"))
    partype = kNIWG;
  else if (!partype_str.compare("genie_parameter"))
    partype = kGENIE;
  else if (!partype_str.compare("nuwro_parameter"))
    partype = kNUWRO;
  else if (!partype_str.compare("custom_parameter"))
    partype = kCUSTOM;
  else if (!partype_str.compare("t2k_parameter"))
    partype = kT2K;
  else if (!partype_str.compare("modenorm_parameter"))
    partype = kMODENORM;
  else {
    ERR(FTL) << "Unknown parameter type! : "
	     << partype_str << endl;
  }

  // Set Parameter Name and Default
  std::string parname = parvect[1];
  params.push_back(parname);

  // Setup type map
  params_type[parname] = partype;

  // Set Default parameters for dial
  currentVals[parname] = 0.0;

  // Set Parameter Nominal
  double curval = 0.0;
  if (parvect.size() > 2){

    curval = pardoub[2];
    currentVals[parname] = curval;

  }

  // Set limits if stated
  std::string curparstate = "FIX";
  if (parvect.size() > 3){
    curparstate = parvect[3];
  }

  // Run Dial Conversions

  // ABSOLUTE CONVERSION
  if (curparstate.find("ABS") != std::string::npos) {
    LOG(MIN) << "Converting abs dial " << parname << " : "
	     << currentVals[parname];
        currentVals[parname] =
	  FitBase::RWAbsToSigma(partype_str, parname, currentVals[parname]);

	// FRACTION CONVERSION
  } else if (curparstate.find("FRAC") != std::string::npos) {
    LOG(FIT) << "Converting frac dial " << parname << " : "
	     << currentVals[parname];
        currentVals[parname] =
	  FitBase::RWFracToSigma(partype_str, parname, currentVals[parname]);
  }

  std::cout << "Read parameter: "<<partype_str << "  "
	    << parname << " = " << curval << "("
	    << curparstate << ")" << endl;

  return 0;
}

//*******************************************
void comparisonRoutines::readFakeDataPars(std::string parstring) {
  //******************************************

  std::string token, parname;
  std::istringstream stream(parstring);
  int val = 0;
  double entry;

  if (parstring.c_str()[0] == '#') return;

  while (std::getline(stream, token, ' ')) {
    stream >> std::ws;  // strip whitespace
    std::istringstream stoken(token);
    if (val == 2) stoken >> entry;

    if (val == 0) {
      if (token.compare("fake_parameter") != 0) return;
    } else if (val == 1) {
      fakeParams.push_back(token);
      parname = token;
    } else if (val == 2) {
      std::cout << "Set fake parameter " << parname << "  = " << entry
                << std::endl;
      fakeVals[parname] = entry;

    } else {
      break;
    }

    val++;
  }

  val = 0;
  while (std::getline(stream, token, ' ')) {
    stream >> std::ws;  // strip whitespace
    std::istringstream stoken(token);
    if (val == 2) stoken >> entry;

    if (val == 0 && token.compare("fake_norm") != 0) {
      return;
    }
    if (val == 1) {
      fakeSamples.push_back(token);
      parname = token;
    } else if (val == 2)
      fakeNorms[parname] = entry;

    val++;
  }
  return;
}

//******************************************
int comparisonRoutines::readSamples(std::string sampleString) {
//******************************************

// Parse String
  // Input is: sample  sample_id  FILE  [STATE]  [NORM]
  std::vector<std::string> samplevect = PlotUtils::FillVectorSFromString(sampleString," ");

  // Skip line if not sample
  if (samplevect[0].compare("sample")) return 0;

  // Check sample line is long enough
  if (samplevect.size() < 2){
    ERR(FTL) << "'sample' with no sample ID specified!" << endl;
    return -1;
  }

  if (samplevect.size() < 3){
    ERR(FTL) << "No input specified for sample '"
	     << samplevect[1]
	     << "'" << endl;
    return -1;
  }

  // Get Sample Name
  samples.push_back(samplevect[1]);
  std::string samplename = samplevect[1] + "_norm";

  sampleDials.push_back(samplename);

  // Set Default Norm to 1.0
  sampleNorms[samplename]  = 1.0;

  // Get File Name
  sampleFiles[samplename] = samplevect[2];

  // Get Sample State (if blank = DEFAULT)
  std::string state = "DEFAULT";
  if (samplevect.size() > 3) state = samplevect[3];
  sampleTypes[samplename] = state;

  // Get Sample Normalisation (if blank = 1.0)
  double norm = 1.0;
  if (samplevect.size() > 4){
    std::istringstream stoken(samplevect[4]);
    stoken >> norm;
  }

  sampleNorms[samplename]  = norm;

  std::cout << "Read sample: " << samplename << " >> " << sampleFiles[samplename] << std::endl;
  return 0;
}

/*
  Setup Functions
*/
//*************************************
void comparisonRoutines::setupConfig() {
  //*************************************

  std::string par_dir = std::string(std::getenv("EXT_FIT")) + "/parameters/";
  FitPar::Config().ReadParamFile(par_dir + "config.list.dat");
  FitPar::Config().ReadParamFile(cardFile);

  for (unsigned int iter = 0; iter < configCmdFix.size(); iter++)
    FitPar::Config().ForceParam(configCmdFix[iter]);

  LOG_VERB(FitPar::Config().GetParS("VERBOSITY"));
  ERR_VERB(FitPar::Config().GetParS("VERBOSITY"));

  return;
};

//*************************************
void comparisonRoutines::setupRWEngine() {
  //*************************************

  for (UInt_t i = 0; i < params.size(); i++) {
    std::string name = params[i];
    std::cout << "Adding parameter " << name << std::endl;
    FitBase::GetRW()->IncludeDial(name, params_type.at(name));
  }

  for (UInt_t i = 0; i < sampleDials.size(); i++) {
    std::string name = sampleDials[i];
    FitBase::GetRW()->IncludeDial(name, kNORM);
  }
  FitBase::GetRW()->Reconfigure();

  updateRWEngine(currentVals, sampleNorms);

  return;
}

//*************************************
void comparisonRoutines::setupFCN() {
  //*************************************

  LOG(FIT) << "Making the jointFCN" << std::endl;
  if (thisFCN) delete thisFCN;
  thisFCN = new jointFCN(cardFile, outputFile);
  thisFCN->SetOutName(outputFileName);
  setFakeData();

  return;
}

//*************************************
void comparisonRoutines::setFakeData() {
  //*************************************

  if (fakeDataFile.empty()) return;

  if (fakeDataFile.compare("MC") == 0) {
    LOG(FIT) << "Setting fake data from MC starting prediction." << std::endl;
    updateRWEngine(fakeVals, fakeNorms);

    FitBase::GetRW()->Reconfigure();
    this->ReconfigureAllEvents();
    thisFCN->SetFakeData("MC");

    updateRWEngine(currentVals, currentNorms);

    LOG(FIT) << "Set all data to fake MC predictions." << std::endl;
  } else {
    thisFCN->SetFakeData(fakeDataFile);
  }

  return;
}

/*
  Fitting Functions
*/
//*************************************
void comparisonRoutines::updateRWEngine(
    std::map<std::string, double>& updateVals,
    std::map<std::string, double>& updateNorms) {
  //*************************************

  for (UInt_t i = 0; i < params.size(); i++) {
    std::string name = params[i];

    if (updateVals.find(name) == updateVals.end()) continue;
    FitBase::GetRW()->SetDialValue(name, updateVals.at(name));
  }

  for (UInt_t i = 0; i < sampleDials.size(); i++) {
    std::string name = sampleDials[i];

    if (updateNorms.find(name) == updateNorms.end()) {
      FitBase::GetRW()->SetDialValue(name, 1.0);
    } else {
      FitBase::GetRW()->SetDialValue(name, updateNorms.at(name));
    }
  }

  FitBase::GetRW()->Reconfigure();
  return;
}

//**************
void comparisonRoutines::SelfFit() {
  //*************

  for (UInt_t i = 0; i < fit_routines.size(); i++) {
    std::string routine = fit_routines.at(i);

    // Stop fit if all pars are constant
    int NFREE = 0;
    for (UInt_t i = 0; i < params.size(); i++) {
      if (!fixVals[params[i]]) NFREE++;
    }
    for (UInt_t i = 0; i < sampleDials.size(); i++) {
      if (!fixNorms[sampleDials[i]]) NFREE++;
    }
    if (NFREE == 0) break;

    LOG(FIT) << "Running Routine: " << routine << std::endl;
    if (routine.find("Compare") != std::string::npos) {
      this->ReconfigureAllEvents();
    } else {
      ERR(FTL) << "Unknown Routine Attempt!" << endl;
      throw;
    }
  }

  return;
}

//*************************************
void comparisonRoutines::ReconfigureAllEvents() {
  //*************************************

  // Main Event Loop from event Manager
  bool using_evtmanager = FitPar::Config().GetParB("EventManager");

  if (using_evtmanager) {
    std::list<MeasurementBase*> fChain = thisFCN->GetSampleList();
    std::list<MeasurementBase*>::const_iterator iterSam = fChain.begin();

    std::map<int, InputHandler*> fInputs = FitBase::EvtManager().GetInputs();
    std::map<int, InputHandler*>::const_iterator iterInp = fInputs.begin();

    int timestart = time(NULL);

    for (; iterInp != fInputs.end(); iterInp++) {
      int input_id = (iterInp->first);
      InputHandler* cur_input = (iterInp->second);
      FitEvent* cust_event = cur_input->GetEventPointer();
      int nevents = cur_input->GetNEvents();
      int countwidth = (nevents / 200);
      size_t NSignal = 0;

      // MAIN EVENT LOOP
      for (int i = 0; i < nevents; i++) {
        cust_event = FitBase::EvtManager().GetEvent(input_id, i);
        double Weight = cust_event->Weight;
        if (fabs(cust_event->Mode) > 60 || cust_event->Mode == 0 ||
            Weight > 200.0 || Weight < 0.0)
          continue;

        iterSam = fChain.begin();
        for (; iterSam != fChain.end(); iterSam++) {
          MeasurementBase* exp = (*iterSam);
          if (exp->GetInputID() != input_id) continue;

          LOG(EVT) << " FILLING EVENT MANAGER LOOP" << std::endl;
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

    iterSam = fChain.begin();
    for (; iterSam != fChain.end(); iterSam++) {
      MeasurementBase* exp = (*iterSam);
      std::cout << "Finalising sample " << exp << std::endl;
      exp->ConvertEventRates();
    }

    std::cout << " Time Taken = " << time(NULL) - timestart << std::endl;
    std::cout << "Finished reconfiguring all events" << std::endl;

  } else {
    thisFCN->ReconfigureAllEvents();
  }

  return;
}

//*************************************
void comparisonRoutines::saveCurrentState(std::string subdir) {
  //*************************************

  LOG(FIT) << "Saving current FCN predictions" << std::endl;

  outputFile->cd();
  FitBase::GetRW()->Reconfigure();
  this->ReconfigureAllEvents();

  if (!subdir.empty()) {
    TDirectory* nominalDIR = (TDirectory*)outputFile->mkdir("nominal");
    nominalDIR->cd();
  }

  thisFCN->Write();

  outputFile->cd();

  return;
}

//*************************************
void comparisonRoutines::saveNominal() {
  //*************************************

  LOG(FIT) << "Saving Neut Nominal Predictions (be cautious with this)"
           << std::endl;

  // rw->ResetAll();
  FitBase::GetRW()->Reconfigure();
  saveCurrentState("nominal");
};

