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
#include "InputUtils.h"

#include "ComparisonRoutines.h"

//*******************************
void printInputCommands() {
  //*******************************

  std::cout << "nuisflat [-c cardfile] -o outfile [-q configname=configval] \n";
  std::cout << "\n Arguments : \n";
  std::cout
      << "   -i inputvector: Path to input vector of events to flatten \n";
  std::cout
      << "   -c cardfile: Path to NUISANCE card file defining fit samples \n";
  std::cout << "    -o outFile:    Path to root file that will be created to "
               "save output file.\n";
  std::cout << "    -q config_name=config_val : Allows any config parameter to "
               "be overridden from the command line.\n";
  std::cout << "                                This will take priority over "
               "those given in the default, or cardFile. \n";
  std::cout << "                                example: -q verbosity=6 -q "
               "maxevents=10000 \n"
            << std::endl;

  exit(-1);
};

//*******************************
int main(int argc, char* argv[]) {
  //*******************************

<<<<<<< HEAD
  // // Program status;
  // int status = 0;

  // std::string inpFileName = "";
  // std::string OutputFile = "";
  // std::string maxevents_flag = "";

  // // If No Arguments print commands
  // if (argc == 1) printInputCommands();

  // for (int i = 1; i < argc; ++i) {
  //   if (!std::strcmp(argv[i], "-h")) {
  //     printInputCommands();
  //   } else if (!std::strcmp(argv[i], "-i")) {
  //     inpFileName = argv[++i];
  //   } else if (!std::strcmp(argv[i], "-n")) {
  //     maxevents_flag = argv[++i];
  //   } else if (!std::strcmp(argv[i], "-o")) {
  //     OutputFile = argv[++i];
  //   }
  // }

  // // Read input arguments such as card file, parameter arguments, and fit
  // // routines
  // LOG(FIT) << "Starting nuisflat.exe" << std::endl;

  // if (!inpFileName.length()) {  // Run with card file
  //   // Make minimizer class and run fit
  //   ComparisonRoutines* flat = new ComparisonRoutines(argc, argv);

  //   // Run the fit rotines
  //   flat->Run();
  //   flat->SaveCurrentState();
  // } else {  // Run standalone

  //   if (!OutputFile.length()) {
  //     OutputFile = inpFileName + ".root";
  //     ERR(WRN) << "Didn't recieve outputfile name. Using \"" << OutputFile
  //              << "\" instead." << std::endl;
  //   }

  //   std::string fullInputName =
  //       InputUtils::PrependGuessedInputTypeToName(inpFileName);
  //   std::string par_dir = GeneralUtils::GetTopLevelDir() + "/parameters/";
  //   FitPar::Config().ReadParamFile(par_dir + "config.list.dat");

  //   if (!maxevents_flag.empty()) {
  //     FitPar::Config().SetParI("input.maxevents", atoi(maxevents_flag.c_str()));
  //   }

  //   LOG_VERB(FitPar::Config().GetParI("VERBOSITY"));
  //   ERR_VERB(FitPar::Config().GetParI("ERROR"));
  //   // Outputs
  //   // ---------------------------
  //   // Save Configs to output file
  //   FitPar::Config().out = new TFile(OutputFile.c_str(), "RECREATE");
  //   FitPar::Config().out->cd();
  //   FitPar::Config().Write();

  //   std::list<MeasurementBase*> fChain;
  //   bool LoadedSample =
  //       SampleUtils::LoadSample(&fChain, "GenericFlux_", fullInputName,
  //                               "DEFAULT", "", FitBase::GetRW());

  //   if (!LoadedSample) {
  //     ERR(FTL) << "Could not Flattener sample. Please report this as a bug."
  //              << std::endl;
  //     throw;
  //   }

  //   fChain.front()->Reconfigure();

  //   FitPar::Config().out->Write();
  //   FitPar::Config().out->Close();
  // }

    /*
  // Program status;
  int status = 0;

  std::string inpFileName = "";
  std::string OutputFile = "";
  std::string maxevents_flag = "";

  // If No Arguments print commands
  if (argc == 1) printInputCommands();

  for (int i = 1; i < argc; ++i) {
    if (!std::strcmp(argv[i], "-h")) {
      printInputCommands();
    } else if (!std::strcmp(argv[i], "-i")) {
      inpFileName = argv[++i];
    } else if (!std::strcmp(argv[i], "-n")) {
      maxevents_flag = argv[++i];
    } else if (!std::strcmp(argv[i], "-o")) {
      OutputFile = argv[++i];
    }
  }

  // Read input arguments such as card file, parameter arguments, and fit
  // routines
  LOG(FIT) << "Starting nuisflat.exe" << std::endl;

  if (!inpFileName.length()) {  // Run with card file
    // Make minimizer class and run fit
    ComparisonRoutines* flat = new ComparisonRoutines(argc, argv);

    // Run the fit rotines
    flat->Run();
    flat->SaveCurrentState();
  } else {  // Run standalone

    if (!OutputFile.length()) {
      OutputFile = inpFileName + ".root";
      ERR(WRN) << "Didn't recieve outputfile name. Using \"" << OutputFile
               << "\" instead." << std::endl;
    }

    std::string fullInputName =
        InputUtils::PrependGuessedInputTypeToName(inpFileName);
    std::string par_dir = GeneralUtils::GetTopLevelDir() + "/parameters/";
    FitPar::Config().ReadParamFile(par_dir + "config.list.dat");

    if (!maxevents_flag.empty()) {
      FitPar::Config().SetParI("input.maxevents", atoi(maxevents_flag.c_str()));
    }

    LOG_VERB(FitPar::Config().GetParI("VERBOSITY"));
    ERR_VERB(FitPar::Config().GetParI("ERROR"));
    // Outputs
    // ---------------------------
    // Save Configs to output file
    FitPar::Config().out = new TFile(OutputFile.c_str(), "RECREATE");
    FitPar::Config().out->cd();
    FitPar::Config().Write();

    std::list<MeasurementBase*> fChain;
    bool LoadedSample =
        SampleUtils::LoadSample(&fChain, "GenericFlux_", fullInputName,
                                "DEFAULT", "", FitBase::GetRW());

    if (!LoadedSample) {
      ERR(FTL) << "Could not Flattener sample. Please report this as a bug."
               << std::endl;
      throw;
    }

    fChain.front()->Reconfigure();
    fChain.front()->Write();

    //    FitPar::Config().out->Write();
    FitPar::Config().out->Close();
  }

  */
  // Show Final Status
  LOG(FIT) << "-------------------------------------" << std::endl;
  LOG(FIT) << "Flattree Generation Complete." << std::endl;
  LOG(FIT) << "-------------------------------------" << std::endl;

  return 0;
}
