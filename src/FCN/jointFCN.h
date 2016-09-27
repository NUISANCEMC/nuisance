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
#include "parameter_pulls.h"
#include "SampleList.h"

#define GetCurrentDir getcwd
#include "EventManager.h"
#include "Math/Functor.h"

using namespace FitUtils;

//! Main FCN Class which ROOT's joint function needs to evaulate the chi2 at each stage of the fit.
class jointFCN 
{
  
 private: 
    
  // Reweighting Engine
  FitWeight *f_rw;

  TFile *out;
  std::string card;
  std::string fakeData;
  std::string drawingOptions;

  TMatrixDSym* FullCovar;

  bool saveHists;
  bool useFullCovar;

  // Append the experiments to include in the fit to this list
  std::list<MeasurementBase*> fChain;
  
  void LoadSamples(std::string cardFile);
 public:
  
  
  jointFCN(std::string cardfile, TFile *outfile);
  ~jointFCN();

  void SetOutName(std::string name){this->outname = name;}
  void Write();

  double operator() (const std::vector<double> & x) const{
    
    double* x_array = new double[x.size()];
    return this->DoEval(x_array);
  };
  
   
  double operator() (const double *x) const{
    return this->DoEval(x);
  };
  
  void CreateIterationTree(std::string name, FitWeight* rw);
  void WriteIterationTree();
  void DestroyIterationTree();
  void FillIterationTree(FitWeight* rw) const;
  unsigned int NDim() {return this->GetNDOF();};
  
  
  /* ROOT::Math::IBaseFunctionMultiDim* Clone(){return this;}; */
  double Up() const { return 1.; }
  
  double DoEval(const double *x) const;
  int GetNDOF();
  void ReconfigureAllEvents() const;
  void ReconfigureSamples(bool fullconfig = false) const;
  
  double GetLikelihood() const;
  void CreateFullCovarMatrix();

  void SetFakeData(std::string fakeOpt);
  TH1D* GetXSecPlot(std::string type);
  void ThrowSamples();

  std::list<MeasurementBase*> GetSampleList();
  
  std::string outname;
  std::vector<std::string> sampleNames;
  std::vector<std::string> sampleTypes;
  std::vector<std::string> sampleFiles;
  
  std::vector<std::string>         parameterNames;
  mutable std::vector<double> parameterValues;
  mutable std::vector<double> sampleNorms;
  mutable std::vector<double> currentFitVals;
  mutable bool dialChanged;


  void ScanSignalEvents(std::string outputName="");
  void ReconfigureSignal();
  void SetSignalTree(TFile* varFile);

  mutable bool SerialEval;
  TRandom3* randGen;

  mutable UInt_t current_iteration;
  mutable  bool filledMC;
  TFile* outputfile;

  mutable int ndials;
  mutable double* dialvals;
  mutable TTree* iteration_tree;
  mutable double likelihood;
};

class helperFunc{
 public:
  jointFCN* fFCN;
  
  helperFunc(jointFCN* f){fFCN = f;};
  ~helperFunc(){};

  double Up() const { return 1.; }
  double DoEval(const double *x) const {
    return fFCN->DoEval(x);
  };

  double operator() (const std::vector<double> & x) const{
    double* x_array = new double[x.size()];
    return this->DoEval(x_array);
  };

  double operator() (const double *x) const{
    return this->DoEval(x);
  };
  
  
};

/*! @} */
#endif // _FIT_FCN_H_
