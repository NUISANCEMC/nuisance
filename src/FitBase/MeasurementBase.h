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
#include "FitBuild.h"
#include "FitWeight.h"

#include "TMultiDimFit.h"

#ifdef __GENIE_ENABLED__
#include "Conventions/Units.h"
#endif

#include "TObject.h"
#include "InputHandler.h"
#include "SignalDef.h"

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
  MeasurementBase();

  //! Default virtual destructor
  virtual ~MeasurementBase();

  /*
    Reconfigure Functions
  */

  //! Function called if MC tuning dials haven't been changed and all we want to do is update the normalisation.
  virtual void Renormalise();

  //! Call reconfigure only looping over signal events to save time.
  virtual void ReconfigureFast();

  //! Call reconfigure looping over all MC events including background
  virtual void Reconfigure();

  virtual TH2D GetCovarMatrix() = 0;
  virtual double GetLikelihood(){return 0.0;};
  virtual int GetNDOF(){return 0;};
  virtual void ThrowCovariance() = 0;
  virtual void SetFakeDataValues(std::string fkdt) = 0;



  //! Set the flux histogram from file
  virtual void SetFluxHistogram(std::string fluxFile, int minE, int maxE, double fluxNorm);

  //! Get the total integrated flux between this samples energy range
  virtual double TotalIntegratedFlux(std::string intOpt="width",double low=-9999.9, double high=-9999.9);

  //! Get the predicted event rate for this sample
  virtual double PredictedEventRate(std::string intOpt="width",double low=-9999.9, double high=-9999.9);




  
  /*
    Reconfigure LOOP
  */
  // All these should be virtual
  ///! Reset Histograms (Handled at Measurement Stage)
  virtual void ResetAll() = 0;

  ///! Fill the event variables for this sample (Handled in each inherited sample)
  virtual void FillEventVariables(FitEvent* event){(void)event;};

  ///! Check whether this event is signle (Handled in each inherited sample) 
  virtual bool isSignal(FitEvent* event){ (void)event; return false;};

  ///! Fill the histogram for this event using X_VAR and Y_VAR (Handled in each inherited sample) 
  virtual void FillHistograms(){};

  ///! Convert event rates to whatever distributions you need.
  virtual void ConvertEventRates();
  
  ///! Call scale events after the plots have been filled at the end of reconfigure.
  virtual void ScaleEvents(){};

  ///! Apply the scale factor at the end of reconfigure.
  virtual void ApplyNormScale(double norm){(void) norm;};

  ///! Save Histograms
  virtual void Write(std::string drawOpt = "") = 0;
    
  /*
    Histogram Access Functions
  */

  ///! Virtual function to get data histogram
  virtual std::vector<TH1*> GetDataList() = 0;

  ///! Virtual function to get MC histogram
  virtual std::vector<TH1*> GetMCList() = 0;
  virtual std::vector<TH1*>  GetFineList() = 0;
  virtual std::vector<TH1*> GetMaskList() = 0;
  
  ///! Return Flux Lists as a vector
  virtual std::vector<TH1*> GetFluxList(){return std::vector<TH1*>(1,fluxHist);};

  ///! Return Event Rate Lists as a vector
  virtual std::vector<TH1*> GetEventRateList(){return std::vector<TH1*>(1,eventHist);};

  ///! Return XSec Lists as a vector
  virtual std::vector<TH1*> GetXSecList(){return std::vector<TH1*>(1,xsecHist);};

  InputHandler* GetInput(){ return input; };
  std::string GetName(){ return measurementName; };
  double GetScaleFactor(){ return scaleFactor; };

  double GetXVar(){ return this->X_VAR; };
  double GetYVar(){ return this->Y_VAR; };
  double GetZVar(){ return this->Z_VAR; };
  double GetMode(){ return this->Mode;  };
  double GetEnu(){ return this->Enu; };
  
  
protected:

  // Minimum and maximum energies
  double Enu; //!< Neutrino Energy  
  double EnuMin; //!< Minimum incoming particle energy of events to include
  double EnuMax; //!< Maximum incoming particle energy of events to include

  FitEventBase* signal_event;
  FitEvent* cust_event;
  FitWeight* rw_engine; //!< Pointer to the rw engine
  InputHandler* input; //!< Instance of the input handler
  std::string measurementName;
  int eventType;
  
  // Input Event rate flux/event histograms
  TH1D* fluxHist;  //!< Flux Histogram
  TH1D* eventHist; //!< Event Histogram
  TH1D* xsecHist;  //!< XSec Histogram

  double exp_distance; //!< Incoming Particle flight distance (for oscillation analysis)
  double scaleFactor; //!< scaleFactor applied to events to convert from eventrate to final distribution
  double currentNorm; //!< current normalisation factor applied if fit is "FREE"
  bool filledMC; //!< flag whether MC plots have been filled (For ApplyNormalisation)
  
  // TEMP OBJECTS TO HANDLE MERGE
  double X_VAR,Y_VAR,Z_VAR,Mode,Weight;
  bool Signal;
  int ievt;
  int nevents;
  double Enu_rec, ThetaMu, CosThetaMu;

  std::vector<double> X_VAR_VECT;
  std::vector<double> Y_VAR_VECT;
  std::vector<double> Z_VAR_VECT;
  std::vector<int>    MODE_VECT;
  std::vector<bool>   SIGNAL_VECT;
};

/*! @} */
#endif
