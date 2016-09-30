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
#include "FitParameters.h"
#include "ParamPull.h"

#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "FitLogger.h"

//*************************************
//! Collects all possible fit routines into a single class to avoid repeated code
class systematicRoutines{
//*************************************

public:

  /*
    Constructor/Destructor
  */

  //! Constructor reads in arguments given at the command line for the fit here.
  systematicRoutines(int argc, char* argv[]);

  //! Default destructor
  ~systematicRoutines();

  /*
    Input Functions
  */

  //! Splits the arguments ready for initial setup
  void ParseArgs(int argc, char* argv[]);

  //! Sorts out configuration and verbosity right at the very start.
  //! Calls readCard to set everything else up.
  void InitialSetup();

  //! Loops through each line of the card file and passes it to other read functions
  void ReadCard();

  void GetCovarFromFCN();
  
  //! Check for parameter string in the line and assign the correct type.
  //! Fills maps for each of the parameters
  void readParameters(std::string parstring);

  //! Reads in fake parameters and assigns them (Requires the parameter to be included as a normal parameter as well)
  void readFakeDataPars(std::string parstring);

  //! Read in the samples so we can set up the free normalisation dials if required
  void readSamples(std::string sampleString);

  //! Read in a parameter pulls class as a covariance for the fit. Currently just used for throwing covariances but will change covariance pull
  //! terms to use this method soon so it is a bit clearer.
  void readCovariance(std::string covarString);

  /*
    Setup Functions
  */


  //! Setup the configuration given the arguments passed at the commandline and card file
  void setupConfig();

  //! Setups up our custom RW engine with all the parameters passed in the card file
  void setupRWEngine();

  //! Setups up the jointFCN.
  void setupFCN();

  //! Set the current data histograms in each sample to the fake data.
  void setFakeData();

  //! Setup the covariances with the correct dimensions. At the start this is either uncorrelated or merged given all the input covariances.
  //! At the end of the fit this produces the blank covariances which can then be filled by the minimizerObj with best fit covariances.
  void SetupCovariance();

  //! Check all dials have been added to the input card so we can grab the limits
  void checkCovarianceDials(bool printdials);

  /*
    Fitting Functions
  */

  //! Main function to actually start iterating over the different required fit routines
  void SelfFit();

  //! Given a new map change the values that the RW engine is currently set to
  void updateRWEngine(std::map<std::string,double>& updateVals);

  //! Given a single routine (see tutorial for options) run that fit routine now.
  void RunFitRoutine(std::string routine);

  //! Throw the current covariance of dial values we have, and fill the thrownVals and thrownNorms maps.
  //! If uniformly is true parameters will be thrown uniformly between their upper and lower limits.
  void ThrowCovariance(bool uniformly);

  //! Step through each parameter one by one and create folders containing the MC predictions at each step.
  //! Doesn't handle correlated parameters well
  void PlotLimits();

  //! Given the covariance we currently have generate error bands by throwing the covariance.
  //! The FitPar config "error_uniform" defines whether to throw using the covariance or uniformly.
  //! The FitPar config "error_throws" defines how many throws are needed.
  //! Currently only supports TH1D plots.
  void GenerateErrorBands();

  /*
    Write Functions
  */

  //! Write plots and TTrees listing the minimizerObj result of the fit to file
  void saveMinimizerState();

  //! Save the sample plots for current MC
  //! dir if not empty forces plots to be saved in a subdirectory of outputfile
  void saveCurrentState(std::string subdir="");

  //! Save starting predictions into a seperate folder
  void saveNominal();

  //! Save predictions before the fit is ran into a seperate folder
  void savePrefit();

  /*
    MISC Functions
  */

  //! Get previous fit status from a file
  Int_t GetStatus();

protected:

  //! Our Custom ReWeight Object
  FitWeight* rw;

  // I/O
  std::string outputFileName;
  std::string inputFileName;

  //! Input file from previous fit
  TFile* inputFile;

  //! Output file where the results are saved
  TFile* outputFile;

  //! Flag for whether the fit should be continued if an output file is already found.
  bool fitContinue;

  //! The actual chi2 Function from FCN module
  JointFCN* thisFCN;

  //! Current number of free parameters. callFCN requires this when being setup.
  int nfreepars;

  // Fit Options

  //! Input cardfile containing fit samples and dials
  std::string cardFile;

  //! comma seperated list of fit routines to be run
  std::string fitStrategy;

  //! fitStrategy is parsed into this vector of fit routines. Each one is run in sequence and current values updated.
  std::vector<std::string> fit_routines;

  //! Directory of a fake data MC file to use for this fit. If "MC" is provided the starting nominal MC and provided fake_parameters are used as fake data.
  std::string fakeDataFile;

  //! Current fit type.
  int type;

  // Parameter Configs
  //! vector command line config overrides
  std::vector<std::string> configCmdFix;

  // Input Dial Values
  //! Vector of dial names
  std::vector<std::string> params;

  //! Map of dial names and dial types. 0 NEUT 1 NIWG 2 GENIE 3 NUWRO 4 CUSTOM
  std::map<std::string,int> params_type;

  //! Map of dial names and starting values given in the fit card
  std::map<std::string, double> startVals;

  //! Map of dial names and current values at this stage in the overall fitting routine
  std::map<std::string, double> currentVals;

  //! Map of dial names and current errors at this stage
  std::map<std::string, double> errorVals;

  //! Map of dial names and the minimum limits provided in the fit card
  std::map<std::string, double> minVals;

  //! Map of dial names and the maximum limits provided in the fit card
  std::map<std::string, double> maxVals;

  //! Map of dial names and the step values for this dial provided in the fit card.
  //! Minuit seems to ignore this anyway, but it is used in the PlotLimits routine.
  std::map<std::string, double> stepVals;

  //! Map of dial names and flags for whether the parmaeter is currently fixed.
  std::map<std::string, bool>   fixVals;

  //! Map of dial names and flags for whether the parameter started fixed.
  std::map<std::string, bool>   startFixVals;

  // Input Samples

  //! Vector of samples included in the fit
  std::vector<std::string> samples;

  //! Vector of sample normalisation dial names (sample + "_norm")
  std::vector<std::string> sampleDials;

  //! Map of norm dial names and the starting value given in the fit card
  std::map<std::string, double> sampleNorms;

  //! Map of norm dial names and the current value in the fit
  std::map<std::string, double> currentNorms;

  //! Map of norm dial names and the current error in the fit
  std::map<std::string, double> errorNorms;

  //! Map of norm dial names and whether it is currently fixed in the fit
  std::map<std::string, bool> fixNorms;

  //! Map of norm dial names and whether it started fixed
  std::map<std::string, bool> startFixNorms;

  //! Map of norm dial names and the files string input for those samples
  std::map<std::string, std::string> sampleFiles;

  //! Map of norm dial names and the type string for those samples
  std::map<std::string, std::string> sampleTypes;

  // Input Fake Data Sets

  //! Vector of fake parameter names
  std::vector<std::string> fakeParams;

  //! Vector of fake parameter sample norms
  std::vector<std::string> fakeSamples;

  //! Map of fake dial names and their values
  std::map<std::string,double> fakeVals;

  //! Map of fake sample norm names and their values
  std::map<std::string,double> fakeNorms;

  //! Map of thrown parameter names and values (After ThrowCovariance)
  std::map<std::string,double> thrownVals;

  //! Map of thrown parameter names and values (After ThrowCovariance)
  std::map<std::string,double> thrownNorms;


  TH2D* covarHist; //!< Covariance matrix for the fit. At the start this is uncorrelated unless input covars are provided.
  TH2D* covarHist_Free; //!< Covariance matrix for the free parameters fit. At the start this is uncorrelated unless input covars are provided.
  TH2D* correlHist; //!< Correlation matrix for the fit. At the start this is uncorrelated unless input covars are provided.
  TH2D* correlHist_Free; //!< Correlation matrix for the free parameters fit. At the start this is uncorrelated unless input covars are provided.
  TH2D* decompHist; //!< Decomposition matrix for the fit. At the start this is uncorrelated unless input covars are provided.
  TH2D* decompHist_Free; //!< Decomposition matrix for the free parameters fit. At the start this is uncorrelated unless input covars are provided.

  std::vector<TH1D*> input_dials;  //!< Vector of histograms from parameter pull classes that give central values of the input pull terms
  std::vector<TH2D> input_covariances; //!< vector of histograms from parameter pull classes that give covariance of the input pull terms

  std::list   <ParamPull*>  fInputThrows; //!< Pointers to pull terms
  std::vector <TH1D>        fInputDials; //!< Vector of Input Histograms
  std::vector <TMatrixDSym> fInputCovar; //!< Vector of Input Covariances

  TH2D* fCovar;
  TH2D* fCorrel;
  TH2D* fDecomp;

  TH2D* fCovarFree;
  TH2D* fCorrelFree;
  TH2D* fDecompFree;
  
};

/*! @} */
#endif
