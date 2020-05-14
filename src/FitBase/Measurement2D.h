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

#ifndef MEASUREMENT_2D_HXX_SEEN
#define MEASUREMENT_2D_HXX_SEEN

/*!
 *  \addtogroup FitBase
 *  @{
 */

#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <numeric>
#include <sstream>
#include <stdlib.h>
#include <string>

// ROOT includes
#include <TArrayF.h>
#include <TDecompSVD.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TMatrixDSym.h>
#include <TROOT.h>
#include <TSystem.h>

// External data fit includes
#include "FitEvent.h"

#include "FitUtils.h"
#include "MeasurementBase.h"
#include "MeasurementVariableBox2D.h"
#include "PlotUtils.h"
#include "SignalDef.h"
#include "StatUtils.h"

//********************************************************************
//! 2D Measurement base class. Histogram handling is done in this base layer.
class Measurement2D : public MeasurementBase {
  //********************************************************************

public:
  /*
    Constructor/Deconstuctor
  */

  //! Default Constructor
  Measurement2D();

  //! Default Destructor
  virtual ~Measurement2D();

  /*
    Setup Functions
  */

  /// \brief Setup all configs once initialised
  ///
  /// Should be called after all configs have been setup inside fSettings
  /// container. Handles the processing of inputs and setting up of types.
  /// Replaces the old 'SetupMeasurement' function.
  void FinaliseSampleSettings();

  /// \brief Creates the 2D data distribution given the binning provided.
  virtual void CreateDataHistogram(int dimx, double *binx, int dimy,
                                   double *biny);

  /// \brief Set Data Histogram from a list of contents in a text file
  ///
  /// Assumes the format: \n
  /// x_low_1  y_low_1  cont_11  err_11 \n
  /// x_low_1  y_low_2  cont_12  err_12 \n
  /// x_low_2  y_low_1  cont_21  err_21 \n
  /// x_low_2  y_low_2  cont_22  err_22 \n
  /// x_low_2  y_low_3  cont_23  err_23 \n
  /// x_low_3  y_low_2  cont_32  err_32 \n
  virtual void SetDataFromTextFile(std::string data, std::string binx,
                                   std::string biny);

  /// \brief Set Data Histogram from a TH2D in a file
  ///
  /// - datfile = Full path to data file
  /// - histname = Name of histogram
  ///
  /// If histname not given it assumes that datfile
  /// is in the format: \n
  /// 'file.root;histname'
  virtual void SetDataFromRootFile(std::string datfile,
                                   std::string histname = "");

  /// \brief Set data values from a 2D array in text file
  ///
  /// \warning requires DATA HISTOGRAM TO BE SET FIRST
  ///
  /// Assumes form: \n
  /// cont_11 cont_12 ... cont_1N \n
  /// cont_21 cont_22 ... cont_2N \n
  /// ...     ...     ... ...     \n
  /// cont_N1 cont_N2 ... cont_NN \n
  virtual void SetDataValuesFromTextFile(std::string datfile,
                                         TH2D *hist = NULL);

  /// \brief Set data errors from a 2D array in text file
  ///
  /// \warning requires DATA HISTOGRAM TO BE SET FIRST
  ///
  /// Assumes form: \n
  /// errs_11 errs_12 ... errs_1N \n
  /// errs_21 errs_22 ... errs_2N \n
  /// ...     ...     ... ...     \n
  /// errs_N1 errs_N2 ... errs_NN \n
  virtual void SetDataErrorsFromTextFile(std::string datfile,
                                         TH2D *hist = NULL);

  /// \brief Set data bin errors to sqrt(entries)
  ///
  /// \warning REQUIRES DATA HISTOGRAM TO BE SET FIRST
  ///
  /// Sets the data errors as the sqrt of the bin contents
  /// Should be use for counting experiments
  virtual void SetPoissonErrors();

  /// \brief Make diagonal covariance from data
  ///
  /// \warning If no histogram passed, data must be setup first!
  /// Setup the covariance inputs by taking the data histogram
  /// errors and setting up a diagonal covariance matrix.
  ///
  /// If no data is supplied, fDataHist is used if already set.
  virtual void SetCovarFromDiagonal(TH2D *data = NULL);

  /// \brief Read the data covariance from a text file.
  ///
  /// Inputfile should have the format: \n
  /// covariance_11  covariance_12  covariance_13 ... \n
  /// covariance_21  covariance_22  covariance_23 ... \n
  /// ...            ...            ...           ... \n
  ///
  /// If no dimensions are given, it is assumed from the number
  /// entries in the first line of covfile.
  virtual void SetCovarFromTextFile(std::string covfile, int dim = -1);

  /// \brief Read the data covariance from a ROOT file.
  ///
  /// - covfile specifies the full path to the file
  /// - histname specifies the name of the covariance object. Both TMatrixDSym
  /// and TH2D are supported.
  ///
  /// If no histogram name is given the inhistfile value
  /// is automatically parsed with ; so that: \n
  /// mycovfile.root;myhistname \n
  /// will also work.
  virtual void SetCovarFromRootFile(std::string covfile,
                                    std::string histname = "");

  /// \brief Read the inverted data covariance from a text file.
  ///
  /// Inputfile should have similar format to that shown
  /// in SetCovarFromTextFile.
  ///
  /// If no dimensions are given, it is assumed from the number
  /// entries in the first line of covfile.
  virtual void SetCovarInvertFromTextFile(std::string covfile, int dim = -1);

  /// \brief Read the inverted data covariance from a ROOT file.
  ///
  /// Inputfile should have similar format to that shown
  /// in SetCovarFromRootFile.
  ///
  /// If no histogram name is given the inhistfile value
  /// is automatically parsed with ; so that: \n
  /// mycovfile.root;myhistname \n
  /// will also work.
  virtual void SetCovarInvertFromRootFile(std::string covfile,
                                          std::string histname = "");

  /// \brief Read the data correlations from a text file.
  ///
  /// \warning REQUIRES DATA HISTOGRAM TO BE SET FIRST
  ///
  /// Inputfile should have similar format to that shown
  /// in SetCovarFromTextFile.
  ///
  /// If no dimensions are given, it is assumed from the number
  /// entries in the first line of covfile.
  virtual void SetCorrelationFromTextFile(std::string covfile, int dim = -1);

  /// \brief Read the data correlations from a ROOT file.
  ///
  /// \warning REQUIRES DATA TO BE SET FIRST
  ///
  /// Inputfile should have similar format to that shown
  /// in SetCovarFromRootFile.
  ///
  /// If no histogram name is given the inhistfile value
  /// is automatically parsed with ; so that: \n
  /// mycovfile.root;myhistname \n
  /// will also work.
  virtual void SetCorrelationFromRootFile(std::string covfile,
                                          std::string histname = "");

  /// \brief Read the cholesky decomposed covariance from a text file and turn
  /// it into a covariance
  ///
  /// Inputfile should have similar format to that shown
  /// in SetCovarFromTextFile.
  ///
  /// If no dimensions are given, it is assumed from the number
  /// entries in the first line of covfile.
  virtual void SetCholDecompFromTextFile(std::string covfile, int dim = -1);

  /// \brief Read the cholesky decomposed covariance from a ROOT file and turn
  /// it into a covariance
  ///
  /// Inputfile should have similar format to that shown
  /// in SetCovarFromRootFile.
  ///
  /// If no histogram name is given the inhistfile value
  /// is automatically parsed with ; so that: \n
  /// mycovfile.root;myhistname \n
  /// will also work.
  virtual void SetCholDecompFromRootFile(std::string covfile,
                                         std::string histname = "");

  void SetShapeCovar();

  /// \brief Read the map values from a text file
  ///
  /// \warning Requires DATA hist to be set beforehand.
  /// Format should be a 2D array of mappings.
  /// -1 indicates empty bins. \n
  /// e.g.: \n
  ///  1  2  3  4  5 \n
  /// -1  6  7  8  9 \n
  /// -1 -1 10 11 -1 \n
  virtual void SetMapValuesFromText(std::string dataFile);

  /// \brief Scale the data by some scale factor
  virtual void ScaleData(double scale);

  /// \brief Scale the data error bars by some scale factor
  virtual void ScaleDataErrors(double scale);

  /// \brief Scale the covariaince and its invert/decomp by some scale factor.
  virtual void ScaleCovar(double scale);

  /// \brief Setup a bin masking histogram and apply masking to data
  ///
  /// \warning REQUIRES DATA HISTOGRAM TO BE SET FIRST
  ///
  /// Reads in a list of bins in a text file to be masked. Format is: \n
  /// bin_index_x_1 bin_index_y_1 1 \n
  /// bin_index_x_2 bin_index_y_2 1 \n
  /// bin_index_x_3 bin_index_y_3 1 \n
  ///
  /// If 0 is given then a bin entry will NOT be masked. So for example: \n\n
  /// 1 1  1 \n
  /// 2 0 1 \n
  /// 3 4 0 \n
  /// 4 0 1 \n\n
  /// Will mask only the (1,1), (2,0), and (4,0) bins.
  ///
  /// Masking can be turned on by specifiying the MASK option when creating a
  /// sample. When this is passed NUISANCE will look in the following locations
  /// for the mask file:
  /// - FitPar::Config().GetParS(fName + ".mask")
  /// - "data/masks/" + fName + ".mask";
  virtual void SetBinMask(std::string maskfile);

  /// \brief Set the current fit options from a string.
  ///
  /// This is called twice for each sample, once to set the default
  /// and once to set the current setting (if anything other than default given)
  ///
  /// For this to work properly it requires the default and allowed types to be
  /// set correctly. These should be specified as a string listing options.
  ///
  /// To split up options so that NUISANCE can automatically detect ones that
  /// are conflicting. Any options seperated with the '/' symbol are non
  /// conflicting and can be given together, whereas any seperated with the ','
  /// symbol cannot be specified by the end user at the same time.
  ///
  /// Default Type Examples:
  /// - DIAG/FIX = Default option will be a diagonal covariance, with FIXED
  /// norm.
  /// - MASK/SHAPE = Default option will be a masked hist, with SHAPE always on.
  ///
  /// Allowed Type examples:
  /// - 'FULL/DIAG/NORM/MASK' = Any of these options can be specified.
  /// - 'FULL,FREE,SHAPE/MASK/NORM' = User can give either FULL, FREE, or SHAPE
  /// as on option. MASK and NORM can also be included as options.
  virtual void SetFitOptions(std::string opt);

  /// \brief Final constructor setup
  /// \warning Should be called right at the end of the constructor.
  ///
  /// Contains a series of checks to ensure the data and inputs have been setup.
  /// Also creates the MC histograms needed for fitting.
  void FinaliseMeasurement();

  /*
    Reconfigure
  */

  /// \brief Create a Measurement1D box
  ///
  /// Creates a new 1D variable box containing just fXVar.
  ///
  /// This box is the bare minimum required by the JointFCN when
  /// running fast reconfigures during a routine.
  /// If for some reason a sample needs extra variables to be saved then
  /// it should override this function creating its own MeasurementVariableBox
  /// that contains the extra variables.
  virtual MeasurementVariableBox *CreateBox() {
    return new MeasurementVariableBox2D();
  };

  /// \brief Reset all MC histograms
  ///
  /// Resets all standard histograms and those registered to auto
  /// process to zero.
  ///
  /// If extra histograms are not included in auto processing, then they must be
  /// reset by overriding this function and doing it manually if required.
  virtual void ResetAll(void);

  /// \brief Fill MC Histograms from XVar, YVar
  ///
  /// Fill standard histograms using fXVar, fYVar, Weight read from the variable
  /// box.
  ///
  /// WARNING : Any extra MC histograms need to be filled by overriding this
  /// function, even if they have been set to auto process.
  virtual void FillHistograms(void);

  // \brief Convert event rates to final histogram
  ///
  /// Apply standard scaling procedure to standard mc histograms to convert from
  /// raw events to xsec prediction.
  ///
  /// If any distributions have been set to auto process
  /// that is done during this function call, and a differential xsec is
  /// assumed. If that is not the case this function must be overriden.
  virtual void ScaleEvents(void);

  /// \brief Scale MC by a factor=1/norm
  ///
  /// Apply a simple normalisation scaling if the option FREE or a
  /// norm_parameter has been specified in the NUISANCE routine.
  virtual void ApplyNormScale(double norm);

  /*
    Statistical Functions
  */

  /// \brief Get Number of degrees of freedom
  ///
  /// Returns the number bins inside the data histogram accounting for
  /// any bin masking applied.
  virtual int GetNDOF();

  /// \brief Return Data/MC Likelihood at current state
  ///
  /// Returns the likelihood of the data given the current MC prediction.
  /// Diferent likelihoods definitions are used depending on the FitOptions.
  virtual double GetLikelihood(void);

  /*
    Fake Data
  */

  /// \brief Set the fake data values from either a file, or MC
  ///
  /// - Setting from a file "path": \n
  /// When reading from a file the full path must be given to a standard
  /// nuisance output. The standard MC histogram should have a name that matches
  /// this sample for it to be read in.
  /// \n\n
  /// - Setting from "MC": \n
  /// If the MC option is given the current MC prediction is used as fake data.
  virtual void SetFakeDataValues(std::string fakeOption);

  /// \brief Reset fake data back to starting fake data
  ///
  /// Reset the fake data back to original fake data (Reset back to before
  /// ThrowCovariance was first called)
  virtual void ResetFakeData(void);

  /// \brief Reset fake data back to original data
  ///
  /// Reset the data histogram back to the true original dataset for this sample
  /// before any fake data was defined.
  virtual void ResetData(void);

  /// \brief Generate fake data by throwing the covariance.
  ///
  /// Can be used on fake MC data or just the original dataset.
  /// Call ResetFakeData or ResetData to return to values before the throw.
  virtual void ThrowCovariance(void);

  /// \brief Throw the data by its assigned errors and assign this to MC
  ///
  /// Used when creating data toys by assign the MC to this thrown data
  /// so that the likelihood is calculated between data and thrown data
  virtual void ThrowDataToy(void);

  /*
    Access Functions
  */

  /// \brief Returns nicely formatted MC Histogram
  ///
  /// Format options can also be given in the samplesettings:
  /// - linecolor
  /// - linestyle
  /// - linewidth
  /// - fillcolor
  /// - fillstyle
  ///
  /// So to have a sample line colored differently in the xml cardfile put: \n
  /// <sample name="MiniBooNE_CCQE_XSec_1DQ2_nu" input="NEUT:input.root"
  /// linecolor="2" linestyle="7"  linewidth="2" />
  virtual TH2D *GetMCHistogram(void);

  /// \brief Returns nicely formatted data Histogram
  ///
  /// Format options can also be given in the samplesettings:
  /// - datacolor
  /// - datastyle
  /// - datawidth
  ///
  /// So to have a sample data colored differently in the xml cardfile put: \n
  /// <sample name="MiniBooNE_CCQE_XSec_1DQ2_nu" input="NEUT:input.root"
  /// datacolor="2" datastyle="7"  datawidth="2" />
  virtual TH2D *GetDataHistogram(void);

  /// \brief Returns a list of all MC histograms.
  ///
  /// Override this if you have extra histograms that need to be
  /// accessed outside of the Measurement1D class.
  inline virtual std::vector<TH1 *> GetMCList(void) {
    return std::vector<TH1 *>(1, GetMCHistogram());
  }

  /// \brief Returns a list of all Data histograms.
  ///
  /// Override this if you have extra histograms that need to be
  /// accessed outside of the Measurement1D class.
  inline virtual std::vector<TH1 *> GetDataList(void) {
    return std::vector<TH1 *>(1, GetDataHistogram());
  }

  /// \brief Returns a list of all Mask histograms.
  ///
  /// Override this if you have extra histograms that need to be
  /// accessed outside of the Measurement1D class.
  inline virtual std::vector<TH1 *> GetMaskList(void) {
    return std::vector<TH1 *>(1, fMaskHist);
  };

  /// \brief Returns a list of all Fine histograms.
  ///
  /// Override this if you have extra histograms that need to be
  /// accessed outside of the Measurement1D class.
  inline virtual std::vector<TH1 *> GetFineList(void) {
    return std::vector<TH1 *>(1, fMCFine);
  };

  /*
    Write Functions
  */
  /// \brief Save the current state to the current TFile directory \n
  ///
  /// Data/MC are both saved by default.
  /// A range of other histograms can be saved by setting the
  /// config option 'drawopts'.
  ///
  /// Possible options: \n
  /// - FINE    = Write Fine Histogram \n
  /// - WEIGHTS = Write Weighted MC Histogram (before scaling) \n
  /// - FLUX    = Write Flux Histogram from MC Input \n
  /// - EVT     = Write Event Histogram from MC Input \n
  /// - XSEC    = Write XSec Histogram from MC Input \n
  /// - MASK    = Write Mask Histogram \n
  /// - COV     = Write Covariance Histogram \n
  /// - INVCOV  = Write Inverted Covariance Histogram \n
  /// - DECMOP  = Write Decomp. Covariance Histogram \n
  /// - RESIDUAL= Write Resudial Histograms \n
  /// - RATIO   = Write Data/MC Ratio Histograms \n
  /// - SHAPE   = Write MC Shape Histograms norm. to Data \n
  /// - CANVMC  = Build MC Canvas Showing Data, MC, Shape \n
  /// - MODES   = Write PDG Stack \n
  /// - CANVPDG = Build MC Canvas Showing Data, PDGStack \n
  ///
  /// So to save a range of these in parameters/config.xml set: \n
  /// <config drawopts='FINE/COV/SHAPE/RATIO' />
  virtual void Write(std::string drawOpt);

  //////// OLD FUNCTIONS ////////////

  //! Intial setup of common measurement variables. Parse input files, types,
  //! etc.
  virtual void SetupMeasurement(std::string input, std::string type,
                                FitWeight *rw, std::string fkdt);

  //! Setup the default mc Hist given a data histogram
  virtual void SetupDefaultHist();

  //! Set the data values and errors from two files
  virtual void SetDataValues(std::string dataFile, double dataNorm,
                             std::string errorFile, double errorNorm);
  virtual void SetDataValues(std::string dataFile, std::string TH2Dname);

  //! Set the data values only from a text file
  virtual void SetDataValuesFromText(std::string dataFile, double norm);

  //! Read a covariance matrix from a file (Default name "covar" in file)
  virtual void SetCovarMatrix(std::string covarFile);

  //! Set the covariance matrix from a text file
  virtual void SetCovarMatrixFromText(std::string covarFile, int dim);

  //! Set the covariance matrix from a text file containing the cholesky
  //! fDecomposition
  virtual void SetCovarMatrixFromChol(std::string covarFile, int dim);

protected:
  // The data histograms
  TH2D *fDataHist;   //!< default data histogram (use in chi2 calculations)
  TH2D *fDataOrig;   //!< histogram to store original data before throws.
  TH2D *fDataTrue;   //!< histogram to store true dataset
  TH1D *fDataHist_X; //!< Projections onto X of the fDataHist
  TH1D *fDataHist_Y; //!< Projections onto Y of the fDataHist

  // The MC histograms
  TH2D *fMCHist;         //!< MC Histogram (used in chi2 calculations)
  TH2D *fMCFine;         //!< Finely binned MC Histogram
  TH2D *fMCHist_PDG[61]; //!< MC Histograms for each interaction mode
  TH1D *fMCHist_X;       //!< Projections onto X of the fMCHist
  TH1D *fMCHist_Y;       //!< Projections onto Y of the fMCHist
  TH2D *fMCWeighted;     //!< Raw Event Weights
  TH2D *fMCStat;

  TH2I *fMaskHist; //!< mask histogram for the data
  TH2I *fMapHist;  //!< map histogram used to convert 2D to 1D distributions

  TH2D *fResidualHist;
  TH2D *fChi2LessBinHist;

  bool fIsFakeData;         //!< is current data actually fake
  std::string fakeDataFile; //!< MC fake data input file

  std::string fPlotTitles; //!< X and Y plot titles.
  std::string fFitType;
  std::string fDefaultTypes; //!< Default Fit Options
  std::string fAllowedTypes; //!< Any allowed Fit Options

  TMatrixDSym *covar;      //!< inverted covariance matrix
  TMatrixDSym *fFullCovar; //!< covariance matrix
  TMatrixDSym *fDecomp;    //!< fDecomposed covariance matrix
  TMatrixDSym *fCorrel;    //!< correlation matrix
  TMatrixDSym *fShapeCovar;
  double fCovDet;    //!< covariance deteriminant
  double fNormError; //!< Normalisation on the error on the data

  double fLikelihood; //!< Likelihood value

  Double_t *fXBins;    //!< X Bin Edges
  Double_t *fYBins;    //!< Y Bin Edges
  Int_t fNDataPointsX; //!< Number of X data points
  Int_t fNDataPointsY; //!< NUmber of Y data points

  // Fit specific flags
  bool fIsShape; //!< Flag: Perform shape-only fit
  bool fUseShapeNormDecomp;
  bool fIsFree;      //!< Flag: Perform normalisation free fit
  bool fIsDiag;      //!< Flag: Only use diagonal bin errors in stats
  bool fIsMask;      //!< Flag: Apply bin masking
  bool fIsRawEvents; //!< Flag: Only event rates in histograms
  bool fIsEnu;       //!< Needs Enu Unfolding
  bool fIsChi2SVD;   //!< Flag: Chi2 SVD Method (DO NOT USE)
  bool fAddNormPen;  //!< Flag: Add normalisation penalty to fi
  bool fIsProjFitX;  //!< Flag: Use 1D projections onto X and Y to calculate the
  //! Chi2 Method. If flagged X will be used to set the rate.
  bool fIsProjFitY; //!< Flag: Use 1D projections onto X and Y to calculate the
  //! Chi2 Method. If flagged Y will be used to set the rate.
  bool fIsFix;     //!< Flag: Fixed Histogram Norm
  bool fIsFull;    //!< Flag; Use Full Covar
  bool fIsDifXSec; //!< Flag: Differential XSec
  bool fIsEnu1D;   //!< Flag: Flux Unfolded XSec
  bool fIsChi2;    //!< Flag; Use Chi2 over LL
  bool fIsWriting;

  TrueModeStack *fMCHist_Modes; ///< Optional True Mode Stack

  TMatrixDSym *fCovar;  ///< New FullCovar
  TMatrixDSym *fInvert; ///< New covar

  // Fake Data
  std::string fFakeDataInput; ///< Input fake data file path
  TFile *fFakeDataFile;       ///< Input fake data file

  // Arrays for data entries
  Double_t *fDataValues; ///< REMOVE data bin contents
  Double_t *fDataErrors; ///< REMOVE data bin errors
};

/*! @} */
#endif
