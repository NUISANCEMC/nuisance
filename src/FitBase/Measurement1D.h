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
  Measurement1D();

  //! Default destructor
  virtual ~Measurement1D();

  //! Initialise values to NULL
  void Init();

  /*
    Setup Functions
  */
  //! Setup the measurement with input values. This automatically handles parsing the fit type and data.
  virtual void SetupMeasurement(std::string input, std::string type, FitWeight *rw, std::string fkdt);

  //! Setup the default MC histograms from the currently set dataHist.
  virtual void SetupDefaultHist();

  //! Set fit options by parsing type
  virtual void SetFitOptions(std::string opt);

  //! Setup the data values from a text file
  virtual void SetDataValues(std::string dataFile);

  //! Setup the data values by reading in a histogram from a TFile
  virtual void SetDataFromFile(std::string inhistfile, std::string histname);

  //! Setup the data by reading in a histogram from the database.
  virtual void SetDataFromDatabase(std::string inhistfile, std::string histname);

  //! Read the covariance matrix from a root file (automatically grabs plot "covar")
  virtual void SetCovarMatrix(std::string covarFile);

  //! Read the covariance matrix from a text file given the covar size
  virtual void SetCovarMatrixFromText(std::string covarFile, int dim);

  //! Set the covariance from a custom root file
  virtual void SetCovarFromDataFile(std::string covarFile, std::string covName);

  //! Set the smearing matrix from a text file given the size of the matrix
  virtual void SetSmearingMatrix(std::string smearFile, int true_dim, int reco_dim);

  //! Set the bin mask from a text file
  virtual void SetBinMask(std::string maskFile);

  
  /*
    XSec Functions
  */
  //! Set the flux histogram from file
  virtual void SetFluxHistogram(std::string fluxFile, int minE, int maxE, double fluxNorm);

  //! Get the total integrated flux between this samples energy range
  virtual double TotalIntegratedFlux(std::string intOpt="width",double low=-9999.9, double high=-9999.9);

  /*
    Reconfigure Functions
  */
  //! Produce an MC prediction given the currently loaded bin splines

  /*
    Reconfigure LOOP
  */

  //! Reset histograms to zero
  virtual void ResetAll();

  //! Fill histograms using X_VAR and Weight
  virtual void FillHistograms();
  

  //! Apply histogram scaling after reconfigure
  virtual void ScaleEvents();

  //! Apply normalisation scale after reconfigure
  virtual void ApplyNormScale(double norm);

  //! Apply smearing matrix to mcHist
  virtual void ApplySmearingMatrix();

  /*
    Statistic Functions - Outsources to StatUtils
  */

  //! Get the current Number of degrees of freedom accounting for bin masking.
  virtual int GetNDOF();
  
  //! Get Likelihood of iteration
  virtual double GetLikelihood();

  /*
    Fake Data Functions
  */

  //! Set the fake data values from either a file, or MC using fakeOption="MC"
  virtual void SetFakeDataValues(std::string fakeOption); // Set dataHist from file

  //! Reset the fake data back to original fake data (Reset to before ThrowCovariance was called)
  virtual void ResetFakeData();

  //! Reset the fake data back to the true original dataset for this sample
  virtual void ResetData();

  //! Generate fake data by throwing the current dataHist using the covariance.
  //! Can be used on fake MC data or just the original dataset.
  virtual void ThrowCovariance();

  // Get MC Histogram Stack
  virtual THStack GetModeStack();

  /*
    Access Functions
  */

  TH1D* GetMCHistogram(){return mcHist;};
  TH1D* GetDataHistogram(){ return dataHist;};
  virtual std::vector<TH1*> GetMCList();
  virtual std::vector<TH1*> GetDataList();
  virtual std::vector<TH1*> GetMaskList(){return std::vector<TH1*> (1, maskHist);};
  virtual std::vector<TH1*> GetFineList(){return std::vector<TH1*> (1, mcFine);};


  //! Get the bin contents and errors from mcHist
  virtual void GetBinContents(std::vector<double>& cont, std::vector<double>& err);

  //! Get the covariance matrix as a pretty plot
  virtual TH2D GetCovarMatrix(){ return TH2D(*covar);};

  //! Return the integrated XSec for this sample, options define whether data or MC is returned.
  virtual std::vector<double> GetXSec(std::string opt);

  /*
    Write Functions
  */

  //! Save the current state to the current TFile directory
  virtual void Write(std::string drawOpt);
  

  //! array of histograms to handle mcHist for each interaction channel.
  TH1D* mcHist_PDG[61];

protected:

  // data histograms
  TH1D* dataHist; //!< default data histogram
  TH1D* dataOrig; //!< histogram to store original data before throws.
  TH1D* dataTrue; //!< histogram to store true dataset

  // Fake Data Flag
  bool usingfakedata; //!< Flag: whether the current data is actually fake from MC
  std::string fakeDataFile; //!< Input fake data file

  // The histogram into which the measurement will be filled
  TH1D* mcHist; //!< default MC Histogram used in the chi2 fits
  TH1D* mcFine; //!< finely binned MC histogram
  TH1D* mcStat; //!< histogram with unweighted events to properly calculate statistical error on MC

  TH1I* maskHist; //!< Mask histogram for neglecting specific bins

  std::string plotTitles; //!< Plot title x and y for the histograms

  // The covariance matrix and its decomposition
  TMatrixDSym *covar;      //!< Inverted Covariance
  TMatrixDSym *fullcovar;  //!< Full Covariance
  TMatrixDSym *decomp;     //!< Decomposed Covariance
  TMatrixDSym *correl;     //!< Correlation Matrix
  TMatrixD *smear;         //!< Smearing matrix (note, this is not symmetric, and also in general not square)
  double covDet;     //!< Determinant of the covariance
  double normError;  //!< Sample norm error
  std::string fitType;

  // Arrays for data entries
  Double_t* xBins;  //!< xBin edges
  Double_t* data_values; //!< data bin contents
  Double_t* data_errors; //!< data bin errors
  Int_t data_points; //!< number of data points

  // Fit specific flags
  bool isShape; //!< Flag: Perform Shape-only fit
  bool isFree; //!< Flag: Perform normalisation free fit
  bool isDiag; //!< Flag: only include uncorrelated diagonal errors
  bool isMask; //!< Flag: Apply bin masking
  bool isRawEvents; //!< Flag: Are bin contents just event rates
  bool isEnu1D; //!< Flag: Perform Flux Unfolded Scaling
  bool isChi2SVD; //!< Flag: Use alternative Chi2 SVD Method (Do not use)
  bool addNormPenalty; //!< Flag: Add a normalisation penalty term to the chi2.
  bool isFix; //!< Flag for keeping norm fixed
  bool isFull; //!< Flag for using full covariaince  
  bool isDifXSec; //!< Flag for creating a dif xsec
  bool isChi2; //!< Flag for using Chi2 over LL methods
  
  std::string allowed_types; //!< Fit Types Possible
  std::string default_types; //!< Starting Default Fit Types

};

/*! @} */
#endif
