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

#ifndef SPLINE_ROUTINES_H
#define SPLINE_ROUTINES_H

/*!
 *  \addtogroup Minimizer
 *  @{
 */

#include "TH1.h"
#include "TF1.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "Minuit2/FCNBase.h"
#include "TFitterMinuit.h"
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
#include "MinimizerFCN.h"
#include "FitParameters.h"

#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "FitLogger.h"
#include "BaseFitEvt.h"

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
class SplineRoutines{
//*************************************

public:

  /*
    Constructor/Destructor
  */

  //! Constructor reads in arguments given at the command line for the fit here.
  SplineRoutines(int argc, char* argv[]);
    
  //! Default destructor
  ~SplineRoutines();

  //! Reset everything to default/NULL
  void Init();
  
  /*
    Input Functions
  */

  //! Splits the arguments ready for initial setup
  void ParseArgs(int argc, char* argv[]);

  //! Sorts out configuration and verbosity right at the very start.
  //! Calls readCard to set everything else up.
  void InitialSetup();

  //! Loops through each line of the card file and passes it to other read functions
  void ReadCard(std::string cardfile);

  //! Check for parameter string in the line and assign the correct type.
  //! Fills maps for each of the parameters
  int ReadParameters(std::string parstring);

  //! Read in the samples so we can set up the free normalisation dials if required
  int ReadSamples(std::string sampleString);

  //! Read Generic Inputs
  int ReadGenericInputs(std::string sampleString);

  //! Read Event Splines
  int ReadEventSplines(std::string splstring);

  //! Read Bin Splines
  int ReadBinSplines(std::string binstring){ return kGoodStatus; };
  
  /*
    Setup Functions
  */

  //! Setup the configuration given the arguments passed at the commandline and card file
  void SetupConfig();

  //! Setups up our custom RW engine with all the parameters passed in the card file
  void SetupRWEngine();

  //! Setups up the jointFCN and uses it to grab samples.
  void SetupSamples();

  void SetupGenericInputs();

  void SaveEvents();

  void SaveEventSplines();

  void TestEventSplines();
  /*
    Fitting Functions
  */

  //! Main function to actually start iterating over the different required fit routines
  void Run();

  //! Given a new map change the values that the RW engine is currently set to
  void UpdateRWEngine(std::map<std::string,double>& updateVals);

  /*
    MISC Functions
  */

  //! Get previous fit status from a file
  Int_t GetStatus();

protected:

  //! Our Custom ReWeight Object
  FitWeight* rw;
  FitWeight* fRW;
  
  std::string fOutputFile;
  std::string fInputFile;

  TFile* fInputRootFile;
  TFile* fOutputRootFile;
  
  JointFCN* fSampleFCN;
  std::list<MeasurementBase*> fSamples;
  
  std::string fCardFile;

  std::string fStrategy;
  std::vector<std::string> fRoutines;
  std::string fAllowedRoutines;
  
  // Input Dial Vals
  //! Vector of dial names
  std::vector<std::string> fParams;
  std::map<std::string, std::string> fStateVals;
  std::map<std::string, double>      fStartVals;
  std::map<std::string, double>      fCurVals;
  std::map<std::string, double>      fErrorVals;
  std::map<std::string, double>      fMinVals;
  std::map<std::string, double>      fMaxVals;
  std::map<std::string, double>      fStepVals;
  std::map<std::string, int>         fTypeVals;
  std::map<std::string, bool>        fFixVals;
  std::map<std::string, bool>        fStartFixVals;

  std::vector<std::string> fGenericInputNames;
  std::map<std::string, std::string> fGenericInputFiles;
  std::map<std::string, std::string> fGenericOutputFiles;
  std::map<std::string, std::string> fGenericOutputTypes;
  std::map<std::string, InputHandler*> fGenericInputs;


  std::vector<std::string> fSplineNames;
  std::map<std::string, std::string> fSplineTypes;
  std::map<std::string, std::string> fSplinePoints;
  
  
};

/*! @} */
#endif
