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

  std::string line;
  std::ifstream card(this->cardFile.c_str(), ifstream::in);

  while (std::getline(card, line, '\n')) {
    std::istringstream stream(line);

    FitPar::Config().cardLines.push_back(line);

    readParameters(line);
    readFakeDataPars(line);
    readSamples(line);
  }
  card.close();
  return;
};

//*****************************************
void comparisonRoutines::readParameters(std::string parstring) {
  //******************************************

  std::string token, parname;
  std::istringstream stream(parstring);
  int val = 0;
  double entry;
  int partype;
  std::string curparstate = "";
  std::string partype_str = "";

  if (parstring.c_str()[0] == '#') return;

  while (std::getline(stream, token, ' ')) {
    stream >> std::ws;

    std::istringstream stoken(token);
    if (val > 1 and val < 6) stoken >> entry;

    // Allow (parameter name val FIX)
    if (val > 2 and val < 6 and token.find("FIX") != std::string::npos) {
      startFixVals[parname] = true;
      fixVals[parname] = true;
      break;
    }

    if (val == 0 && token.compare("niwg_parameter") &&
        token.compare("neut_parameter") && token.compare("genie_parameter") &&
        token.compare("nuwro_parameter") && token.compare("custom_parameter") &&
        token.compare("t2k_parameter") && token.compare("modenorm_parameter")) {
      return;

    } else if (val == 0) {
      partype_str = token;

      if (!token.compare("neut_parameter"))
        partype = kNEUT;
      else if (!token.compare("niwg_parameter"))
        partype = kNIWG;
      else if (!token.compare("genie_parameter"))
        partype = kGENIE;
      else if (!token.compare("nuwro_parameter"))
        partype = kNUWRO;
      else if (!token.compare("custom_parameter"))
        partype = kCUSTOM;
      else if (!token.compare("t2k_parameter"))
        partype = kT2K;
      else if (!token.compare("modenorm_parameter"))
        partype = kMODENORM;

    } else if (val == 1) {
      params.push_back(token);
      parname = token;

      // Set Type
      params_type[parname] = partype;

      // Defaults
      startVals[parname] = 0.0;
      currentVals[parname] = 0.0;
      errorVals[parname] = 0.0;

      minVals[parname] = -1.0;
      maxVals[parname] = 1.0;
      stepVals[parname] = 0.5;

    } else if (val == 2) {  // Nominal
      startVals[parname] = entry;
      currentVals[parname] = entry;
      errorVals[parname] = 0.0;
    } else if (val == 3) {
      minVals[parname] = entry;  // min
    } else if (val == 4) {
      maxVals[parname] = entry;  // max
    } else if (val == 5) {
      stepVals[parname] = entry;  // step
    } else if (val == 6) {        // type
      startFixVals[parname] = (token.find("FIX") != std::string::npos);
      fixVals[parname] = (token.find("FIX") != std::string::npos);

      curparstate = token;

    } else
      break;

    val++;
  }

  // Run Dial Conversions

  // ABSOLUTE CONVERSION
  if (curparstate.find("ABS") != std::string::npos) {
    LOG(MIN) << "Converting abs dial " << parname << " : "
             << startVals[parname];
    startVals[parname] =
        FitBase::RWAbsToSigma(partype_str, parname, startVals[parname]);
    currentVals[parname] =
        FitBase::RWAbsToSigma(partype_str, parname, currentVals[parname]);
    minVals[parname] =
        FitBase::RWAbsToSigma(partype_str, parname, minVals[parname]);
    maxVals[parname] =
        FitBase::RWAbsToSigma(partype_str, parname, maxVals[parname]);
    stepVals[parname] =
        fabs((FitBase::RWAbsToSigma(partype_str, parname,
                                    startVals[parname] + stepVals[parname]) -
              FitBase::RWAbsToSigma(partype_str, parname, startVals[parname])));

    LOG(MIN) << " -> " << startVals[parname] << std::endl;

    // FRACTION CONVERSION
  } else if (curparstate.find("FRAC") != std::string::npos) {
    LOG(FIT) << "Converting frac dial " << parname << " : "
             << startVals[parname];
    startVals[parname] =
        FitBase::RWFracToSigma(partype_str, parname, startVals[parname]);
    currentVals[parname] =
        FitBase::RWFracToSigma(partype_str, parname, currentVals[parname]);
    minVals[parname] =
        FitBase::RWFracToSigma(partype_str, parname, minVals[parname]);
    maxVals[parname] =
        FitBase::RWFracToSigma(partype_str, parname, maxVals[parname]);
    stepVals[parname] =
        (FitBase::RWFracToSigma(partype_str, parname, stepVals[parname]));

    LOG(MIN) << " -> " << startVals[parname] << std::endl;
  }

  return;
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
void comparisonRoutines::readSamples(std::string sampleString) {
//******************************************

  std::string token, samplename;
  std::istringstream stream(sampleString);
  int val = 0;
  double entry;

  if (sampleString.c_str()[0] == '#') return;

  while (std::getline(stream, token, ' ')) {
    stream >> std::ws;  // strip whitespace
    std::istringstream stoken(token);
    stoken >> entry;

    if (val == 0) {
      if (token.compare("sample") != 0) {
        return;
      }
    } else if (val == 1) {
      samplename = token + "_norm";
      samples.push_back(token);
      sampleDials.push_back(samplename);

      sampleNorms[samplename] = 1.0;
      currentNorms[samplename] = 1.0;
      errorNorms[samplename] = 1.0;

    } else if (val == 2) {
      sampleTypes[samplename] = token;
      bool fixed = (token.find("FREE") == std::string::npos);

      fixNorms[samplename] = fixed;       // fixed;
      startFixNorms[samplename] = fixed;  // fixed;

      // defaults
      sampleNorms[samplename] = 1.0;
      currentNorms[samplename] = 1.0;
      errorNorms[samplename] = 0.0;

    } else if (val == 3) {
      sampleFiles[samplename] = token;
    } else if (val == 4) {
      if (entry > 0.3 and entry < 1.7) {
        sampleNorms[samplename] = entry;
        currentNorms[samplename] = entry;
        errorNorms[samplename] = entry;
      }
    }

    val++;
  }
  std::cout << "added sample " << samplename << " with norm "
            << sampleNorms[samplename] << " " << currentNorms[samplename]
            << std::endl;
  return;
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

  updateRWEngine(startVals, sampleNorms);

  return;
}

//*************************************
void comparisonRoutines::setupFCN() {
  //*************************************

  LOG(FIT) << "Making the minimizerFCN" << std::endl;
  if (thisFCN) delete thisFCN;
  thisFCN = new minimizerFCN(cardFile, outputFile);
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

