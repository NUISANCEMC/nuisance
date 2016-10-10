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

#ifndef MEASUREMENT_1D_H_SEEN
#define MEASUREMENT_1D_H_SEEN

/*!
 *  \addtogroup FitBase
 *  @{
 */

#include <stdlib.h>
#include <numeric>
#include <math.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <deque>

// ROOT includes
#include <TROOT.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TArrayF.h>
#include <TGraph.h>
#include <TCut.h>
#include <TGraphErrors.h>
#include <TMatrixDSym.h>
#include <TDecompSVD.h>
#include <TDecompChol.h>
#include <TSystem.h>
#include <TCanvas.h>

// External data fit includes
#include "MeasurementBase.h"
#include "FitEvent.h"
#include "FitUtils.h"
#include "StatUtils.h"
#include "PlotUtils.h"
#include "FitParameters.h"

#include "SignalDef.h"
#include "InputHandler.h"

//********************************************************************
//! 1D Measurement base class. Histogram handling is done in this base layer.
class Measurement1D : public MeasurementBase {
//********************************************************************

 public:

  /*
    Constructor/Deconstuctor
  */

  //! Default Constructor
  Measurement1D(void);

  //! Default destructor
  virtual ~Measurement1D(void);

  //! Initialise values to NULL
  void Init(void);


  //! Setup the measurement with input values. This automatically handles parsing the fit type and data.
  virtual void SetupMeasurement(std::string input, std::string type,
				FitWeight *rw, std::string fkdt);

  //! Setup the default MC histograms from the currently set fDataHist.
  virtual void SetupDefaultHist (void);

  //! Set fit options by parsing type
  virtual void SetFitOptions (std::string opt);

  //! Setup the data values from a text file
  virtual void SetDataValues (std::string dataFile);

  //! Setup the data values by reading in a histogram from a TFile
  virtual void SetDataFromFile (std::string inhistfile,
				std::string histname);

  //! Setup the data by reading in a histogram from the database.
  virtual void SetDataFromDatabase (std::string inhistfile,
				    std::string histname);

  //! Read the covariance matrix from a root file (automatically grabs plot "covar")
  virtual void SetCovarMatrix (std::string covarFile);

  //! Read the covariance matrix from a text file given the covar size
  virtual void SetCovarMatrixFromText (std::string covarFile, int dim);

  //! Set the covariance from a custom root file
  virtual void SetCovarFromDataFile (std::string covarFile,
				     std::string covName);

  //! Set the smearing matrix from a text file given the size of the matrix
  virtual void SetSmearingMatrix (std::string smearfile,
				  int truedim, int recodim);

  //! Set the bin mask from a text file
  virtual void SetBinMask (std::string maskFile);

  //! Set the flux histogram from a ROOT file
  virtual void SetFluxHistogram (std::string fluxFile,
				int minE, int maxE, double fluxNorm);

  //! Get the total integrated flux between this samples energy range
  virtual double TotalIntegratedFlux (std::string intOpt="width",
				      double low=-9999.9, double high=-9999.9);

  
  //! Reset histograms to zero    
  virtual void ResetAll       (void);
  
  //! Fill histograms using fXVar,Weight    
  virtual void FillHistograms (void); 
  
  //! Scale to XSec Prediction  
  virtual void ScaleEvents    (void); 

  //! Apply normalisation scale after reconfigure
  virtual void ApplyNormScale (double norm);

  //! Apply smearing matrix to fMCHist
  virtual void ApplySmearingMatrix (void);


  //! Get the current Number of degrees of freedom accounting for bin masking.
  virtual int GetNDOF (void);

  //! Get Likelihood of iteration
  virtual double GetLikelihood (void);

  
  //! Set the fake data values from either a file, or MC using fakeOption="MC"
  virtual void SetFakeDataValues (std::string fakeOption); 

  //! Reset the fake data back to original fake data (Reset to before ThrowCovariance was called)
  virtual void ResetFakeData (void);

  //! Reset the fake data back to the true original dataset for this sample
  virtual void ResetData (void);

  //! Generate fake data by throwing the current fDataHist using the covariance.
  //! Can be used on fake MC data or just the original dataset.
  virtual void ThrowCovariance (void);

  // Get MC Histogram Stack
  virtual THStack GetModeStack (void);



  /// Get Histogram Functions
  inline TH1D* GetMCHistogram   (void) { return fMCHist;   };
  inline TH1D* GetDataHistogram (void) { return fDataHist; };

  virtual std::vector<TH1*> GetMCList   (void);
  virtual std::vector<TH1*> GetDataList (void);

  inline virtual std::vector<TH1*> GetMaskList (void) {
    return std::vector<TH1*> (1, fMaskHist);
  };

  inline virtual std::vector<TH1*> GetFineList (void) {
    return std::vector<TH1*> (1, fMCFine);
  };

  //! Get the bin contents and errors from fMCHist
  virtual void GetBinContents(std::vector<double>& cont,
			      std::vector<double>& err);

  //! Get the covariance matrix as a pretty plot
  inline virtual TH2D GetCovarMatrix (void) {
    return TH2D(*covar);
  };

  //! Return the integrated XSec for this sample, options define whether data or MC is returned.
  virtual std::vector<double> GetXSec(std::string opt);


  
  //! Save the current state to the current TFile directory
  virtual void Write(std::string drawOpt);

  //! array of histograms to handle fMCHist for each interaction channel.
  //TODO (P.Stowell) Figure out why I put mcHist as unprotected! :S
  TH1D* fMCHist_PDG[61];

protected:

  // data histograms
  TH1D* fDataHist; //!< default data histogram
  TH1D* fDataOrig; //!< histogram to store original data before throws.
  TH1D* fDataTrue; //!< histogram to store true dataset

  // Fake Data Flag
  bool fIsFakeData; //!< Flag: whether the current data is actually fake from MC
  std::string fakeDataFile; //!< Input fake data file

  // The histogram into which the measurement will be filled
  TH1D* fMCHist; //!< default MC Histogram used in the chi2 fits
  TH1D* fMCFine; //!< finely binned MC histogram
  TH1D* fMCStat; //!< histogram with unweighted events to properly calculate statistical error on MC
  TH1D* fMCWeighted; //!< Weighted histogram before xsec scaling
  
  TH1I* fMaskHist; //!< Mask histogram for neglecting specific bins

  std::string fPlotTitles; //!< Plot title x and y for the histograms

  // The covariance matrix and its fDecomposition
  TMatrixDSym *covar;      //!< Inverted Covariance
  TMatrixDSym *fFullCovar;  //!< Full Covariance
  TMatrixDSym *fDecomp;     //!< Decomposed Covariance
  TMatrixDSym *fCorrel;     //!< Correlation Matrix
  TMatrixD *fSmearMatrix; //!< Smearing matrix (note, this is not symmetric, and also in general not square)
  double fCovDet;     //!< Determinant of the covariance
  double fNormError;  //!< Sample norm error
  std::string fFitType;

  // Arrays for data entries
  Double_t* fXBins;  //!< xBin edges
  Double_t* fDataValues; //!< data bin contents
  Double_t* fDataErrors; //!< data bin errors
  Int_t fNDataPointsX; //!< number of data points

  // Fit specific flags
  bool fIsShape; //!< Flag: Perform Shape-only fit
  bool fIsFree; //!< Flag: Perform normalisation free fit
  bool fIsDiag; //!< Flag: only include uncorrelated diagonal errors
  bool fIsMask; //!< Flag: Apply bin masking
  bool fIsRawEvents; //!< Flag: Are bin contents just event rates
  bool fIsEnu1D; //!< Flag: Perform Flux Unfolded Scaling
  bool fIsChi2SVD; //!< Flag: Use alternative Chi2 SVD Method (Do not use)
  bool fAddNormPen; //!< Flag: Add a normalisation penalty term to the chi2.
  bool fIsFix; //!< Flag for keeping norm fixed
  bool fIsFull; //!< Flag for using full covariaince
  bool fIsDifXSec; //!< Flag for creating a dif xsec
  bool fIsChi2; //!< Flag for using Chi2 over LL methods

  std::string fAllowedTypes; //!< Fit Types Possible
  std::string fDefaultTypes; //!< Starting Default Fit Types

};

/*! @} */
#endif
