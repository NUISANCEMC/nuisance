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

//*******************************
void printInputCommands() {
//*******************************

  std::cout << "NUISCOMP : NUISANCE Fixed Comparison App\n\n";
  std::cout << "Running with cardfile usage example : \n";
  std::cout << "  nuiscomp -c cardfile [-o outfile] [-n nevents] [-q config=val] [-i 'xmlstruct']\n";
  std::cout << "\n  Arguments : \n";
  std::cout << "\t  -c cardfile    : Path to NUISANCE card file defining fit samples \n\n";
  std::cout << "\t[ -o outfile ]   : Path to root file that will be created to save output file.\n\n";
  std::cout << "\t[ -n nevents ]   : Sets Max Events \n\n";
  std::cout << "\t[ -q config=val ]: Allows any config parameter to be overridden from the command line.\n";
  std::cout << "\t                   This will take priority over those given in the default, or cardFile. \n";
  std::cout << "\t                   (e.g.): -q verbosity=6 -q maxevents=10000 \n\n";
  std::cout << "\t[ -i 'xmlstruc' ]: Include additional xml/simple-card structures at runtime. \n";
  std::cout << "\t                   (        xml e.g.): -i '\'sample name=\"MiniBooNE_CCQE_XSec_1DQ2_nu\""
            << " input=\"NEUT:miniboone_events.root\"\'" << std::endl;
  std::cout << "\t                   (simple-card e.g.): -i \'sample MiniBooNE_CCQE_XSec_1DQ2_nu"
            << " NEUT:miniboone_events.root\' \n\n\n" << std::endl;

  std::cout << "Running with cmd-line xmlstruct useage example : \n";
  std::cout << "  nuiscomp -i 'xmlstruct' -o outfile [-n nevents] [-q config=val] [-i 'xmlstruct']\n";
  std::cout << "\n  Arguments : \n";
  std::cout << "\t  -i 'xmlstruc' : At least one xml struct is required if running without a card file. "
            << "See below for struct examples. \n\n";
  std::cout << "\t  -o outfile    : Path to root file that will be created to save output file (REQUIRED) \n\n";
  std::cout << "\t[ -n nevents ]   : Sets Max Events \n\n";
  std::cout << "\t[ -q config=val ]: Allows any config parameter to be overridden from the command line.\n";
  std::cout << "\t                   This will take priority over those given in the default, or cardFile. \n";
  std::cout << "\t                   (e.g.): -q verbosity=6 -q maxevents=10000 \n\n";
  std::cout << "\t[ -i 'xmlstruc' ]: Include additional xml/simple-card structures at runtime. \n";
  std::cout << "\t                   (        xml e.g.): -i \'sample name=\"MiniBooNE_CCQE_XSec_1DQ2_nu\""
            << " input=\"NEUT:miniboone_events.root\"\'" << std::endl;
  std::cout << "\t                   (simple-card e.g.): -i \'sample MiniBooNE_CCQE_XSec_1DQ2_nu"
            << " NEUT:miniboone_events.root\' \n" << std::endl;

  exit(-1);

};

//*******************************
int main(int argc, char* argv[]) {
  //*******************************

  // Program status;
  int status = 0;

  // If No Arguments print commands
  if (argc == 1) printInputCommands();

  for (int i = 1; i < argc; ++i) {
    // Cardfile
    if (!std::strcmp(argv[i], "-h")) printInputCommands();
    else break;
  }

  // Read input arguments such as card file, parameter arguments, and fit routines
  LOG() << "Starting nuiscomp.exe" << std::endl;

  // Make minimizer class and run fit
  ComparisonRoutines* comp = new ComparisonRoutines(argc, argv);
  comp->Run();

  // Show Final Status
  LOG(FIT) << "------------------------------------ -" << std::endl;
  LOG(FIT) << "Comparison Complete." << std::endl;
  LOG(FIT) << "------------------------------------ -" << std::endl;

  return status;
}



