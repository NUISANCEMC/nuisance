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
void printInputCommands(){
//*******************************
  
  std::cout << "nuiscomp -c cardfile -o outfile [-q configname=configval] \n";
  std::cout << "\n Arguments : \n";
  std::cout << "   -c cardfile: Path to NUISANCE card file defining fit samples \n";
  std::cout <<"     -o outFile:    Path to root file that will be created to save output file.\n";
  std::cout <<"     -q config_name=config_val : Allows any config parameter to be overridden from the command line.\n";
  std::cout <<"                                 This will take priority over those given in the default, or cardFile. \n";
  std::cout <<"                                 example: -q verbosity=6 -q maxevents=10000 \n" << std::endl;

  exit(-1);

};

//*******************************
int main(int argc, char* argv[]){ 
//*******************************

  // Program status;
  int status = 0;
  
  // If No Arguments print commands
  if (argc == 1) printInputCommands();
    
  for (int i = 1; i< argc; ++i){
    // Cardfile
    if (!std::strcmp(argv[i], "-h")) printInputCommands();
    else break;
  }
  
  // Read input arguments such as card file, parameter arguments, and fit routines
  std::cout<<"Starting nuiscomp.exe"<<std::endl;

  // Make minimizer class and run fit
  ComparisonRoutines* min = new ComparisonRoutines(argc, argv);
  
  // Save Starting States
  if (FitPar::Config().GetParB("savenominal")) min->SaveNominal();

  // Run the fit rotines
  min->Run();
  min->SaveCurrentState();

  // Show Final Status
  std::cout<<"-------------------------------------"<<std::endl;
  std::cout<<"Comparison Complete."<<std::endl;
  std::cout<<"-------------------------------------"<<std::endl;
  
  return status;
}



