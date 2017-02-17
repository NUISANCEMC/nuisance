#ifndef _JOINT_FCN_H_
#define _JOINT_FCN_H_
/*!                                                                                                                                                                                                   
 *  \addtogroup FCN                                                                                                                                                                                 
 *  @{                                                                                                                                                                                                
 */

#include <iostream>
#include <vector>
#include <fstream>
#include <list>

// ROOT headers
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include <TMatrixDSym.h>
#include "TGraphErrors.h"
#include <TVectorD.h>
#include <TMath.h>

#include "FitUtils.h"

// External fitter headers
#include "MeasurementBase.h"
#include "SampleList.h"

#define GetCurrentDir getcwd
#include "EventManager.h"
#include "Math/Functor.h"
#include "ParamPull.h"

#include "NuisConfig.h"
#include "NuisKey.h"

using namespace FitUtils;
using namespace FitBase;
//! Main FCN Class which ROOT's joint function needs to evaulate the chi2 at each stage of the fit.
class JointFCN 
{
 public:

  //! Constructor
  //! cardfile = Path to input card file listing samples
  JointFCN(std::string cardfile, TFile *outfile=NULL);
  JointFCN(std::vector<nuiskey> samplekeys, TFile* outfile=NULL);
  JointFCN(TFile* outfile=NULL); // Loads from global config
  //! Destructor
  ~JointFCN();

  //! Create sample list from cardfile
  void LoadSamples(std::string cardFile);
  void LoadSamples(std::vector<nuiskey> samplekeys);

  //! Main Likelihood evaluation FCN
  double DoEval(const double *x);

  //! Func Wrapper for ROOT
  inline double operator() (const std::vector<double> & x) {
    double* x_array = new double[x.size()];
    return this->DoEval(x_array);
  };

  //! Func Wrapper for ROOT
  inline double operator() (const double *x) {
    return this->DoEval(x);
  };
  
  //! Create a TTree to save all dial value iterations for this FCN
  void CreateIterationTree(std::string name, FitWeight* rw);

  //! Fills dial values and sample likelihoods into tree
  void FillIterationTree(FitWeight* rw);

  //! Writes TTree to fOutput directory
  void WriteIterationTree();

  //! Deletes TTree
  void DestroyIterationTree();

  //! Get Degrees of Freedom for samples (NBins)
  int GetNDOF();

  //! Return NDOF wrapper
  inline unsigned int NDim() {return this->GetNDOF();};
  
  //! Reconfigure samples where we force all events to be looped over.
  void ReconfigureAllEvents() ;

  //! Call Reconfigure on samples.
  //! Choice of reconfigure type depends on whether dials have changed
  //! and the MC has been filled.
  void ReconfigureSamples(bool fullconfig = false);

  //! Call reconfigure on only signal events (defaults to all events if CurIter=0)
  void ReconfigureSignal();

  //! Gets likelihood for all samples in FCN (assuming uncorrelated)
  double GetLikelihood();

  //! Returns list of pointers to the samples
  inline std::list<MeasurementBase*> GetSampleList(){ return fSamples; }

  //! Return list of pointers to all the pulls
  inline std::list<ParamPull*> GetPullList(){ return fPulls; };
  
  //! Write all samples to output DIR
  void Write();

  //! Set Fake data from file/MC
  void SetFakeData(std::string fakeinput);

  //! Reconfigure looping over duplicate inputs
  void ReconfigureUsingManager();

  //! Reconfigure Fast looping over duplicate inputs
  void ReconfigureFastUsingManager();
  
 private: 

  //! Append the experiments to include in the fit to this list
  std::list<MeasurementBase*> fSamples;

  //! Append parameter pull terms to include penalties in the fit to this list
  std::list<ParamPull*> fPulls;
  
  TDirectory *fOutputDir; //!< Directory to save contents

  std::string fCardFile; //!< Input Card text file

  bool fDialChanged;  //!< Flag for if RW dials changed
  UInt_t  fCurIter;   //!< Counter for how many times reconfigure called
  bool    fMCFilled;  //!< Check MC has at least been filled once

  TTree*  fIterationTree;  //!< Tree to save RW values on each function call
  int     fNDials;         //!< Number of RW Dials in FitWeight
  double* fDialVals;       //!< Current Values of RW Dials
  double  fLikelihood;     //!< Current likelihood for joint sample likelihood
  double  fNDOF;           //!< Total NDOF
  double* fSampleLikes;    //!< Likelihoods for each individual measurement in list
  int *   fSampleNDOF;     //!< NDOF for each individual measurement in list

  bool fUsingEventManager; //!< Flag for doing joint comparisons
  
};

/*! @} */
#endif // _JOINT_FCN_H_
