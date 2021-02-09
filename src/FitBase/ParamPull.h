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

#ifndef PARAM_PULL_H_SEEN
#define PARAM_PULL_H_SEEN

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
#include <list>
#include <vector>

// ROOT includes
#include <TROOT.h>
#include <TH1D.h>
#include <TTree.h>
#include <TFile.h>
#include <TMatrixDSym.h>
#include <TDecompSVD.h>
#include <TVectorD.h>
#include "TRandom3.h"

// Fit Includes
#include "PlotUtils.h"
#include "StatUtils.h"
#include "FitWeight.h"
#include "FitLogger.h"
#include "EventManager.h"
#include "TVector.h"

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
  kGausThrow = 1,
  kFlatThrow = 2
};
  
//! Used to produce gaussian penalty terms in the fit.
class ParamPull {
 public:

  //! Default Constructor
  ParamPull(std::string name, std::string inputfile, std::string type, std::string dials="");    
  
  //! Default destructor
  virtual ~ParamPull(void) {};

  // Set dial types (DEFAULT,ABS,FRAC)
  void SetType(std::string type);

  // Setup Histogram inputs (from previous fit file, or ROOT file)
  void SetupHistograms(std::string input);

  // Read a previous NUISANCE file
  void ReadFitFile(std::string input);

  // Read a ROOT file with any histograms in
  void ReadRootFile(std::string input);

  // Read Text file
  void ReadVectFile(std::string input);
  
  // Read a single dial central value and error
  void ReadDialInput(std::string input);
  
  //! Reconfigure function reads in current RW engine dials and sets their value to MC
  void Reconfigure(void);

  //! Get likelihood given the current values
  double GetLikelihood(void);

  //! Get NDOF if used in likelihoods
  int GetNDOF(void);
  
  // Get Covariance Matrices as plots
  TH2D GetCovar(void);
  TH2D GetFullCovar(void);
  TH2D GetDecompCovar(void);

  // Get Covariance Matrices
  inline TMatrixDSym GetCovarMatrix       (void) const { return *fInvCovar; };
  inline TMatrixDSym GetFullCovarMatrix   (void) const { return *fCovar;    };
  inline TMatrixDSym GetDecompCovarMatrix (void) const { return *fDecomp;   };

  //! Save the histograms
  void Write(std::string writeopt="");

  //! Throw the dial values using the current covariance. Useful for parameter throws.
  void ThrowCovariance(void);

  //! Compare dials to RW
  bool CheckDialsValid(void);

  //! Reset toy data back to original data
  void ResetToy(void);

  //! Read fake data from MC
  void SetFakeData(std::string fakeinput);

  //! Reset fake data back to original data (before fake data or throws)
  void RemoveFakeData();
  
  // Get Functions
  inline std::string GetName        (void) const { return fName;        };
  inline std::string GetInput       (void) const { return fInput;       };
  inline std::string GetType        (void) const { return fType;        };
  inline std::string GetFileType    (void) const { return fFileType;    };
  inline std::string GetDialOptions (void) const { return fDialOptions; };

  std::map<std::string, int> GetAllDials();
  
  inline TH1D GetDataHist  (void) const { return *fDataHist; };
  inline TH1D GetDataTrue  (void) const { return *fDataTrue; };
  inline TH1D GetDataOrig  (void) const { return *fDataOrig; };
  inline TH1D GetMCHist    (void) const { return *fMCHist;   };
  inline TH1D GetMaxHist   (void) const { return *fMaxHist;  };
  inline TH1D GetMinHist   (void) const { return *fMinHist;  };
  inline TH1I GetDialTypes (void) const { return *fTypeHist; };
  inline TH1D GetLimitHist  (void) const { return *fLimitHist; };


 private:

  void CheckHist(TH1D*);
  TH1D RemoveBinsNotInString(TH1D hist, std::string mystr);
  TH1I RemoveBinsNotInString(TH1I hist, std::string mystr);
  
  std::string fName;        //!< Pull Name
  std::string fInput;       //!< Pull input string
  std::string fType;        //!< Pull options type
  std::string fFileType;    //!< Pull input file types
  std::string fPlotTitles;  //! Axis format
  std::string fDialOptions; //!< Dial handling options
  std::string fDialSelection; //!< Dial Selection
  
  TH1D* fMCHist;    //!< Current MC Histogram
  TH1D* fDataHist;  //!< Current data Histogram
  TH1D* fDataTrue;  //!< True Data (before histogram throws)
  TH1D* fDataOrig;  //!< Orig Data (without toys or fake data)
  TH1D* fMaxHist;   //!< Maximum limit on MC/Data
  TH1D* fMinHist;   //!< Maximum limit on MC/Data
  TH1I* fTypeHist; //!< Dial Types
  
  int fCalcType;   //!< Method to calculate likelihood
  int fThrowType;  //!< Method to calculate throws
    
  TMatrixDSym* fCovar;    //!< Covariance
  TMatrixDSym* fInvCovar; //!< Inverted Covariance
  TMatrixDSym* fDecomp;   //!< Decomposition

  TH1D* fLimitHist;
  
};

// Class TypeDefs
typedef std::list<ParamPull*>::const_iterator PullListConstIter;
typedef std::list<ParamPull*>::iterator PullListIter;
typedef std::vector<ParamPull*>::const_iterator PullVectConstIter;
typedef std::vector<ParamPull*>::iterator PullVectIter;

/*! @} */  
#endif
