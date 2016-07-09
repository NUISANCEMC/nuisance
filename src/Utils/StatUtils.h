// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NuFiX.
*
*    NuFiX is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NuFiX is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#ifndef STATUTILS_H
#define STATUTILS_H

// C Includes
#include <stdlib.h>
#include <numeric>
#include <math.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <deque>


// Root Includes
#include "TH1D.h"
#include "TH2I.h"
#include "TH2D.h"
#include "TFile.h"
#include "TMatrixDSym.h"
#include "TDecompSVD.h"
#include "TMath.h"
#include "TRandom3.h"
#include "TDecompChol.h"
#include "TGraphErrors.h"


// Fit Includes
#include "FitParameters.h"
#include "FitLogger.h"

/*!      
 *  \addtogroup Utils      
 *  @{ 
 */

//! Functions for handling statistics calculations
namespace StatUtils{

  /*
    Chi2 Functions
  */

  //! Get Chi2 using diagonal bin errors from the histogram. Masking applied before calculation if mask provided.
  Double_t GetChi2FromDiag(TH1D* data, TH1D* mc, TH1I* mask=NULL);

  //! Get Chi2 using diagonal bin errors from the histogram. 
  //! Plots converted to 1D histograms before using 1D calculation.
  Double_t GetChi2FromDiag(TH2D* data, TH2D* mc, TH2I* map=NULL, TH2I* mask=NULL);

  //! Get Chi2 using an inverted covariance for the data
  Double_t GetChi2FromCov( TH1D* data, TH1D* mc, TMatrixDSym* invcov, TH1I* mask=NULL);

  //! Get Chi2 using an inverted covariance for the data  
  //! Plots converted to 1D histograms before using 1D calculation.    
  Double_t GetChi2FromCov( TH2D* data, TH2D* mc, TMatrixDSym* invcov, TH2I* map=NULL, TH2I* mask=NULL);

  //! Get Chi2 using an SVD method on the covariance before calculation.
  //! Method suggested by Rex at MiniBooNE. Shown that it doesn't actually work. 
  Double_t GetChi2FromSVD( TH1D* data, TH1D* mc, TMatrixDSym* cov,    TH1I* mask=NULL);

  //! Get Chi2 using an SVD method on the covariance before calculation.
  //! Method suggested by Rex at MiniBooNE. Shown that it doesn't actually work.    
  //! Plots converted to 1D histograms before using 1D calculation.    
  Double_t GetChi2FromSVD( TH2D* data, TH2D* mc, TMatrixDSym* cov,    TH2I* map=NULL, TH2I* mask=NULL);


  //! Get Chi2 using only the raw event rates given in each bin using a -2LL method.
  Double_t GetChi2FromEventRate(TH1D* data, TH1D* mc, TH1I* mask=NULL);

  //! Get Chi2 using only the raw event rates given in each bin using a -2LL method.
  //! Plots converted to 1D histograms before using 1D calculation.    
  Double_t GetChi2FromEventRate(TH2D* data, TH2D* mc, TH2I* map=NULL, TH2I* mask=NULL);

  // Likelihood Functions
  
  //! Placeholder for 1D binned likelihood method
  Double_t GetLikelihoodFromDiag(TH1D* data, TH1D* mc, TH1I* mask=NULL);
  //! Placeholder for 2D binned likelihood method     
  Double_t GetLikelihoodFromDiag(TH2D* data, TH2D* mc, TH2I* map=NULL, TH2I* mask=NULL);

  //! Placeholder for 1D binned likelihood method     
  Double_t GetLikelihoodFromCov( TH1D* data, TH1D* mc, TMatrixDSym* invcov, TH1I* mask=NULL);
  //! Placeholder for 2D binned likelihood method     
  Double_t GetLikelihoodFromCov( TH2D* data, TH2D* mc, TMatrixDSym* invcov, TH2I* map=NULL, TH2I* mask=NULL);

  //! Placeholder for 1D binned likelihood method     
  Double_t GetLikelihoodFromSVD( TH1D* data, TH1D* mc, TMatrixDSym* cov,    TH1I* mask=NULL);
  //! Placeholder for 2D binned likelihood method     
  Double_t GetLikelihoodFromSVD( TH2D* data, TH2D* mc, TMatrixDSym* cov,    TH2I* map=NULL, TH2I* mask=NULL);

  //! Placeholder for 1D binned likelihood method     
  Double_t GetLikelihoodFromEventRate(TH1D* data, TH1D* mc, TH1I* mask=NULL);
  //! Placeholder for 2D binned likelihood method     
  Double_t GetLikelihoodFromEventRate(TH2D* data, TH2D* mc, TH2I* map=NULL, TH2I* mask=NULL);

  /* 
     NDOF Functions
  */
  
  //! Return 1D Histogram NDOF considering masking and empty bins
  Int_t GetNDOF(TH1D* hist, TH1I* mask=NULL);

  //! Return 2D Histogram NDOF considering masking and empty bins            
  Int_t GetNDOF(TH2D* hist, TH2I* map=NULL, TH2I* mask=NULL);

  /*
    Fake Data Functions
  */
  
  //! Given a full covariance for a 1D data set throw the decomposition to generate fake data.
  //! throwdiag determines whether diagonal statistical errors are thrown.
  //! If no covariance is provided only statistical errors are thrown.
  TH1D* ThrowHistogram(TH1D* hist, TMatrixDSym* cov, bool throwdiag=true, TH1I* mask=NULL);

  //! Given a full covariance for a 2D data set throw the decomposition to generate fake data.
  //! Plots are converted to 1D histograms and the 1D ThrowHistogram is used, before being converted back to 2D histograms.
  TH2D* ThrowHistogram(TH2D* hist, TMatrixDSym* cov, TH2I* map=NULL, bool throwdiag=true, TH2I* mask=NULL);

  
  /*
    Masking Functions
  */

  //! Given a mask histogram, mask out any bins in hist with non zero entries in mask.
  TH1D* ApplyHistogramMasking(TH1D* hist, TH1I* mask);

  //! Given a mask histogram, mask out any bins in hist with non zero entries in mask. 
  TH2D* ApplyHistogramMasking(TH2D* hist, TH2I* mask);

  //! Given a mask histogram apply the masking procedure to each of the rows/columns in a covariance, before recalculating its inverse.
  TMatrixDSym* ApplyInvertedMatrixMasking(TMatrixDSym* mat, TH1I* mask);

  //! Given a mask histogram apply the masking procedure to each of the rows/columns in a covariance, before recalculating its inverse.
  //! Converts to 1D data before using the 1D ApplyInvertedMatrixMasking function and converting back to 2D.
  TMatrixDSym* ApplyInvertedMatrixMasking(TMatrixDSym* mat, TH2D* data, TH2I* mask, TH2I* map=NULL);

  //! Given a mask histogram apply the masking procedure to each of the rows/columns in a covariance
  TMatrixDSym* ApplyMatrixMasking(TMatrixDSym* mat, TH1I* mask);

  //! Given a mask histogram apply the masking procedure to each of the rows/columns in a covariance
  //! Converts to 1D data before using the 1D ApplyInvertedMatrixMasking function and converting back to 2D.
  TMatrixDSym* ApplyMatrixMasking(TMatrixDSym* mat, TH2D* data, TH2I* mask, TH2I* map=NULL);

  /*
    Covariance Handling Functions
  */

  //! Return inverted matrix of TMatrixDSym
  TMatrixDSym* GetInvert(TMatrixDSym* mat);

  //! Return Cholesky Decomposed matrix of TMatrixDSym
  TMatrixDSym* GetDecomp(TMatrixDSym* mat);


  //! Given a normalisation factor for a dataset add in a new normalisation term to the covariance.
  void ForceNormIntoCovar(TMatrixDSym* mat, TH1D* data, double norm);

  //! Given a normalisation factor for a dataset add in a new normalisation term to the covariance.
  //! Convertes 2D to 1D, before using 1D ForceNormIntoCovar
  void ForceNormIntoCovar(TMatrixDSym* mat, TH2D* data, double norm, TH2I* map=NULL);

  //! Given a dataset generate an uncorrelated covariance matrix using the bin errors.
  TMatrixDSym* MakeDiagonalCovarMatrix(TH1D* data);

  //! Given a dataset generate an uncorrelated covariance matrix using the bin errors.  
  TMatrixDSym* MakeDiagonalCovarMatrix(TH2D* data, TH2I* map=NULL);

  //! Given a covariance set the errors in each bin on the data from the covariance diagonals.
  void SetDataErrorFromCov(TH1D* data, TMatrixDSym* cov, double scale=1.0);

  //! Given a covariance set the errors in each bin on the data from the covariance diagonals.
  void SetDataErrorFromCov(TH2D* data, TMatrixDSym* cov, TH2I* map=NULL, double scale=1.0);
  

  /*
    Mapping Functions
  */
  
  //! If no map is provided for the 2D histogram, generate one by counting up through the bins along x and y.
  TH2I* GenerateMap(TH2D* hist);

  //! Apply a map to a 2D histogram converting it into a 1D histogram.
  TH1D* MapToTH1D(TH2D* hist, TH2I* map);

  //! Apply a map to a 2D mask convering it into a 1D mask.
  TH1I* MapToMask(TH2I* hist, TH2I* map);

};

/*! @} */
#endif
