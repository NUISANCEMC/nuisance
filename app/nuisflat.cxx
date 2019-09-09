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
#include "ComparisonRoutines.h"
#include "GenericFlux_Tester.h"
#include "GenericFlux_Vectors.h"
#include "InputUtils.h"
#include "MeasurementBase.h"
#include "Smearceptance_Tester.h"

// Global Arguments
std::string gOptInputFile = "";
std::string gOptFormat = "";
std::string gOptOutputFile = "";
std::string gOptType = "DEFAULT";
std::string gOptNumberEvents = "NULL";
std::string gOptCardInput = "";
std::string gOptOptions = "";

// Input Dial Vals
std::vector<std::string> fParams;              ///< Vector of dial names.
std::map<std::string, std::string> fStateVals; ///< Map of dial states
std::map<std::string, double> fCurVals;        ///< Map of dial values
std::map<std::string, int> fTypeVals;          ///< Map of dial type enums.
// Fake Dial Vals
std::map<std::string, double> fFakeVals; ///< Map of fake data settings.

void SetupComparisonsFromXML();
void SetupRWEngine();

//*******************************
void PrintSyntax() {
  //*******************************

  std::cout << "nuisflat -i input [-f format]  [-o outfile] [-n nevents] [-t "
               "options] [-q con=val] \n";
  std::cout
      << "\n Arguments : "
      << "\n\t -i input   : Path to input vector of events to flatten"
      << "\n\t"
      << "\n\t              This should be given in the same format a normal "
         "input file"
      << "\n\t              is given to NUISANCE. {e.g. NUWRO:eventsout.root}."
      << "\n\t"
      << "\n\t -f format  : FlatTree format to output. If none given "
         "GenericVectors used."
      << "\n\t\t GenericFlux   : Flat event summary format."
      << "\n\t\t GenericVectors   : Standard event summary format with "
         "particle vectors."
      << "\n\t "
      << "\n\t[-c crd.xml]: Input card file to override configs or set dial "
         "values."
      << "\n\t "
      << "\n\t[-o outfile]: Optional output file path. "
      << "\n\t "
      << "\n\t              If none given, input.format.root is chosen."
      << "\n\t"
      << "\n\t[-n nevents]: Optional choice of Nevents to run over. Default is "
         "all."
      << "\n\t"
      << "\n\t[-t options]: Pass OPTION to the FlatTree sample. "
      << "\n\t              Similar to type field in comparison xml configs."
      << "\n\t"
      << "\n\t[-q con=val]: Configuration overrides." << std::endl;

  exit(-1);
};

//____________________________________________________________________________
void GetCommandLineArgs(int argc, char **argv) {
  // Check for -h flag.
  for (int i = 0; i < argc; i++) {
    if ((!std::string(argv[i]).compare("-h")) ||
        (!std::string(argv[i]).compare("-?")) ||
        (!std::string(argv[i]).compare("--help")))
      PrintSyntax();
  }

  // Format is nuwro -r run_number -n n events
  std::vector<std::string> args = GeneralUtils::LoadCharToVectStr(argc, argv);

  // Parse input file
  ParserUtils::ParseArgument(args, "-i", gOptInputFile, false);
  if (gOptInputFile == "") {
    QTHROW("Need to provide a valid input file to nuisflat using -i flag!");
  } else {
    QLOG(FIT, "Reading Input File = " << gOptInputFile);
  }

  // Get Output Format
  ParserUtils::ParseArgument(args, "-f", gOptFormat, false);
  if (gOptFormat == "") {
    gOptFormat = "GenericVectors";
    QLOG(FIT, "Saving flattree in default format = " << gOptFormat);
  } else {
    QLOG(FIT, "Saving flattree in format = " << gOptFormat);
  }

  // Get Output File
  ParserUtils::ParseArgument(args, "-o", gOptOutputFile, false);
  if (gOptOutputFile == "") {
    gOptOutputFile = gOptInputFile + "." + gOptFormat + ".root";
    QLOG(FIT, "No output file given so saving nuisflat output to:"
                  << gOptOutputFile);
  } else {
    QLOG(FIT, "Saving nuisflat output to " << gOptOutputFile);
  }

  // Get N Events and Configs
  nuisconfig configuration = Config::Get();

  ParserUtils::ParseArgument(args, "-n", gOptNumberEvents, false);
  if (gOptNumberEvents.compare("NULL")) {
    configuration.OverrideConfig("MAXEVENTS=" + gOptNumberEvents);
  }

  std::vector<std::string> configargs;
  ParserUtils::ParseArgument(args, "-q", configargs);
  for (size_t i = 0; i < configargs.size(); i++) {
    configuration.OverrideConfig(configargs[i]);
  }

  ParserUtils::ParseArgument(args, "-c", gOptCardInput, false);
  if (gOptCardInput != "") {
    QLOG(FIT, "Reading cardfile: " << gOptCardInput);
    configuration.LoadSettings(gOptCardInput, "");
  }

  ParserUtils::ParseArgument(args, "-t", gOptOptions, false);
  if (gOptOptions != "") {
    QLOG(FIT, "Read options: \"" << gOptOptions << "\'");
  }
  return;
}

//*******************************
int main(int argc, char *argv[]) {
  //*******************************

  // Parse
  GetCommandLineArgs(argc, argv);

  // Make output file
  TFile *f = new TFile(gOptOutputFile.c_str(), "RECREATE");
  if (f->IsZombie()) {
    QTHROW("Cannot create output file!");
  }
  f->cd();
  FitPar::Config().out = f;

  // Create a new measurementbase class depending on the Format
  MeasurementBase *flattreecreator = NULL;

  SetupComparisonsFromXML();
  SetupRWEngine();
  // SetupFCN();

  // Make a new sample key for the format of interest.
  nuiskey samplekey = Config::CreateKey("sample");
  if (!gOptFormat.compare("GenericFlux")) {
    samplekey.Set("name", "FlatTree");
    samplekey.Set("input", gOptInputFile);
    samplekey.Set("type", gOptType);
    flattreecreator = new GenericFlux_Tester("FlatTree", gOptInputFile,
                                             FitBase::GetRW(), gOptType, "");

  } else if (!gOptFormat.compare("GenericVectors")) {
    samplekey.Set("name", "FlatTree");
    samplekey.Set("input", gOptInputFile);
    samplekey.Set("type", gOptType);
    flattreecreator = new GenericFlux_Vectors("FlatTree", gOptInputFile,
                                              FitBase::GetRW(), gOptType, "");

  } else {
    QERROR(FTL, "Unknown FlatTree format!");
  }

  // Make the FlatTree reconfigure
  flattreecreator->Reconfigure();
  f->cd();
  flattreecreator->Write();
  f->Close();

  // Show Final Status
  QLOG(FIT, "-------------------------------------");
  QLOG(FIT, "Flattree Generation Complete.");
  QLOG(FIT, "-------------------------------------");

  return 0;
}
//*************************************
void SetupComparisonsFromXML() {
  //*************************************

  QLOG(FIT, "Setting up nuiscomp");

  // Setup Parameters ------------------------------------------
  std::vector<nuiskey> parkeys = Config::QueryKeys("parameter");
  if (!parkeys.empty()) {
    QLOG(FIT, "Number of parameters :  " << parkeys.size());
  }

  for (size_t i = 0; i < parkeys.size(); i++) {
    nuiskey key = parkeys.at(i);

    // Check for type,name,nom
    if (!key.Has("type")) {
      QERROR(FTL, "No type given for parameter " << i);
      QERROR(FTL, "type='PARAMETER_TYPE'");
      throw;
    } else if (!key.Has("name")) {
      QERROR(FTL, "No name given for parameter " << i);
      QERROR(FTL, "name='SAMPLE_NAME'");
      throw;
    } else if (!key.Has("nominal")) {
      QERROR(FTL, "No nominal given for parameter " << i);
      QERROR(FTL, "nominal='NOMINAL_VALUE'");
      throw;
    }

    // Get Inputs
    std::string partype = key.GetS("type");
    std::string parname = key.GetS("name");
    double parnom = key.GetD("nominal");
    double parlow = parnom - 1;
    double parhigh = parnom + 1;
    double parstep = 1;

    // override if state not given
    if (!key.Has("state")) {
      key.SetS("state", "FIX");
    }

    std::string parstate = key.GetS("state");

    // Check for incomplete limtis
    int limdef =
        ((int)key.Has("low") + (int)key.Has("high") + (int)key.Has("step"));

    if (limdef > 0 and limdef < 3) {
      QERROR(FTL, "Incomplete limit set given for parameter : " << parname);
      QERROR(
          FTL,
          "Requires: low='LOWER_LIMIT' high='UPPER_LIMIT' step='STEP_SIZE' ");
      throw;
    }

    // Extra limits
    if (key.Has("low")) {

      parlow = key.GetD("low");
      parhigh = key.GetD("high");
      parstep = key.GetD("step");

      QLOG(FIT, "Read " << partype << " : " << parname << " = " << parnom
                        << " : " << parlow << " < p < " << parhigh << " : "
                        << parstate);
    } else {
      QLOG(FIT, "Read " << partype << " : " << parname << " = " << parnom
                        << " : " << parstate);
    }

    // Convert if required
    if (parstate.find("ABS") != std::string::npos) {
      parnom = FitBase::RWAbsToSigma(partype, parname, parnom);
      parlow = FitBase::RWAbsToSigma(partype, parname, parlow);
      parhigh = FitBase::RWAbsToSigma(partype, parname, parhigh);
      parstep = FitBase::RWAbsToSigma(partype, parname, parstep);
    } else if (parstate.find("FRAC") != std::string::npos) {
      parnom = FitBase::RWFracToSigma(partype, parname, parnom);
      parlow = FitBase::RWFracToSigma(partype, parname, parlow);
      parhigh = FitBase::RWFracToSigma(partype, parname, parhigh);
      parstep = FitBase::RWFracToSigma(partype, parname, parstep);
    }

    // Push into vectors
    fParams.push_back(parname);

    fTypeVals[parname] = FitBase::ConvDialType(partype);
    ;
    fCurVals[parname] = parnom;
    fStateVals[parname] = parstate;
  }

  // Setup Samples ----------------------------------------------
  std::vector<nuiskey> samplekeys = Config::QueryKeys("sample");
  if (!samplekeys.empty()) {
    QLOG(FIT,"Number of samples : " << samplekeys.size());
  }

  for (size_t i = 0; i < samplekeys.size(); i++) {
    nuiskey key = samplekeys.at(i);

    // Get Sample Options
    std::string samplename = key.GetS("name");
    std::string samplefile = key.GetS("input");

    std::string sampletype = key.Has("type") ? key.GetS("type") : "DEFAULT";

    double samplenorm = key.Has("norm") ? key.GetD("norm") : 1.0;

    // Print out
    QLOG(FIT, "Read Sample " << i << ". : " << samplename << " ("
             << sampletype << ") [Norm=" << samplenorm << "]" << std::endl
             << "                                -> input='" << samplefile
             << "'");

    // If FREE add to parameters otherwise continue
    if (sampletype.find("FREE") == std::string::npos) {
      continue;
    }

    // Form norm dial from samplename + sampletype + "_norm";
    std::string normname = samplename + "_norm";

    // Check normname not already present
    if (fTypeVals.find("normname") != fTypeVals.end()) {
      continue;
    }

    // Add new norm dial to list if its passed above checks
    fParams.push_back(normname);

    fTypeVals[normname] = kNORM;
    fStateVals[normname] = sampletype;
    fCurVals[normname] = samplenorm;
  }

  // Setup Fake Parameters -----------------------------
  std::vector<nuiskey> fakekeys = Config::QueryKeys("fakeparameter");
  if (!fakekeys.empty()) {
    QLOG(FIT,"Number of fake parameters : " << fakekeys.size());
  }

  for (size_t i = 0; i < fakekeys.size(); i++) {
    nuiskey key = fakekeys.at(i);

    // Check for type,name,nom
    if (!key.Has("name")) {
      QERROR(FTL, "No name given for fakeparameter " << i);
      throw;
    } else if (!key.Has("nominal")) {
      QERROR(FTL, "No nominal given for fakeparameter " << i);
      throw;
    }

    // Get Inputs
    std::string parname = key.GetS("name");
    double parnom = key.GetD("nominal");

    // Push into vectors
    fFakeVals[parname] = parnom;
  }
}

//*************************************
void SetupRWEngine() {
  //*************************************

  QLOG(FIT,"Setting up FitWeight Engine");
  for (UInt_t i = 0; i < fParams.size(); i++) {
    std::string name = fParams[i];
    FitBase::GetRW()->IncludeDial(name, fTypeVals.at(name));
  }

  for (UInt_t i = 0; i < fParams.size(); i++) {
    std::string name = fParams[i];

    if (fCurVals.find(name) == fCurVals.end())
      continue;
    FitBase::GetRW()->SetDialValue(name, fCurVals.at(name));
  }

  FitBase::GetRW()->Reconfigure();
  return;
}
