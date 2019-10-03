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
#include "InputUtils.h"

#include "Smearceptance_Tester.h"

// Global Arguments
std::string gOptInputFile = "";
std::string gOptOutputFile = "";
std::string gOptType = "DEFAULT";
std::string gOptNumberEvents = "NULL";
std::string gOptCardInput = "";
std::string gOptOptions = "";

//*******************************
void PrintSyntax() {
  //*******************************

  std::cout << "nuisflat -i input [-o outfile] [-n nevents] [-t "
               "options] [-q con=val] \n";
  std::cout
      << "\n Arguments : "
      << "\n\t -i input   : Path to input vector of events to flatten"
      << "\n\t"
      << "\n\t              This should be given in the same format a normal "
         "input file"
      << "\n\t              is given to NUISANCE. {e.g. NUWRO:eventsout.root}."
      << "\n\t"
      << "\n\t[-c crd.xml]: Input card file to override configs or define "
         "smearcepters."
      << "\n\t "
      << "\n\t[-o outfile]: Optional output file path. "
      << "\n\t "
      << "\n\t              If none given, input.smear.root is chosen."
      << "\n\t"
      << "\n\t[-n nevents]: Optional choice of Nevents to run over. Default is "
         "all."
      << "\n\t"
      << "\n\t[-t options]: Pass OPTION to the smearception sample. "
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
    NUIS_ABORT("Need to provide a valid input file to nuisflat using -i flag!");
  } else {
    NUIS_LOG(FIT, "Reading Input File = " << gOptInputFile);
    gOptInputFile = InputUtils::PrependGuessedInputTypeToName(gOptInputFile);
  }

  // Get Output File
  ParserUtils::ParseArgument(args, "-o", gOptOutputFile, false);
  if (gOptOutputFile == "") {
    gOptOutputFile = gOptInputFile + ".smear.root";
    NUIS_LOG(FIT, "No output file given so saving nuisflat output to:"
                  << gOptOutputFile);
  } else {
    NUIS_LOG(FIT, "Saving nuisflat output to " << gOptOutputFile);
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
    NUIS_LOG(FIT, "Reading cardfile: " << gOptCardInput);
    configuration.LoadSettings(gOptCardInput, "");
  }

  ParserUtils::ParseArgument(args, "-t", gOptOptions, false);
  if (gOptOptions != "") {
    NUIS_LOG(FIT, "Read options: \"" << gOptOptions << "\'");
  }
  return;
}

void SetupRW() {
  std::vector<nuiskey> parkeys = Config::QueryKeys("parameter");
  if (!parkeys.empty()) {
    NUIS_LOG(FIT, "Number of parameters :  " << parkeys.size());
  }

  std::vector<std::string> Params;
  std::map<std::string, int> TypeVals;
  std::map<std::string, double> CurrVals;
  for (size_t i = 0; i < parkeys.size(); i++) {
    nuiskey key = parkeys.at(i);

    // Check for type,name,nom
    if (!key.Has("type")) {
      NUIS_ERR(FTL, "No type given for parameter " << i);
      NUIS_ABORT("type='PARAMETER_TYPE'");
    } else if (!key.Has("name")) {
      NUIS_ERR(FTL, "No name given for parameter " << i);
      NUIS_ABORT("name='SAMPLE_NAME'");
    } else if (!key.Has("nominal")) {
      NUIS_ERR(FTL, "No nominal given for parameter " << i);
      NUIS_ABORT("nominal='NOMINAL_VALUE'");
    }

    // Get Inputs
    std::string partype = key.GetS("type");
    std::string parname = key.GetS("name");
    double parnom = key.GetD("nominal");

    // Push into vectors
    Params.push_back(parname);

    TypeVals[parname] = FitBase::ConvDialType(partype);
    CurrVals[parname] = parnom;
  }

  for (UInt_t i = 0; i < Params.size(); i++) {
    FitBase::GetRW()->IncludeDial(Params[i], TypeVals[Params[i]]);
    FitBase::GetRW()->SetDialValue(Params[i], CurrVals[Params[i]]);
  }

  FitBase::GetRW()->Reconfigure();
}

//*******************************
int main(int argc, char *argv[]) {
  //*******************************

  // Parse
  GetCommandLineArgs(argc, argv);

  // Make output file
  TFile *f = new TFile(gOptOutputFile.c_str(), "RECREATE");
  if (f->IsZombie()) {
    NUIS_ABORT("Cannot create output file!");
  }
  f->cd();
  Config::Get().out = f;

  // Create a new measurementbase class depending on the Format
  MeasurementBase *flattreecreator = NULL;

  // Make a new sample key for the format of interest.
  nuiskey samplekey = Config::CreateKey("sample");

  samplekey.Set("name", "FlatTree");
  samplekey.Set("smearceptor", gOptOptions);
  samplekey.Set("input", gOptInputFile);
  samplekey.Set("type", gOptType);

  if (gOptOptions == "") {
    NUIS_ABORT("Attempting to flatten with Smearceptor, but no Smearceptor given. "
           "Please supply a -t option.");
  }
  if (gOptCardInput == "") {
    NUIS_ABORT("Attempting to flatten with Smearceptor, but no card passed with "
           "Smearceptors configured. Please supply a -c option.");
  }

  SetupRW();

  flattreecreator = new Smearceptance_Tester(samplekey);

  // Make the FlatTree reconfigure
  flattreecreator->Reconfigure();
  f->cd();
  flattreecreator->Write();
  f->Close();

  // Show Final Status
  NUIS_LOG(FIT, "-------------------------------------");
  NUIS_LOG(FIT, "Flattree Generation Complete.");
  NUIS_LOG(FIT, "-------------------------------------");

  return 0;
}
