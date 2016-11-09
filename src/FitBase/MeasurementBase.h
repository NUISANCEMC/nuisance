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

#ifndef INPUTHANDLER_H_SEEN
#define INPUTHANDLER_H_SEEN

/*!
 *  \addtogroup FitBase
 *  @{
 */

// C Includes
#include <stdlib.h>
#include <numeric>
#include <math.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <deque>
#include <time.h>
#include <list>

// ROOT includes
#include <TROOT.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TArrayF.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TMatrixDSym.h>
#include <TDecompSVD.h>
#include <TDecompChol.h>
#include <TSystem.h>

// External data fit includes
#include "FitEvent.h"
#include "FitUtils.h"
#include "PlotUtils.h"
#include "StatUtils.h"
#include "FitParameters.h"
#include "GeneralUtils.h"

#include "FitWeight.h"

#include "TMultiDimFit.h"

#ifdef __GENIE_ENABLED__
#include "Conventions/Units.h"
#endif

#include "TObject.h"
#include "InputHandler.h"
#include "EventManager.h"

/// Enumerations to help with extra plot functions
enum extraplotflags {
  kExtraPlotError   = 0,
  kExtraPlotReset   = 1,
  kExtraPlotFill    = 2,
  kExtraPlotConvert = 3,
  kExtraPlotWrite   = 4
};


/// InputHandler Class
///
/// Inherits from Measurement base to handle whatever input is throwna t the fitter automatically.
/// All functions here handle how files are read in, converted to custom formats and reconfigures are called.
/// Used generally for the MC inputs.

//! 2nd level experiment class that handles converting MC into a common format and calling reconfigure
class MeasurementBase {
 public:

  /*
    Constructor/Destructors
  */
  //! Default Constructor. Set everything to NULL
  MeasurementBase(void);

  //! Default virtual destructor
  virtual ~MeasurementBase(void);

  /*
    Reconfigure Functions
  */

  //! Function called if MC tuning dials haven't been changed and all we want to do is update the normalisation.
  virtual void Renormalise(void);

  //! Call reconfigure only looping over signal events to save time.
  virtual void ReconfigureFast(void);

  //! Call reconfigure looping over all MC events including background
  virtual void Reconfigure(void);

  virtual TH2D GetCovarMatrix(void) = 0;
  virtual double GetLikelihood(void){return 0.0;};
  virtual int GetNDOF(void){return 0;};
  virtual void ThrowCovariance(void) = 0;
  virtual void SetFakeDataValues(std::string fkdt) = 0;


  //! Get the total integrated flux between this samples energy range
  virtual double TotalIntegratedFlux(std::string intOpt="width",double low=-9999.9, double high=-9999.9);

  //! Get the predicted event rate for this sample
  virtual double PredictedEventRate(std::string intOpt="width",double low=-9999.9, double high=-9999.9);


  int GetPassed() {
    int signalSize = fXVar_VECT.size();
    return signalSize;
  }

  int GetTotal() {
    return fNEvents;
  }

  /*
    Reconfigure LOOP
  */
  // All these should be virtual
  ///! Reset Histograms (Handled at Measurement Stage)
  virtual void ResetAll(void) = 0;

  ///! Fill the event variables for this sample (Handled in each inherited sample)
  virtual void FillEventVariables(FitEvent* event){(void)event;};

  ///! Check whether this event is signle (Handled in each inherited sample)
  virtual bool isSignal(FitEvent* event){ (void)event; return false;};

  ///! Fill the histogram for this event using fXVar and fYVar (Handled in each inherited sample)
  virtual void FillHistograms(void){};

  ///! Convert event rates to whatever distributions you need.
  virtual void ConvertEventRates(void);

  ///! Call scale events after the plots have been filled at the end of reconfigure.
  virtual void ScaleEvents(void){};

  ///! Apply the scale factor at the end of reconfigure.
  virtual void ApplyNormScale(double norm){(void) norm;};

  ///! Save Histograms
  virtual void Write(std::string drawOpt = "") = 0;

  /*
    Histogram Access Functions
  */

  ///! Virtual function to get data histogram
  virtual std::vector<TH1*> GetDataList(void) = 0;

  ///! Virtual function to get MC histogram
  virtual std::vector<TH1*> GetMCList   (void) = 0;
  virtual std::vector<TH1*> GetFineList (void) = 0;
  virtual std::vector<TH1*> GetMaskList (void) = 0;

  ///! Return flux histograms in a vector
  virtual std::vector<TH1*> GetFluxList      (void);
  virtual std::vector<TH1*> GetEventRateList (void);
  virtual std::vector<TH1*> GetXSecList      (void);

  ///! Return input for this sample
  InputHandler* GetInput (void);

  std::string GetName (void){ return fName; };
  double GetScaleFactor(void){ return fScaleFactor; };

  double GetXVar(void){ return fXVar; };
  double GetYVar(void){ return fYVar; };
  double GetZVar(void){ return fZVar; };
  double GetMode(void){ return this->Mode;  };
  double GetEnu(void){ return this->Enu; };
  
  void SetupInputs(std::string inputfile);
  int GetInputID(void);
  void SetSignal(bool sig);
  void SetSignal(FitEvent* evt);
  void SetWeight(double wght);
  void SetMode(int md);
  void SetNoData(bool isTrue=true){ fNoData = isTrue; };

  inline void SetXVar(double xvar){ fXVar = xvar; };
  inline void SetYVar(double yvar){ fYVar = yvar; };
  inline void SetZVar(double zvar){ fZVar = zvar; };
  
  
protected:

  // Minimum and maximum energies
  double Enu; //!< Neutrino Energy
  double EnuMin; //!< Minimum incoming particle energy of events to include
  double EnuMax; //!< Maximum incoming particle energy of events to include

  BaseFitEvt* signal_event;
  FitEvent* cust_event;
  FitWeight* fRW; //!< Pointer to the rw engine
  InputHandler* fInput; //!< Instance of the input handler
  std::string fName;
  int fEventType;

  TH1D* fEventHist;
  TH1D* fXSecHist;
  TH1D* fFluxHist;

  double fBeamDistance; //!< Incoming Particle flight distance (for oscillation analysis)
  double fScaleFactor; //!< fScaleFactor applied to events to convert from eventrate to final distribution
  double fCurrentNorm; //!< current normalisation factor applied if fit is "FREE"
  bool fMCFilled; //!< flag whether MC plots have been filled (For ApplyNormalisation)
  bool fNoData; //!< flag whether data plots do not exist (for ratios)

  // TEMP OBJECTS TO HANDLE MERGE
  double fXVar,fYVar,fZVar,Mode,Weight;
  bool Signal;
  int ievt;
  int fNEvents;
  double Enu_rec, ThetaMu, CosThetaMu;

  std::vector<double> fXVar_VECT;
  std::vector<double> fYVar_VECT;
  std::vector<double> fZVar_VECT;
  std::vector<int>    fMode_VECT;
  std::vector<UInt_t> fIndex_VECT;

  std::string inputfilename;
};

// Class TypeDefs
typedef std::list<MeasurementBase*>::const_iterator MeasListConstIter;
typedef std::list<MeasurementBase*>::iterator MeasListIter;
typedef std::vector<MeasurementBase*>::const_iterator MeasVectConstIter;
typedef std::vector<MeasurementBase*>::iterator MeasVectIter;

/*! @} */
#endif
