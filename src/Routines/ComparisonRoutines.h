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

#ifndef COMPARISON_ROUTINES_H
#define COMPARISON_ROUTINES_H

/*!
 *  \addtogroup Minimizer
 *  @{
 */

#include "TH1.h"
#include "TF1.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TSystem.h"
#include "TFile.h"
#include "TProfile.h"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cstring>

#include "FitEvent.h"
#include "JointFCN.h"
#include "FitParameters.h"
#include "GeneralUtils.h"

#include "FitLogger.h"

enum minstate {
  kErrorStatus = -1,
  kGoodStatus,
  kFitError,
  kNoChange,
  kFitFinished,
  kFitUnfinished,
  kStateChange,
};

//*************************************
//! Collects all possible fit routines into a single class to avoid repeated code
class ComparisonRoutines{
//*************************************

public:

  /*
    Constructor/Destructor
  */

  //! Constructor reads in arguments given at the command line for the fit here.
  ComparisonRoutines(int argc, char* argv[]);
    
  //! Default destructor
  ~ComparisonRoutines();

  //! Reset everything to default/NULL
  void Init();
  
  /*
    Input Functions
  */

  //! Sorts out configuration and verbosity right at the very start.
  //! Calls readCard to set everything else up.
  void InitialSetup();

  //! Loops through each line of the card file and passes it to other read functions
  void ReadCard(std::string cardfile);

  //! Check for parameter string in the line and assign the correct type.
  //! Fills maps for each of the parameters
  int ReadParameters(std::string parstring);

  //! Reads in fake parameters and assigns them (Requires the parameter to be included as a normal parameter as well)
  int ReadFakeDataPars(std::string parstring);

  //! Read in the samples so we can set up the free normalisation dials if required
  int ReadSamples(std::string sampleString);

  /*
    Setup Functions
  */

  //! Setup the configuration given the arguments passed at the commandline and card file
  void SetupConfig();

  //! Setups up our custom RW engine with all the parameters passed in the card file
  void SetupRWEngine();

  //! Setups up the jointFCN.
  void SetupFCN();

  //! Set the current data histograms in each sample to the fake data.
  void SetFakeData();

  /*
    Fitting Functions
  */

  //! Main function to actually start iterating over the different required fit routines
  void Run();

  //! Creates a comparison from FCN
  void GenerateComparison();
  
  //! Given a new map change the values that the RW engine is currently set to
  void UpdateRWEngine(std::map<std::string,double>& updateVals);

  //! Print current value
  void PrintState();
  
  /*
    Write Functions
  */

  //! Save the sample plots for current MC
  //! dir if not empty forces plots to be saved in a subdirectory of outputfile
  void SaveCurrentState(std::string subdir="");

  //! Save starting predictions into a seperate folder
  void SaveNominal();

  /*
    MISC Functions
  */

  //! Get previous fit status from a file
  Int_t GetStatus();

protected:

  //! Our Custom ReWeight Object
  FitWeight* rw;

  std::string fOutputFile;
  std::string fInputFile;

  TFile* fInputRootFile;
  TFile* fOutputRootFile;

  JointFCN* fSampleFCN;

  std::string fCardFile;

  std::string fStrategy;
  std::vector<std::string> fRoutines;
  std::string fAllowedRoutines;
  
  std::string fFakeDataInput;

  // Input Dial Vals
  //! Vector of dial names
  std::vector<std::string> fParams;
  std::map<std::string, std::string> fStateVals;
  std::map<std::string, double>      fCurVals;
  std::map<std::string, int>         fTypeVals;

  //! Vector of fake parameter names
  std::map<std::string,double> fFakeVals;

};

/*! @} */
#endif
