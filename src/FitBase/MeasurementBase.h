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

#ifndef INPUTHANDLER_H_SEEN
#define INPUTHANDLER_H_SEEN

/*!
 *  \addtogroup FitBase
 *  @{
 */

// C Includes
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <deque>
#include <iomanip>
#include <iostream>
#include <list>
#include <numeric>
#include <sstream>
#include <string>

// ROOT includes
#include <TArrayF.h>
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
#include "GeneralUtils.h"
#include "PlotUtils.h"
#include "StatUtils.h"
#include "InputFactory.h"
#include "FitWeight.h"

#include "TMultiDimFit.h"

#ifdef GENIE_ENABLED
#ifdef GENIE3_API_ENABLED
#include "Framework/Conventions/Units.h"
#else
#include "Conventions/Units.h"
#endif
#endif

#include "EventManager.h"
#include "TObject.h"
#include "InputHandler.h"
#include "NuisConfig.h"
#include "NuisKey.h"
#include "SampleSettings.h"
#include "StackBase.h"
#include "StandardStacks.h"

/// Enumerations to help with extra plot functions
enum extraplotflags {
  kCMD_Reset = 0,
  kCMD_Fill,
  kCMD_Scale,
  kCMD_Norm,
  kCMD_Write,
  kCMD_Error,
  kCMD_extraplotflags
};

enum MeasurementSpeciesClass {
  kSingleSpeciesMeasurement = 0,
  kNumuWithWrongSignMeasurement,
  kNueWithWrongSignMeasurement,
  kFourSpeciesMeasurement,
};

/// InputHandler Class
///
/// Inherits from Measurement base to handle whatever input is throwna t the
/// fitter automatically.
/// All functions here handle how files are read in, converted to custom formats
/// and reconfigures are called.
/// Used generally for the MC inputs.

//! 2nd level experiment class that handles converting MC into a common format
//! and calling reconfigure


class MeasurementBase {
public:
  /*
    Constructor/Destructors
  */
  //! Default Constructor. Set everything to NULL
  MeasurementBase();

  //! Default virtual destructor
  virtual ~MeasurementBase(void);
  virtual void InitialSetup(void) {};

  /*
    Reconfigure Functions
  */
  //! Function called if MC tuning dials haven't been changed and all we want to
  //! do is update the normalisation.
  virtual void Renormalise(void);

  //! Call reconfigure only looping over signal events to save time.
  virtual void ReconfigureFast(void);

  virtual void FillHistograms(double weight);


  //! Call reconfigure looping over all MC events including background
  virtual void Reconfigure(void);

  // virtual TH2D GetCovarMatrix(void) = 0;
  virtual double GetLikelihood(void) { return 0.0; };
  virtual int GetNDOF(void) { return 0; };
  virtual void ThrowCovariance(void) = 0;
  virtual void ThrowDataToy(void) = 0;
  virtual void SetFakeDataValues(std::string fkdt) = 0;

  //! Get the total integrated flux between this samples energy range
  virtual double TotalIntegratedFlux(std::string intOpt = "width",
                                     double low = -9999.9,
                                     double high = -9999.9);

  //! Get the predicted event rate for this sample
  virtual double PredictedEventRate(std::string intOpt = "width",
                                    double low = -9999.9,
                                    double high = -9999.9);

  virtual SampleSettings LoadSampleSettings(nuiskey samplekey);
  virtual SampleSettings LoadSampleSettings(std::string name, std::string input, std::string type);

  virtual void FinaliseSampleSettings();
  virtual void FinaliseMeasurement();
  virtual void ProcessExtraHistograms(int cmd, MeasurementVariableBox* vars,
                                      double weight = 1.0);

  virtual void FillExtraHistograms(MeasurementVariableBox* vars, double weight = 1.0);
  virtual void ScaleExtraHistograms(MeasurementVariableBox* vars);
  virtual void ResetExtraHistograms();
  virtual void NormExtraHistograms(MeasurementVariableBox* vars, double norm = 1.0);
  virtual void WriteExtraHistograms();
  virtual MeasurementVariableBox* CreateBox() {return new MeasurementVariableBox();};

  int GetPassed() {
    int signalSize = 0;
    return signalSize;
  }

  int GetTotal() { return fNEvents; }

  /*
    Reconfigure LOOP
  */
  // All these should be virtual
  ///! Reset Histograms (Handled at Measurement Stage)
  virtual void ResetAll(void) = 0;

  ///! Fill the event variables for this sample (Handled in each inherited
  /// sample)
  virtual void FillEventVariables(FitEvent* event) { (void)event; };

  ///! Check whether this event is signle (Handled in each inherited sample)
  virtual bool isSignal(FitEvent* event) {
    (void)event;
    return false;
  };

  ///! Fill the histogram for this event using fXVar and fYVar (Handled in each
  /// inherited sample)
  virtual void FillHistograms(void) {};

  ///! Convert event rates to whatever distributions you need.
  virtual void ConvertEventRates(void);

  ///! Call scale events after the plots have been filled at the end of
  /// reconfigure.
  virtual void ScaleEvents(void) {};

  ///! Apply the scale factor at the end of reconfigure.
  virtual void ApplyNormScale(double norm) { (void)norm; };

  ///! Save Histograms
  virtual void Write(std::string drawOpt = "") = 0;

  virtual MeasurementVariableBox* FillVariableBox(FitEvent* event);

  virtual MeasurementVariableBox* GetBox();

  void FillHistogramsFromBox(MeasurementVariableBox* var, double weight);
  /*
    Histogram Access Functions
  */

  ///! Virtual function to get data histogram
  virtual std::vector<TH1*> GetDataList(void) = 0;

  ///! Virtual function to get MC histogram
  virtual std::vector<TH1*> GetMCList(void) = 0;
  virtual std::vector<TH1*> GetFineList(void) = 0;
  virtual std::vector<TH1*> GetMaskList(void) = 0;

  ///! Return flux histograms in a vector
  virtual std::vector<TH1*> GetFluxList(void);
  virtual std::vector<TH1*> GetEventRateList(void);
  virtual std::vector<TH1*> GetXSecList(void);

  virtual TH1D* GetEventHistogram() { return fInput->GetEventHistogram(); };
  virtual TH1D* GetXSecHistogram() { return fInput->GetXSecHistogram(); };
  virtual TH1D* GetFluxHistogram() { return fInput->GetFluxHistogram(); };

  ///! Return input for this sample
  InputHandlerBase* GetInput(void);

  std::string GetName(void) { return fName; };
  double GetScaleFactor(void) { return fScaleFactor; };

  double GetXVar(void) { return fXVar; };
  double GetYVar(void) { return fYVar; };
  double GetZVar(void) { return fZVar; };
  double GetMode(void) { return this->Mode; };
  double GetEnu(void) { return this->Enu; };

  void SetupInputs(std::string inputfile);
  int GetInputID(void);
  std::string GetInputFileName() { return fInputFileName; };
  void SetSignal(bool sig);
  void SetSignal(FitEvent* evt);
  void SetWeight(double wght);
  void SetMode(int md);
  void SetNoData(bool isTrue = true) { fNoData = isTrue; };

  inline void SetXVar(double xvar) { fXVar = xvar; };
  inline void SetYVar(double yvar) { fYVar = yvar; };
  inline void SetZVar(double zvar) { fZVar = zvar; };

  virtual std::vector<MeasurementBase*> GetSubSamples() {
    return std::vector<MeasurementBase*>(1, this);
  }


  void SetAutoProcessTH1(TH1* hist,  int c1 = -1,
                         int c2 = -1, int c3 = -1,
                         int c4 = -1, int c5 = -1);
  void SetAutoProcess(TH1* hist,  int c1 = -1,
		      int c2 = -1, int c3 = -1,
		      int c4 = -1, int c5 = -1);
  void SetAutoProcess(TGraph* g,  int c1 = -1,
                      int c2 = -1, int c3 = -1,
                      int c4 = -1, int c5 = -1);
  void SetAutoProcess(TF1* f,  int c1 = -1,
                      int c2 = -1, int c3 = -1,
                      int c4 = -1, int c5 = -1);
  void SetAutoProcess(StackBase* hist, int c1 = -1,
		      int c2 = -1, int c3 = -1,
		      int c4 = -1, int c5 = -1);
  void SetAutoProcessTH1(StackBase* hist, int c1 = -1,
                         int c2 = -1, int c3 = -1,
                         int c4 = -1, int c5 = -1);
  void AutoFillExtraTH1();
  void AutoResetExtraTH1();
  void AutoScaleExtraTH1();
  void AutoNormExtraTH1(double norm);
  void AutoWriteExtraTH1();


  // functions that need to be added.
  // - Initial Check
  // - Check Target/Beam loop.
  // - Check flux shape if suggested one given.
  // - Return MeasurementList (returns )


protected:
  // Minimum and maximum energies
  double Enu;     //!< Neutrino Energy
  double EnuMin;  //!< Minimum incoming particle energy of events to include
  double EnuMax;  //!< Maximum incoming particle energy of events to include

  BaseFitEvt* signal_event;
  FitEvent* cust_event;

  FitWeight* fRW;        //!< Pointer to the rw engine
  InputHandlerBase* fInput;  //!< Instance of the input handler

  std::string fName; //!< Name of the sample
  int fEventType;

  double fBeamDistance;  //!< Incoming Particle flight distance (for oscillation
  //! analysis)
  double fScaleFactor;   //!< fScaleFactor applied to events to convert from
  //! eventrate to final distribution
  double
  fCurrentNorm;  //!< current normalisation factor applied if fit is "FREE"
  bool fMCFilled;    //!< flag whether MC plots have been filled (For
  //! ApplyNormalisation)
  bool fNoData;      //!< flag whether data plots do not exist (for ratios)
  bool fIsNoWidth;    ///< Flag : Don't scale by bin width

  // TEMP OBJECTS TO HANDLE MERGE
  double fXVar, fYVar, fZVar, Mode, Weight;
  bool Signal;
  int ievt;
  int fNEvents;
  double Enu_rec, ThetaMu, CosThetaMu;

  InputUtils::InputType fInputType;
  std::string fInputFileName;
  TH1D* fFluxHist;
  TH1D* fEventHist;

  MeasurementSpeciesClass fMeasurementSpeciesType;
  SampleSettings fSettings;

  MeasurementVariableBox* fEventVariables;

  std::map<StackBase*, std::vector<int> > fExtraTH1s;
  int NSignal;
  // std::map<TH1*, bool[6] > fExtaStacks;

  bool fIsJoint;



  double fNPOT, fFluxIntegralOverride, fTargetVolume, fTargetMaterialDensity;
  double fEvtRateScaleFactor;

};




// Class TypeDefs
typedef std::list<MeasurementBase*>::const_iterator MeasListConstIter;
typedef std::list<MeasurementBase*>::iterator MeasListIter;
typedef std::vector<MeasurementBase*>::const_iterator MeasVectConstIter;
typedef std::vector<MeasurementBase*>::iterator MeasVectIter;

/*! @} */
#endif
