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

#ifndef STATUTILS_H
#define STATUTILS_H

// C Includes
#include "assert.h"
#include <deque>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <numeric>
#include <sstream>
#include <stdlib.h>
#include <string>

// Root Includes
#include "TDecompChol.h"
#include "TDecompSVD.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH2I.h"
#include "TMath.h"
#include "TMatrixDSym.h"
#include "TRandom3.h"

// Fit Includes

#include "FitLogger.h"

/*!
 *  \addtogroup Utils
 *  @{
 */

//! Functions for handling statistics calculations
namespace StatUtils {

/*
  Chi2 Functions
*/

//! Get Chi2 using diagonal bin errors from the histogram. Masking applied
//! before calculation if mask provided.
Double_t GetChi2FromDiag(TH1D *data, TH1D *mc, TH1I *mask = NULL);

//! Get Chi2 using diagonal bin errors from the histogram.
//! Plots converted to 1D histograms before using 1D calculation.
Double_t GetChi2FromDiag(TH2D *data, TH2D *mc, TH2I *map = NULL,
                         TH2I *mask = NULL);

//! Get Chi2 using an inverted covariance for the data
Double_t GetChi2FromCov(TH1D *data, TH1D *mc, TMatrixDSym *invcov,
                        TH1I *mask = NULL, double data_scale = 1,
                        double covar_scale = 1E76, TH1D *outchi2perbin = NULL);

//! Get Chi2 using an inverted covariance for the data
//! Plots converted to 1D histograms before using 1D calculation.
Double_t GetChi2FromCov(TH2D *data, TH2D *mc, TMatrixDSym *invcov,
                        TH2I *map = NULL, TH2I *mask = NULL,
                        TH2D *outchi2perbin = NULL);

//! Get Chi2 using an SVD method on the covariance before calculation.
//! Method suggested by Rex at MiniBooNE. Shown that it doesn't actually work.
Double_t GetChi2FromSVD(TH1D *data, TH1D *mc, TMatrixDSym *cov,
                        TH1I *mask = NULL);

//! Get Chi2 using an SVD method on the covariance before calculation.
//! Method suggested by Rex at MiniBooNE. Shown that it doesn't actually work.
//! Plots converted to 1D histograms before using 1D calculation.
Double_t GetChi2FromSVD(TH2D *data, TH2D *mc, TMatrixDSym *cov,
                        TH2I *map = NULL, TH2I *mask = NULL);

//! Get Chi2 using only the raw event rates given in each bin using a -2LL
//! method.
Double_t GetChi2FromEventRate(TH1D *data, TH1D *mc, TH1I *mask = NULL);

//! Get Chi2 using only the raw event rates given in each bin using a -2LL
//! method. Plots converted to 1D histograms before using 1D calculation.
Double_t GetChi2FromEventRate(TH2D *data, TH2D *mc, TH2I *map = NULL,
                              TH2I *mask = NULL);

// Likelihood Functions

//! Placeholder for 1D binned likelihood method
Double_t GetLikelihoodFromDiag(TH1D *data, TH1D *mc, TH1I *mask = NULL);
//! Placeholder for 2D binned likelihood method
Double_t GetLikelihoodFromDiag(TH2D *data, TH2D *mc, TH2I *map = NULL,
                               TH2I *mask = NULL);

//! Placeholder for 1D binned likelihood method
Double_t GetLikelihoodFromCov(TH1D *data, TH1D *mc, TMatrixDSym *invcov,
                              TH1I *mask = NULL);
//! Placeholder for 2D binned likelihood method
Double_t GetLikelihoodFromCov(TH2D *data, TH2D *mc, TMatrixDSym *invcov,
                              TH2I *map = NULL, TH2I *mask = NULL);

//! Placeholder for 1D binned likelihood method
Double_t GetLikelihoodFromSVD(TH1D *data, TH1D *mc, TMatrixDSym *cov,
                              TH1I *mask = NULL);
//! Placeholder for 2D binned likelihood method
Double_t GetLikelihoodFromSVD(TH2D *data, TH2D *mc, TMatrixDSym *cov,
                              TH2I *map = NULL, TH2I *mask = NULL);

//! Placeholder for 1D binned likelihood method
Double_t GetLikelihoodFromEventRate(TH1D *data, TH1D *mc, TH1I *mask = NULL);
//! Placeholder for 2D binned likelihood method
Double_t GetLikelihoodFromEventRate(TH2D *data, TH2D *mc, TH2I *map = NULL,
                                    TH2I *mask = NULL);

/*
   NDOF Functions
*/

//! Return 1D Histogram NDOF considering masking and empty bins
Int_t GetNDOF(TH1D *hist, TH1I *mask = NULL);

//! Return 2D Histogram NDOF considering masking and empty bins
Int_t GetNDOF(TH2D *hist, TH2I *map = NULL, TH2I *mask = NULL);

/*
  Fake Data Functions
*/

//! Given a full covariance for a 1D data set throw the decomposition to
//! generate fake data. throwdiag determines whether diagonal statistical errors
//! are thrown. If no covariance is provided only statistical errors are thrown.
TH1D *ThrowHistogram(TH1D *hist, TMatrixDSym *cov, bool throwdiag = true,
                     TH1I *mask = NULL);

//! Given a full covariance for a 2D data set throw the decomposition to
//! generate fake data. Plots are converted to 1D histograms and the 1D
//! ThrowHistogram is used, before being converted back to 2D histograms.
TH2D *ThrowHistogram(TH2D *hist, TMatrixDSym *cov, TH2I *map = NULL,
                     bool throwdiag = true, TH2I *mask = NULL);

/*
  Masking Functions
*/

//! Given a mask histogram, mask out any bins in hist with non zero entries in
//! mask.
TH1D *ApplyHistogramMasking(TH1D *hist, TH1I *mask);

//! Given a mask histogram, mask out any bins in hist with non zero entries in
//! mask.
TH2D *ApplyHistogramMasking(TH2D *hist, TH2I *mask);

//! Given a mask histogram apply the masking procedure to each of the
//! rows/columns in a covariance, before recalculating its inverse.
TMatrixDSym *ApplyInvertedMatrixMasking(TMatrixDSym *mat, TH1I *mask);

//! Given a mask histogram apply the masking procedure to each of the
//! rows/columns in a covariance, before recalculating its inverse. Converts to
//! 1D data before using the 1D ApplyInvertedMatrixMasking function and
//! converting back to 2D.
TMatrixDSym *ApplyInvertedMatrixMasking(TMatrixDSym *mat, TH2D *data,
                                        TH2I *mask, TH2I *map = NULL);

//! Given a mask histogram apply the masking procedure to each of the
//! rows/columns in a covariance
TMatrixDSym *ApplyMatrixMasking(TMatrixDSym *mat, TH1I *mask);

//! Given a mask histogram apply the masking procedure to each of the
//! rows/columns in a covariance Converts to 1D data before using the 1D
//! ApplyInvertedMatrixMasking function and converting back to 2D.
TMatrixDSym *ApplyMatrixMasking(TMatrixDSym *mat, TH2D *data, TH2I *mask,
                                TH2I *map = NULL);

/*
  Covariance Handling Functions
*/

//! Return inverted matrix of TMatrixDSym
TMatrixDSym *GetInvert(TMatrixDSym *mat);

//! Return Cholesky Decomposed matrix of TMatrixDSym
TMatrixDSym *GetDecomp(TMatrixDSym *mat);

//! Return full covariances
TMatrixDSym *GetCovarFromCorrel(TMatrixDSym *correl, TH1D *data);

//! Given a normalisation factor for a dataset add in a new normalisation term
//! to the covariance.
void ForceNormIntoCovar(TMatrixDSym *&mat, TH1D *data, double norm);

//! Given a normalisation factor for a dataset add in a new normalisation term
//! to the covariance. Convertes 2D to 1D, before using 1D ForceNormIntoCovar
void ForceNormIntoCovar(TMatrixDSym *mat, TH2D *data, double norm,
                        TH2I *map = NULL);

//! Given a dataset generate an uncorrelated covariance matrix using the bin
//! errors.
TMatrixDSym *MakeDiagonalCovarMatrix(TH1D *data, double scaleF = 1E38);

//! Given a dataset generate an uncorrelated covariance matrix using the bin
//! errors.
TMatrixDSym *MakeDiagonalCovarMatrix(TH2D *data, TH2I *map = NULL,
                                     double scaleF = 1E38);

//! Given a covariance set the errors in each bin on the data from the
//! covariance diagonals.
void SetDataErrorFromCov(TH1D *data, TMatrixDSym *cov, double scale = 1.0,
                         bool ErrorCheck = true);

//! Given a covariance set the errors in each bin on the data from the
//! covariance diagonals.
void SetDataErrorFromCov(TH2D *data, TMatrixDSym *cov, TH2I *map = NULL,
                         double scale = 1.0, bool ErrorCheck = true);

//! Given a covariance, extracts the shape-only matrix using the method from the
//! MiniBooNE TN
TMatrixDSym *ExtractShapeOnlyCovar(TMatrixDSym *full_covar, TH1D *data_hist,
                                   double data_scale = 1E38);
TMatrixDSym *ExtractShapeOnlyCovar(TMatrixDSym *full_covar, TH2D *data_hist,
                                   TH2I *map = NULL, double data_scale = 1E38);
/*
  Mapping Functions
*/

//! If no map is provided for the 2D histogram, generate one by counting up
//! through the bins along x and y.
TH2I *GenerateMap(TH2D *hist);

//! Apply a map to a 2D histogram converting it into a 1D histogram.
TH1D *MapToTH1D(TH2D *hist, TH2I *map);

//! Apply a map to fill a TH2D from a TH1D;
void MapFromTH1D(TH2 *fillhist, TH1 *fromhist, TH2I *map);

//! Apply a map to a 2D mask convering it into a 1D mask.
TH1I *MapToMask(TH2I *hist, TH2I *map);

/// \brief Read TMatrixD from a text file
///
/// - covfile = full path to text file
/// - dimx = x dimensions of matrix
/// - dimy = y dimensions of matrix
///
/// Format of textfile should be: \n
/// cov_11  cov_12 ...  cov_1N \n
/// cov_21  cov_22 ...  cov_2N \n
/// ...     ...    ...  ...    \n
/// cov_N1  ...    ...  cov_NN \n
///
/// If no dimensions are given, dimx and dimy are determined from rows/columns
/// inside textfile.
///
/// If only dimx is given a symmetric matrix is assumed.
TMatrixD *GetMatrixFromTextFile(std::string covfile, int dimx = -1,
                                int dimy = -1);

/// \brief Read TMatrixD from a ROOT file
///
/// - covfile = full path to root file (+';histogram')
/// - histname = histogram name
///
/// If no histogram name is given function assumes it has been appended
/// covfile path as: \n
/// 'covfile.root;histname'
///
/// histname can point to a TMatrixD object, a TMatrixDSym object, or
/// a TH2D object.
TMatrixD *GetMatrixFromRootFile(std::string covfile, std::string histname = "");

/// \brief Calls GetMatrixFromTextFile and turns it into a TMatrixDSym
TMatrixDSym *GetCovarFromTextFile(std::string covfile, int dim);

/// \brief Calls GetMatrixFromRootFile and turns it into a TMatrixDSym
TMatrixDSym *GetCovarFromRootFile(std::string covfile, std::string histname);

}; // namespace StatUtils

/*! @} */
#endif
