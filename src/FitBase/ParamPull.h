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

#ifndef PARAM_PULL_H_SEEN
#define PARAM_PULl_H_SEEN

/*!                                                                                                                                                                                                   
 *  \addtogroup FitBase                                                                                                                                                                               
 *  @{                                                                                                                                                                                                
 */

#include <stdlib.h>
#include <numeric>
#include <math.h>
#include <iostream>
#include <sstream>
#include <fstream>

// ROOT includes
#include <TROOT.h>
#include <TH1D.h>
#include <TTree.h>
#include <TFile.h>
#include <TMatrixDSym.h>
#include <TDecompSVD.h>
#include <TVectorD.h>

// Fit Includes
#include "PlotUtils.h"
#include "StatUtils.h"
#include "FitWeight.h"
#include "FitLogger.h"
#include "EventManager.h"

using namespace std;

//! Enums to allow Non Guassian Pulls in Future
enum FitPullTypes {
  kUnknownPull = -1,
  kNoPull = 0,
  kGausPull = 1
};

enum FitThrowTypes {
  kUnknownThrow = -1,
  kNoThrow = 0,
  kGausThrow = 1
};
  
//! Used to produce gaussian penalty terms in the fit.
class ParamPull {
    
 public:

  //! Default Constructor
  ParamPull(std::string name, std::string inputfile, std::string type);    
  
  //! Default destructor
  virtual ~ParamPull() {};

  // Set dial types (DEFAULT,ABS,FRAC)
  void SetType(std::string type);

  // Setup Histogram inputs (from previous fit file, or ROOT file)
  void SetupHistograms(std::string input);

  // Read a previous NUISANCE file
  void ReadFitFile(std::string input);

  // Read a ROOT file with any histograms in
  void ReadRootFile(std::string input);

  // Read Text file
  void ReadTextFile(std::string input);
  
  // Read a single dial central value and error
  void ReadDialInput(std::string input);
  
  //! Reconfigure function reads in current RW engine dials and sets their value to MC
  void Reconfigure();

  //! Get likelihood given the current values
  double GetLikelihood();

  //! Get NDOF if used in likelihoods
  int GetNDOF();
  
  //! Return the inverted covariance matrix
  TH2D GetCovar();
  inline TMatrixDSym* GetCovarMatrix(){ return fInvCovar; };
    
  //! Get the covariance matrix
  TH2D GetFullCovar();
  inline TMatrixDSym* GetFullCovarMatrix(){ return fCovar; };

  //! Get the decomp covar
  TH2D GetDecompCovar();
  inline TMatrixDSym* GetDecompCovarMatrix(){ return fDecomp; };

  //! Save the histograms
  void Write(std::string writeopt="");

  //! Throw the dial values using the current covariance. Useful for parameter throws.
  void ThrowCovariance();

  //! Compare dials to RW
  bool CheckDialsValid();

  //! Reset toy data back to original data
  inline void ResetToy(){ fDataHist = fDataTrue; };

  // Get Functions
  inline std::string GetName() const  { return fName; };
  inline std::string GetInput()      { return fInput; };
  inline std::string GetType()       { return fType;  };
  inline std::string GetFileType()   { return fFileType; };
  inline std::string GetDialOptions(){ return fDialOptions; };

  inline TH1D GetDataHist()  const { return *fDataHist;  };
  inline TH1D GetDataTrue()  const { return *fDataTrue;  };
  inline TH1D GetMCHist()    const { return *fMCHist;    };
  inline TH1D GetMaxHist()   const { return *fMaxHist;   };
  inline TH1D GetMinHist()   const { return *fMinHist;   };
  inline TH1I GetDialTypes() const { return *fDialTypes; };
    
 private:

  std::string fName;        //!< Pull Name
  std::string fInput;       //!< Pull input string
  std::string fType;        //!< Pull options type
  std::string fFileType;    //!< Pull input file types
  std::string fPlotTitles;  //! Axis format
  std::string fDialOptions; //!< Dial handling options
  
  TH1D* fMCHist;    //!< Current MC Histogram
  TH1D* fDataHist;  //!< Current data Histogram
  TH1D* fDataTrue;  //!< True Data (before histogram throws)
  TH1D* fMaxHist;   //!< Maximum limit on MC/Data
  TH1D* fMinHist;   //!< Maximum limit on MC/Data
  TH1I* fDialTypes; //!< Dial Types
  
  int fCalcType;   //!< Method to calculate likelihood
  int fThrowType;  //!< Method to calculate throws
    
  TMatrixDSym* fCovar;    //!< Covariance
  TMatrixDSym* fInvCovar; //!< Inverted Covariance
  TMatrixDSym* fDecomp;   //!< Decomposition
  
};

// Class TypeDefs
typedef std::list<ParamPull*>::const_iterator PullListConstIter;
typedef std::list<ParamPull*>::iterator PullListIter;
typedef std::vector<ParamPull*>::const_iterator PullVectConstIter;
typedef std::vector<ParamPull*>::iterator PullVectIter;

/*! @} */  
#endif
