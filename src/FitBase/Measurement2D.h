#ifndef MEASUREMENT_2D_HXX_SEEN
#define MEASUREMENT_2D_HXX_SEEN

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
#include <fstream>
#include <iomanip>
#include <deque>

// ROOT includes
#include <TROOT.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TArrayF.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TMatrixDSym.h>
#include <TDecompSVD.h>
#include <TSystem.h>

// External data fit includes
#include "MeasurementBase.h"
#include "FitEvent.h"
#include "FitUtils.h"
#include "StatUtils.h"
#include "PlotUtils.h"
#include "FitParameters.h"
#include "SignalDef.h"

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

  //! Intial setup of common measurement variables. Parse input files, types, etc.
  virtual void SetupMeasurement(std::string input, std::string type, FitWeight *rw, std::string fkdt);

  //! Setup the default mc Hist given a data histogram
  virtual void SetupDefaultHist();

  //! Parse the fit type to get fit options
  virtual void SetFitOptions(std::string opt);

  //! Set the data values and errors from two files
  virtual void SetDataValues(std::string dataFile, double dataNorm, std::string errorFile, double errorNorm);
  virtual void SetDataValues(std::string dataFile, std::string TH2Dname);

  //! Set the data values only from a text file
  virtual void SetDataValuesFromText(std::string dataFile,  double norm);

  //! Read a covariance matrix from a file (Default name "covar" in file)
  virtual void SetCovarMatrix(std::string covarFile);

  //! Set the covariance matrix from a text file
  virtual void SetCovarMatrixFromText(std::string covarFile, int dim);

  //! Set the covariance matrix from a text file containing the cholesky decomposition
  virtual void SetCovarMatrixFromChol(std::string covarFile, int dim);

  // virtual void SetMaskValuesFromText(std::string dataFile);

  //! Read in a histogram 2Dto1D map from a text file
  virtual void SetMapValuesFromText(std::string dataFile);

  //! Set the bin mask for a 2D histogram (list: bini, binj, MaskFlag)
  virtual void SetBinMask(std::string maskFile);


  // virtual void ReadHistogramFile();

  /*
    XSec Functions
  */
  //! Set the flux from a text file
  virtual void SetFluxHistogram(std::string fluxFile, int minE, int maxE, double fluxNorm);

  //! Get the integrated flux between this measurements energy ranges
  virtual double TotalIntegratedFlux(std::string intOpt="width",double low=-9999.9, double high=-9999.9);

  /*
    Reconfigure LOOP
  */

  //! Reset the MC Histograms to zero
  virtual void ResetAll();

  //! Fill the histograms given X_VAR, Y_VAR, and Weight for this event
  virtual void FillHistograms();

  //! Apply event scaling to XSec values after reconfigure has been called
  virtual void ScaleEvents();

  //! Apply a normalisation scale in free normalisation fits after reconfigure has been called
  virtual void ApplyNormScale(double norm);

  /*
    Statistic Functions - Outsources to StatUtils
   */

  //! Get the Number of degrees of freedom accounting for bin masking
  virtual int GetNDOF();

  //! Get the likelihood at current state
  virtual double GetLikelihood();


  /*
    Fake Data Functions
  */

  //! Set fake data values from MC. Use external file, or current MC prediction using option "MC"
  virtual void SetFakeDataValues(std::string fakeOption);
  // virtual void ResetFakeData();
  // virtual void ResetData();

  //! Use the covariance to throw fake data from the current dataHist
  virtual void ThrowCovariance();

  virtual THStack GetModeStack();

  /*
    Access Functions
  */

  TH2D* GetMCHistogram(){ return mcHist; };
  TH2D* GetDataHistogram(){ return dataHist; };

  virtual std::vector<TH1*> GetMCList();
  virtual std::vector<TH1*> GetDataList();
  virtual std::vector<TH1*> GetMaskList(){return std::vector<TH1*> (1, maskHist);};
  virtual std::vector<TH1*> GetFineList(){return std::vector<TH1*> (1, mcFine);};

  //! Get bin contents and errors from mcHist and fill a vector with them
  virtual void GetBinContents(std::vector<double>& cont,std::vector<double>& err);

  //! Get covariance matrix as a pretty plot
  virtual TH2D GetCovarMatrix(){ return TH2D(*covar);};

  //! Get Integrated XSec (option flags whether to get data or MC)
  virtual std::vector<double> GetXSec(std::string option);

  /*
    Write Functions
  */

  //! Save Histograms to the current directory
  virtual void Write(std::string drawOpt);

protected:

  // The data histograms
  TH2D* dataHist; //!< default data histogram (use in chi2 calculations)
  TH2D* dataOrig; //!< histogram to store original data before throws.
  TH2D* dataTrue; //!< histogram to store true dataset

  TH1D* dataHist_X; //!< Projections onto X of the dataHist
  TH1D* dataHist_Y; //!< Projections onto Y of the dataHist

  // The MC histograms
  TH2D* mcHist;  //!< MC Histogram (used in chi2 calculations)
  TH2D* mcFine;  //!< Finely binned MC Histogram
  TH2D* mcHist_PDG[61]; //!< MC Histograms for each interaction mode

  TH1D* mcHist_X; //!< Projections onto X of the mcHist
  TH1D* mcHist_Y; //!< Projections onto Y of the mcHist

  // Fake Data Flag
  std::string fitType;
  bool usingfakedata;  //!< is current data actually fake
  std::string fakeDataFile; //!< MC fake data input file

  // Mask Histogram
  TH2I* maskHist; //!< mask histogram for the data
  TH2I* mapHist; //!< map histogram used to convert 2D to 1D distributions

  // Plot Titles for X Y and Z
  std::string plotTitles; //!< X and Y plot titles.

  // The covariance matrix and its decomposition
  TMatrixDSym *covar;  //!< inverted covariance matrix
  TMatrixDSym *fullcovar;  //!< covariance matrix
  TMatrixDSym *decomp; //!< decomposed covariance matrix
  TMatrixDSym *correl; //!< correlation matrix
  double covDet;  //!< covariance deteriminant
  double normError; //!< Normalisation on the error on the data

  // Arrays for data entires
  Double_t* xBins; //!< X Bin Edges
  Double_t* yBins; //!< Y Bin Edges
  Int_t data_points_x;  //!< Number of X data points
  Int_t data_points_y;  //!< NUmber of Y data points

  // Fit specific flags
  bool isShape;   //!< Flag: Perform shape-only fit
  bool isFree;    //!< Flag: Perform normalisation free fit
  bool isDiag;    //!< Flag: Only use diagonal bin errors in stats
  bool isMask;    //!< Flag: Apply bin masking
  bool isRawEvents;   //!< Flag: Only event rates in histograms
  bool isEnu;
  bool isChi2SVD;   //!< Flag: Chi2 SVD Method (DO NOT USE)
  bool addNormPenalty; //!< Flag: Add normalisation penalty to fi
  bool isProjFitX; //!< Flag: Use 1D projections onto X and Y to calculate the Chi2 Method. If flagged X will be used to set the rate.
  bool isProjFitY; //!< Flag: Use 1D projections onto X and Y to calculate the Chi2 Method. If flagged Y will be used to set the rate.
  bool isFix;   //!< Flag: Fixed Histogram Norm
  bool isFull;  //!< Flag; Use Full Covar
  bool isDifXSec; //!< Flag: Differential XSec
  bool isEnu1D;  //!< Flag: Flux Unfolded XSec
  bool isChi2;  //!< Flag; Use Chi2 over LL
  
  std::string default_types; //!< Default Fit Options
  std::string allowed_types; //!< Any allowed Fit Options

};

/*! @} */
#endif
