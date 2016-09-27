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

// Author: Patrick Stowell     09/2016
/*
  Usage: ./nuissyst -c card file -o output file, where the results of the throws are stored
   where:   
*/

#include "systematicRoutines.h"

//*******************************
void printInputCommands(){
//*******************************
  
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
  std::cout<<"Starting nuissyst"<<std::endl;

  // Make systematic class and run fit
  //systematicRoutines* min = new systematicRoutines(argc, argv);
  //  min->initialSetup();



  
  // Show Final Status
  std::cout<<"-------------------------------------"<<std::endl;
  if (status == 0) std::cout<<"Systematic Complete."<<std::endl;
  else std::cerr<<"Systematic Failed (error state = "<<status<<")"<<std::endl;
  std::cout<<"-------------------------------------"<<std::endl;
  
  return status;
}



