// Copyright 2016 L. Pickering, P towell, R. Terri, C. Wilkinson, C. Wret

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

#include <math.h>
#include <stdlib.h>
#include <deque>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>

// ROOT includes
#include <TArrayF.h>
#include <TCanvas.h>
#include <TCut.h>
#include <TDecompChol.h>
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
#include "PlotUtils.h"
#include "StatUtils.h"

#include "SignalDef.h"
#include "MeasurementVariableBox.h"
#include "MeasurementVariableBox1D.h"

namespace NUISANCE {
namespace FitBase {

}
}

//********************************************************************
/// 1D Measurement base class. Histogram handling is done in this base layer.
class Measurement1D : public MeasurementBase {
//********************************************************************

public:
  /*
    Constructor/Deconstuctor
  */
  Measurement1D(void);
  virtual ~Measurement1D(void);

  /*
    Setup Functions
  */

  /// \brief Setup all configs once initialised
  ///
  /// Should be called after all configs have been setup inside fSettings container.
  /// Handles the processing of inputs and setting up of types.
  /// Replaces the old 'SetupMeasurement' function.
  void FinaliseSampleSettings();

  /// \brief Creates the 1D data distribution given the binning provided.
  virtual void CreateDataHistogram(int dimx, double* binx);

  /// \brief Read 1D data inputs from a text file.
  ///
  /// Inputfile should have the format: \n
  /// low_binedge_1    bin_content_1  bin_error_1 \n
  /// low_binedge_2    bin_content_2  bin_error_2 \n
  /// ....             ....           ....        \n
  /// high_bin_edge_N  0.0            0.0
  virtual void SetDataFromTextFile(std::string datafile);

  /// \brief Read 1D data inputs from a root file.
  ///
  /// inhistfile specifies the path to the root file
  /// histname specifies the name of the histogram.
  ///
  /// If no histogram name is given the inhistfile value
  /// is automatically parsed with ';' so that: \n
  /// 'myhistfile.root;myhistname' \n
  /// will also work.
  virtual void SetDataFromRootFile(std::string inhistfile, std::string histname = "");


  /// \brief Setup a default empty data histogram
  ///
  /// Only used for flattree creators.
  virtual void SetEmptyData();

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
  virtual void SetCovarFromDiagonal(TH1D* data = NULL);

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

  virtual void SetCovarFromMultipleTextFiles(std::string covfiles, int dim = -1);

  /// \brief Read the data covariance from a ROOT file.
  ///
  /// - covfile specifies the full path to the file
  /// - histname specifies the name of the covariance object. Both TMatrixDSym and TH2D are supported.
  ///
  /// If no histogram name is given the inhistfile value
  /// is automatically parsed with ; so that: \n
  /// mycovfile.root;myhistname \n
  /// will also work.
  virtual void SetCovarFromRootFile(std::string covfile, std::string histname="");

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
  virtual void SetCovarInvertFromRootFile(std::string covfile, std::string histname="");

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

  /// \brief Read the data correlations from multiple text files.
  ///
  /// \warning REQUIRES DATA HISTOGRAM TO BE SET FIRST
  ///
  /// Inputfile should have similar format to that shown
  /// in SetCovarFromTextFile.
  ///
  /// If no dimensions are given, it is assumed from the number
  /// entries in the first line of the first corrfile.
  virtual void SetCorrelationFromMultipleTextFiles(std::string corrfiles, int dim = -1);

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
  virtual void SetCorrelationFromRootFile(std::string covfile, std::string histname="");


  /// \brief Read the cholesky decomposed covariance from a text file and turn it into a covariance
  ///
  /// Inputfile should have similar format to that shown
  /// in SetCovarFromTextFile.
  ///
  /// If no dimensions are given, it is assumed from the number
  /// entries in the first line of covfile.
  virtual void SetCholDecompFromTextFile(std::string covfile, int dim = -1);


  /// \brief Read the cholesky decomposed covariance from a ROOT file and turn it into a covariance
  ///
  /// Inputfile should have similar format to that shown
  /// in SetCovarFromRootFile.
  ///
  /// If no histogram name is given the inhistfile value
  /// is automatically parsed with ; so that: \n
  /// mycovfile.root;myhistname \n
  /// will also work.
  virtual void SetCholDecompFromRootFile(std::string covfile, std::string histname="");

  /// \brief Try to extract a shape-only matrix from the existing covariance
  virtual void SetShapeCovar();

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
  /// bin_index_1  1 \n
  /// bin_index_2  1 \n
  /// bin_index_3  1 \n
  ///
  /// If 0 is given then a bin entry will NOT be masked. So for example: \n\n
  /// 1  1 \n
  /// 2  1 \n
  /// 3  0 \n
  /// 4  1 \n\n
  /// Will mask only the 1st, 2nd, and 4th bins.
  ///
  /// Masking can be turned on by specifiying the MASK option when creating a sample.
  /// When this is passed NUISANCE will look in the following locations for the mask file:
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
  /// are conflicting. Any options seperated with the '/' symbol are non conflicting
  /// and can be given together, whereas any seperated with the ',' symbol cannot
  /// be specified by the end user at the same time.
  ///
  /// Default Type Examples:
  /// - DIAG/FIX = Default option will be a diagonal covariance, with FIXED norm.
  /// - MASK/SHAPE = Default option will be a masked hist, with SHAPE always on.
  ///
  /// Allowed Type examples:
  /// - 'FULL/DIAG/NORM/MASK' = Any of these options can be specified.
  /// - 'FULL,FREE,SHAPE/MASK/NORM' = User can give either FULL, FREE, or SHAPE as on option.
  /// MASK and NORM can also be included as options.
  virtual void SetFitOptions(std::string opt);


  /// \brief Final constructor setup
  /// \warning Should be called right at the end of the constructor.
  ///
  /// Contains a series of checks to ensure the data and inputs have been setup.
  /// Also creates the MC histograms needed for fitting.
  void FinaliseMeasurement();




  /*
    Smearing
  */
  /// \brief Read in smearing matrix from file
  ///
  /// Set the smearing matrix from a text file given the size of the matrix
  virtual void SetSmearingMatrix(std::string smearfile, int truedim,
                                 int recodim);

  /// \brief Apply smearing to MC true to get MC reco
  ///
  /// Apply smearing matrix to fMCHist using fSmearingMatrix
  virtual void ApplySmearingMatrix(void);



  /*
    Reconfigure Functions
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
  virtual MeasurementVariableBox* CreateBox() {return new MeasurementVariableBox1D();};

  /// \brief Reset all MC histograms
  ///
  /// Resets all standard histograms and those registered to auto
  /// process to zero.
  ///
  /// If extra histograms are not included in auto processing, then they must be reset
  /// by overriding this function and doing it manually if required.
  virtual void ResetAll(void);

  /// \brief Fill MC Histograms from XVar
  ///
  /// Fill standard histograms using fXVar, Weight read from the variable box.
  ///
  /// WARNING : Any extra MC histograms need to be filled by overriding this function,
  /// even if they have been set to auto process.
  virtual void FillHistograms(void);

  // \brief Convert event rates to final histogram
  ///
  /// Apply standard scaling procedure to standard mc histograms to convert from
  /// raw events to xsec prediction.
  ///
  /// If any distributions have been set to auto process
  /// that is done during this function call, and a differential xsec is assumed.
  /// If that is not the case this function must be overriden.
  virtual void ScaleEvents(void);

  /// \brief Scale MC by a factor=1/norm
  ///
  /// Apply a simple normalisation scaling if the option FREE or a norm_parameter
  /// has been specified in the NUISANCE routine.
  virtual void ApplyNormScale(double norm);


  /*
    Statistical Functions
  */

  /// \brief Get Number of degrees of freedom
  ///
  /// Returns the number bins inside the data histogram accounting for
  /// any bin masking applied.
  virtual int GetNDOF(void);

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
  virtual TH1D* GetMCHistogram(void);

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
  virtual TH1D* GetDataHistogram(void);

  /// \brief Returns a list of all MC histograms.
  ///
  /// Override this if you have extra histograms that need to be
  /// accessed outside of the Measurement1D class.
  inline virtual std::vector<TH1*> GetMCList(void) {
    return std::vector<TH1*>(1, GetMCHistogram());
  }

  /// \brief Returns a list of all Data histograms.
  ///
  /// Override this if you have extra histograms that need to be
  /// accessed outside of the Measurement1D class.
  inline virtual std::vector<TH1*> GetDataList(void) {
    return std::vector<TH1*>(1, GetDataHistogram());
  }

  /// \brief Returns a list of all Mask histograms.
  ///
  /// Override this if you have extra histograms that need to be
  /// accessed outside of the Measurement1D class.
  inline virtual std::vector<TH1*> GetMaskList(void) {
    return std::vector<TH1*>(1, fMaskHist);
  };

  /// \brief Returns a list of all Fine histograms.
  ///
  /// Override this if you have extra histograms that need to be
  /// accessed outside of the Measurement1D class.
  inline virtual std::vector<TH1*> GetFineList(void) {
    return std::vector<TH1*>(1, fMCFine);
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



  virtual void WriteRatioPlot();




  virtual void WriteShapePlot();
  virtual void WriteShapeRatioPlot();




  //*
  // OLD DEFUNCTIONS
  //

  /// OLD FUNCTION
  virtual void SetupMeasurement(std::string input, std::string type,
          FitWeight* rw, std::string fkdt);

  /// OLD FUNCTION
  virtual void SetupDefaultHist(void);

  /// OLD FUNCTION
  virtual void SetDataValues(std::string dataFile);

  /// OLD FUNCTION
  virtual void SetDataFromFile(std::string inhistfile, std::string histname);
  /// OLD FUNCTION
  virtual void SetDataFromDatabase(std::string inhistfile,
             std::string histname);

  /// OLD FUNCTION
  virtual void SetCovarMatrix(std::string covarFile);
  /// OLD FUNCTION
  virtual void SetCovarMatrixFromText(std::string covarFile, int dim,
                double scale = 1.0);
  /// OLD FUNCTION
  virtual void SetCovarMatrixFromCorrText(std::string covarFile, int dim);


  /// OLD FUNCTION
  virtual void SetCovarFromDataFile(std::string covarFile, std::string covName,
				    bool FullUnits = false);


  /// OLD FUNCTION
  // virtual THStack GetModeStack(void);

protected:

  // Data
  TH1D* fDataHist;  ///< default data histogram
  TH1D* fDataOrig;  ///< histogram to store original data before throws.
  TH1D* fDataTrue;  ///< histogram to store true dataset
  std::string fPlotTitles;  ///< Plot title x and y for the histograms


  // MC
  TH1D* fMCHist;     ///< default MC Histogram used in the chi2 fits
  TH1D* fMCFine;     ///< finely binned MC histogram
  TH1D* fMCStat;     ///< histogram with unweighted events to properly calculate
  TH1D* fMCWeighted; ///< Weighted histogram before xsec scaling

  TH1I* fMaskHist;   ///< Mask histogram for neglecting specific bins
  TMatrixD* fSmearMatrix;   ///< Smearing matrix (note, this is not symmetric)

  TH1D *fResidualHist;
  TH1D *fChi2LessBinHist;

  TrueModeStack* fMCHist_Modes; ///< Optional True Mode Stack


  // Statistical
  TMatrixDSym* covar;       ///< Inverted Covariance
  TMatrixDSym* fFullCovar;  ///< Full Covariance
  TMatrixDSym* fDecomp;     ///< Decomposed Covariance
  TMatrixDSym* fCorrel;     ///< Correlation Matrix

  TMatrixDSym* fShapeCovar;  ///< Shape-only covariance
  TMatrixDSym* fShapeDecomp; ///< Decomposed shape-only covariance
  TMatrixDSym* fShapeInvert; ///< Inverted shape-only covariance

  TMatrixDSym* fCovar;    ///< New FullCovar
  TMatrixDSym* fInvert;   ///< New covar

  double fNormError;        ///< Sample norm error

  double fLikelihood; ///< Likelihood value

  // Fake Data
  bool fIsFakeData;            ///< Flag: is the current data fake from MC
  std::string fFakeDataInput;  ///< Input fake data file path
  TFile* fFakeDataFile;        ///< Input fake data file


  // Fit specific flags
  std::string fFitType;       ///< Current fit type
  std::string fAllowedTypes;  ///< Fit Types Possible
  std::string fDefaultTypes;  ///< Starting Default Fit Types

  bool fIsShape;      ///< Flag : Perform Shape-only fit
  bool fUseShapeNormDecomp;
  bool fIsFree;       ///< Flag : Perform normalisation free fit
  bool fIsDiag;       ///< Flag : only include uncorrelated diagonal errors
  bool fIsMask;       ///< Flag : Apply bin masking
  bool fIsRawEvents;  ///< Flag : Are bin contents just event rates
  bool fIsEnu1D;      ///< Flag : Perform Flux Unfolded Scaling
  bool fIsChi2SVD;    ///< Flag : Use alternative Chi2 SVD Method (Do not use)
  bool fAddNormPen;   ///< Flag : Add a normalisation penalty term to the chi2.
  bool fIsFix;        ///< Flag : keeping norm fixed
  bool fIsFull;       ///< Flag : using full covariaince
  bool fIsDifXSec;    ///< Flag : creating a dif xsec
  bool fIsChi2;       ///< Flag : using Chi2 over LL methods
  bool fIsSmeared;    ///< Flag : Apply smearing?
  bool fIsWriting;


  /// OLD STUFF TO REMOVE
  TH1D* fMCHist_PDG[61]; ///< REMOVE OLD MC PDG Plot

  // Arrays for data entries
  Double_t* fXBins;       ///< REMOVE xBin edges
  Double_t* fDataValues;  ///< REMOVE data bin contents
  Double_t* fDataErrors;  ///< REMOVE data bin errors
  Int_t fNDataPointsX;    ///< REMOVE number of data points

};

/*! @} */
#endif
