#ifndef SPLINE_ROUTINE_H
#define SPLINE_ROUTINE_H

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
#include "FitBuild.h"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cstring>

#include "FitEvent.h"
#include "minimizerFCN.h"
#include "FitParameters.h"

#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "FitLogger.h"

//*************************************
//! Collects all possible fit routines into a single class to avoid repeated code
class splineRoutines{
//*************************************
  
public:

  /*                                                                                                                    
    Constructor/Destructor                                                                                              
  */

  //! Constructor reads in arguments given at the command line for the fit here.
  splineRoutines(int argc, char* argv[]);

  //! Default destructor
  ~splineRoutines();

  /*                                                                                                                    
    Input Functions                                                                                                     
  */

  //! Splits the arguments ready for initial setup
  void parseArgs(int argc, char* argv[]);

  //! Sorts out configuration and verbosity right at the very start.
  //! Calls readCard to set everything else up.
  void initialSetup();

  //! Loops through each line of the card file and passes it to other read functions
  void readCard();

  //! Check for parameter string in the line and assign the correct type.
  //! Fills maps for each of the parameters
  void readParameters(std::string parstring);

  //! Read in the samples so we can set up the free normalisation dials if required
  void readSamples(std::string sampleString);

  //! Read in splines from card
  void readSplines(std::string splineString);

  //! Setup the configuration given the arguments passed at the commandline and card file
  void setupConfig();

  //! Setups up our custom RW engine with all the parameters passed in the card file
  void setupRWEngine();

  //! Given a new map change the values that the RW engine is currently set to
  void updateRWEngine(std::map<std::string,double>& updateVals);
  
  // ROUTINES ----
  
  //! Loop all input samples, generate a class and splines
  void GenerateSampleSplines();

  //! For all samples generate spline fit plots for a sub-sample of the events
  void MakeSplinePlots();

  //! Generate class for each sample and throw dials N times to compare RW and spline output when binned
  //! in official sample distributions. Flags if large disagreements occur.
  //! Each dial is handled individually, then pairwise, then all dials are thrown together.
  //! Maximum bin differences are saved for each event.
  void ValidateSplineBinResponse();

  //! Generate input handler for each sample and perform previous throw procedures on an event by event basis.
  //! Done over a selected sub range of the events for N throws. Maximum dial differences are saved for each event.
  void ValidateSplineEventResponse();

  void CheckSplinePlots();
  void ValidePlots();
  void Run();
  
protected:
  //! Our Custom ReWeight Object       
  FitWeight* rw;
  
  //! Input cardfile containing fit samples and dials
  std::string cardFile;

  //! Output file for validation plots
  std::string outFile;
  TFile* outRootFile;
  
  //! comma seperated list of fit routines to be run
  std::string splineStrategy;

  //! fitStrategy is parsed into this vector of fit routines. Each one is run in sequence and current values updated.
  std::vector<std::string> spline_routines;

  //! Command line overrides for pars file
  std::vector<std::string> configCmdFix;
  
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

  // Input Samples

  //! Vector of samples included in the fit
  std::vector<std::string> samples;

  //! Map of norm dial names and the files string input for those samples
  std::map<std::string, std::string> sampleInFiles;

  //! Map of sample names and the files string output 
  std::map<std::string, std::string> sampleOutFiles;
  
  //! Map of norm dial names and the type string for those samples
  std::map<std::string, std::string> sampleTypes;
  
  std::map<std::string, std::string> sampleOutTypes;

  // Splines
  std::vector<std::string> splineNames;
  std::map<std::string, std::string> splineTypes;
  std::map<std::string, std::string> splinePoints;

};

/*! @} */
#endif
