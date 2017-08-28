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

#ifndef SYSTEMATIC_ROUTINES_H
#define SYSTEMATIC_ROUTINES_H

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
#include "FitParameters.h"
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
class SystematicRoutines{
//*************************************

public:

  /*
    Constructor/Destructor
  */

  //! Constructor reads in arguments given at the command line for the fit here.
  SystematicRoutines(int argc, char* argv[]);
    
  //! Default destructor
  ~SystematicRoutines();

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

  //! Reads in fake parameters and assigns them (Requires the parameter to be included as a normal parameter as well)
  int ReadFakeDataPars(std::string parstring);

  //! Read in the samples so we can set up the free normalisation dials if required
  int ReadSamples(std::string sampleString);

  /*
    Setup Functions
  */
void SetupSystematicsFromXML();

  //! Setup the configuration given the arguments passed at the commandline and card file
  void SetupConfig();

  //! Setups up our custom RW engine with all the parameters passed in the card file
  void SetupRWEngine();

  //! Setups up the jointFCN.
  void SetupFCN();

  //! Sets up the minimizerObj for ROOT. there are cases where this is called repeatedly, e.g. If you are using a brute force scan before using Migrad.
  void SetupFitter(std::string routine);

  //! Set the current data histograms in each sample to the fake data.
  void SetFakeData();

  //! Setup the covariances with the correct dimensions. At the start this is either uncorrelated or merged given all the input covariances.
  //! At the end of the fit this produces the blank covariances which can then be filled by the minimizerObj with best fit covariances.
  void SetupCovariance();

  void GetCovarFromFCN();
  /*
    Fitting Functions
  */

  //! Main function to actually start iterating over the different required fit routines
  void Run();

  //! Given a new map change the values that the RW engine is currently set to
  void UpdateRWEngine(std::map<std::string,double>& updateVals);

  //! Given a single routine (see tutorial for options) run that fit routine now.
  int RunFitRoutine(std::string routine);

  //! Print current value
  void PrintState();
  
  //! Performs a fit routine where the input.maxevents is set to a much lower value to try and move closer to the best fit minimum.
  void LowStatRoutine(std::string routine);

  //! Perform a chi2 scan in 1D around the current point
  void Create1DScans();

  //! Perform a chi2 scan in 2D around the current point
  void Chi2Scan2D();

  //! Currently a placeholder NEEDS UPDATING
  void CreateContours();

  //! If any currentVals are close to the limits set them to the limit and fix them
  int FixAtLimit();

  //! Throw the current covariance of dial values we have, and fill the thrownVals and thrownNorms maps.
  //! If uniformly is true parameters will be thrown uniformly between their upper and lower limits.
  void ThrowCovariance(bool uniformly);

  //! Given the covariance we currently have generate error bands by throwing the covariance.
  //! The FitPar config "error_uniform" defines whether to throw using the covariance or uniformly.
  //! The FitPar config "error_throws" defines how many throws are needed.
  //! Currently only supports TH1D plots.
  void GenerateErrorBands();
  
  void GenerateThrows();
  void MergeThrows();
  //! Step through each parameter one by one and create folders containing the MC predictions at each step.
  //! Doesn't handle correlated parameters well
  void PlotLimits();
  
  /*
    Write Functions
  */

  //! Save the sample plots for current MC
  //! dir if not empty forces plots to be saved in a subdirectory of outputfile
  void SaveCurrentState(std::string subdir="");

  //! Save starting predictions into a seperate folder
  void SaveNominal();

  //! Save predictions before the main study is ran into a seperate folder
  void SavePrefit();

  //! Save final outputs
  void SaveResults();
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

  TH2D* fCorrel;
  TH2D* fDecomp;
  TH2D* fCovar;
  
  TH2D* fCorrelFree;
  TH2D* fDecompFree;
  TH2D* fCovarFree;

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
