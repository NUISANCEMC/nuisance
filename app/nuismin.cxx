// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

// Author: Callum Wilkinson    01/2014

//         Patrick Stowell     09/2015
/**
  Usage: ./GSLminimizerWithReWeight.exe -c card file, where samples and
  parameters are defined -o output file, where the results of the fit are stored
   where:
*/

#include "MinimizerRoutines.h"

//*******************************
void printInputCommands() {
  //*******************************

  std::cout
      << "nuismin -c cardFile -o outFile [-f fitStategy] [-d "
         "fakeDataFile] [-i inputFile] [-q config_name=config_val] \n";
  std::cout << std::endl;
  std::cout << "Arguments:" << std::endl;
  std::cout << "     -c cardFile:   Path to card file that defines fit "
               "samples, free parameters, and config overrides \n";
  std::cout << "     -o outFile:    Path to root file that will be created to "
               "save output file.\n";
  std::cout << "                    To turn automatically overwrite outFile if "
               "one exists turn off use 'config overwrite_output 1'\n";
  std::cout << "                    To automatically use previous  outFile as "
               "an inputFile if it exists so that the fit can be continued\n";
  std::cout << "                    use the flag 'config use_previous_output "
               "1'. (A warning will be printed when doing this). \n";
  std::cout << "     -f fitStategy: Pass a comma separated list of fit "
               "routines to run in order. Default is Migrad,FixAtLim \n";
  std::cout << "                    Possible Options: \n";
  std::cout << "                      1. Migrad - Minuit2 Migrad Minimizer \n";
  std::cout << "                      2. Simplex - Simplex Minimizer \n";
  std::cout << "                      3. Scan - Brute force scan of parameter "
               "space \n";
  std::cout << "                      4. FixAtLim - Takes any free parameters "
               "close to a limit and fixes them \n";
  std::cout << "                      5. Scan1D - Make 1D Scans and save them "
               "in a folder \n";
  std::cout << "                      6. Contours - Make Contour Scans \n";
  std::cout << "                      7. Save - Will save the state of the "
               "fitter (Always done by default at the end) \n";
  std::cout << "                      Extra option LowStatFit will perform "
               "each of these options with a lower number \n";
  std::cout << "                      of fit events (config lowstat). Example: "
               "LowStatMigrad, LowStatScan \n";
  std::cout << "     -d fakeDataFile: Uses the MC generated from a previous "
               "fit as a fake data set for these fits \n";
  std::cout << "                      Can also specify MC to set the fake-data "
               "to the Monte-Carlo prediction\n";
  std::cout << "                         In this case, you can specify "
               "fake_parameter PARAM_NAME PARAM_VALUE in the card\n";
  std::cout << "                         to reweight the MC parameter "
               "PARAM_NAME to some PARAM_VALUE. The minimiser will start\n";
  std::cout << "                         at whatever neut_parameter or "
               "genie_parameter is set to in the cardfile.\n";
  std::cout << "     -i inputFile:  Uses the results from a previous fit file "
               "as starting input for these fits \n";
  std::cout << "     -q config_name=config_val : Allows any config parameter "
               "to be overridden from the command line.\n";
  std::cout << "                                 This will take priority over "
               "those given in the default, or cardFile. \n";
  std::cout << "                                 example: -q verbosity=6 -q "
               "maxevents=10000 \n";

  exit(-1);
};

//*******************************
int main(int argc, char *argv[]) {
  //*******************************

  // Program status;
  int status = 0;

  // If No Arguments print commands
  if (argc == 1)
    printInputCommands();

  for (int i = 1; i < argc; ++i) {
    // Cardfile
    if (!std::strcmp(argv[i], "-h"))
      printInputCommands();
    else
      break;
  }

  // Read input arguments such as card file, parameter arguments, and fit
  // routines
  NUIS_LOG(FIT, "Starting nuismin");

  // Make minimizer class and run fit
  MinimizerRoutines *min = new MinimizerRoutines(argc, argv);

  // Save Starting States
  if (FitPar::Config().GetParB("savenominal"))
    min->SaveNominal();
  if (FitPar::Config().GetParB("saveprefit"))
    min->SavePrefit();

  // Run the fit routines
  min->Run();

  // Save by default
  min->SaveResults();

  // Get Status
  status = min->GetStatus();

  // Show Final Status
  NUIS_LOG(FIT, "-------------------------------------");
  if (status == 0) {
    NUIS_LOG(FIT, "Minimizer Complete.");
  } else {
    NUIS_ERR(WRN, "Minimizer Failed (error state = " << status << ")");
  }
  NUIS_LOG(FIT, "-------------------------------------");
  
  delete min;
  return status;
}
