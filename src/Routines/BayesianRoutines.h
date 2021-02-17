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

#ifndef BAYESIAN_ROUTINES_H
#define BAYESIAN_ROUTINES_H

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

#include <sys/time.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cstring>

#include "FitEvent.h"
#include "JointFCN.h"

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
//! Collects all possible fit routines into a single class to avoid repeated code
class BayesianRoutines{
//*************************************

public:

  /*
    Constructor/Destructor
  */

  //! Constructor reads in arguments given at the command line for the fit here.
  BayesianRoutines(int argc, char* argv[]);
    
  //! Default destructor
  ~BayesianRoutines();

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

  /*
    Setup Functions
  */
  void SetupSystematicsFromXML();

  //! Setups up our custom RW engine with all the parameters passed in the card file
  void SetupRWEngine();

  //! Setups up the jointFCN.
  void SetupFCN();

  /*
    Fitting Functions
  */

  //! Main function to actually start iterating over the different required fit routines
  void Run();

  //! Given a new map change the values that the RW engine is currently set to
  void UpdateRWEngine(std::map<std::string,double>& updateVals);

  //! Given a single routine (see tutorial for options) run that fit routine now.
  int RunFitRoutine(std::string routine);

  //! Throw the current covariance of dial values we have, and fill the thrownVals and thrownNorms maps.
  //! If uniformly is true parameters will be thrown uniformly between their upper and lower limits.
  void ThrowParameters();

  //! Run Throws   
  void GenerateThrows();
 
protected:

  //! Our Custom ReWeight Object
  FitWeight* rw;

  std::string fOutputFile;
  std::string fInputFile;

  TFile* fInputRootFile;
  TFile* fOutputRootFile;

  //! Flag for whether the fit should be continued if an output file is already found.
  bool fitContinue;

  //! Minimizer Object for handling roots different minimizer methods
  JointFCN* fSampleFCN;

  int nfreepars;

  std::string fCardFile;

  std::string fStrategy;
  std::vector<std::string> fRoutines;
  std::string fAllowedRoutines;
  
  std::string fFakeDataInput;

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

  //! Vector of fake parameter names
  std::map<std::string,double> fFakeVals;

  //! Map of thrown parameter names and values (After ThrowCovariance)
  std::map<std::string,double> fThrownVals;


  std::list   <ParamPull*>  fInputThrows; //!< Pointers to pull terms
  std::vector <TH1D>        fInputDials; //!< Vector of Input Histograms
  std::vector <TMatrixDSym> fInputCovar; //!< Vector of Input Covariances  

  nuiskey fCompKey;
  std::vector<std::string> fThrowList;
  std::string fThrowString;

  int fNThrows;
  int fStartThrows;


};

/*! @} */
#endif
