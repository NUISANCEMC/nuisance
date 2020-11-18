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
/*! \addtogroup Routines @{ */

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

#include "GeneralUtils.h"
#include "NuisConfig.h"
#include "NuisKey.h"
#include "FitLogger.h"
#include "ParserUtils.h"

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
/// Collects all possible fit routines into a single class to avoid repeated code
class ComparisonRoutines {
//*************************************

public:

  /*
    Constructor/Destructor
  */

  /// Constructor reads in arguments given at the command line for the fit here.
  ComparisonRoutines(int argc, char* argv[]);
    
  /// Default destructor
  ~ComparisonRoutines();

  /// Reset everything to default/NULL
  void Init();
  
  /*
    Input Functions
  */

  /// Queries configuration keys to setup Parameters/Samples/FakeParameters
  void SetupComparisonsFromXML();

  /*
    Setup Functions
  */

  /// Setups up our custom RW engine with all the parameters passed in the card file
  void SetupRWEngine();

  /// Setups up the jointFCN.
  void SetupFCN();

  /// Set the current data histograms in each sample to the fake data.
  void SetFakeData();

  /*
    Fitting Functions
  */

  /// Main function to actually start iterating over the different required fit routines
  void Run();

  /// Creates a comparison from FCN
  void GenerateComparison();
  
  /// Given a new map change the values that the RW engine is currently set to
  void UpdateRWEngine(std::map<std::string,double>& updateVals);

  /// Print current value
  void PrintState();
  
  /*
    Write Functions
  */

  /// Save the sample plots for current MC
  /// dir if not empty forces plots to be saved in a subdirectory of outputfile
  void SaveCurrentState(std::string subdir="");

  /// Save starting predictions into a separate folder
  void SaveNominal();

  /*
    MISC Functions
  */

  /// Get previous fit status from a file
  Int_t GetStatus();

protected:

  //! Our Custom ReWeight Object
  FitWeight* rw;

  std::string fOutputFile; ///< Output file name
  // std::string fInputFile;  ///< Input file name

  // TFile* fInputRootFile;   ///< 
  TFile* fOutputRootFile; ///< Output ROOT TFile

  JointFCN* fSampleFCN; ///< Joint Samples Container that handles reconfigures.

  std::string fCardFile; ///< Input card/XML file.

  std::string fStrategy; ///< Comparison routine selection.
  std::vector<std::string> fRoutines; ///< Split vector of comparison routine selection.
  std::string fAllowedRoutines; ///< Hard coded list of allowed routines.
  
  /// Fake data flag. Can be 'MC' to use 'fake_parameter'
  /// or 'path_to_file.root' to use previous NUISANCE MC predictions.
  std::string fFakeDataInput; 

  // Input Dial Vals
  std::vector<std::string> fParams; ///< Vector of dial names.
  std::map<std::string, std::string> fStateVals; ///< Map of dial states
  std::map<std::string, double>      fCurVals; ///< Map of dial values
  std::map<std::string, int>         fTypeVals; ///< Map of dial type enums.

  // Fake Dial Vals
  std::map<std::string,double> fFakeVals; ///< Map of fake data settings.

  // Configuration
  nuiskey fCompKey; ///< Configuration Key for this Comparison Instance

};

/*! @} */
#endif
