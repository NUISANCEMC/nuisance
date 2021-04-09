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
#ifndef INPUTHANDLER2_H
#define INPUTHANDLER2_H
/*!
 *  \addtogroup InputHandler
 *  @{
 */
#include "BaseFitEvt.h"
#include "FitEvent.h"
#include "TH1D.h"
#include "TTreePerfStats.h"

/// Base InputHandler class defining how events are requested and setup.
class InputHandlerBase {
public:
  /// Base constructor resets everything to default
  InputHandlerBase();

  /// Removes flux/event rate histograms
  virtual ~InputHandlerBase();

  /// Return NUISANCE FitEvent Class from given event entry.
  /// Must be overriden by GeneratorInputHandler. Lightweight allows a faster
  /// option to be given where only RW information is needed.
  virtual FitEvent *GetNuisanceEvent(const UInt_t entry,
                                     const bool lightweight = false) = 0;
  /// Calls GetNuisanceEvent(entry, TRUE);
  virtual BaseFitEvt *GetBaseEvent(const UInt_t entry);

  /// Print current event information
  virtual void Print();

  /// Return handler ID
  inline std::string GetName(void) { return fName; };
  /// Return Handler Event Type Index
  inline int GetType(void) { return fEventType; };

  /// Get Total Number of Events being Handled
  inline virtual int GetNEvents(void) { return std::max(fNEvents - fSkip, 0); };
  /// Get the Total Flux Histogram these events were generated with
  inline virtual TH1D *GetFluxHistogram(void) { return fFluxHist; };
  /// Get the Total Event Histogram these events were generated with
  inline virtual TH1D *GetEventHistogram(void) { return fEventHist; };
  /// Get the Total Cross-section Histogram (EventHist/FluxHist)
  virtual TH1D *GetXSecHistogram(void);

  /// Return all Flux Histograms for all InputFiles.
  virtual std::vector<TH1 *> GetFluxList(void);
  /// Return all Event Histograms for all InputFiles
  virtual std::vector<TH1 *> GetEventList(void);
  /// Return all Xsec Histograms for all InputFiles
  virtual std::vector<TH1 *> GetXSecList(void);

  /// Placeholder to create a cache to speed up reads in GeneratorInputHandler
  inline virtual void CreateCache(){};
  /// Placeholder to remove optional cache to free up memory
  inline virtual void RemoveCache(){};

  /// Return starting NUISANCE event pointer (entry=0)
  FitEvent *FirstNuisanceEvent();
  /// Iterate to next NUISANCE event. Returns NULL when entry > fNEvents.
  FitEvent *NextNuisanceEvent();
  /// Returns starting Base Event Pointer (entry=0)
  BaseFitEvt *FirstBaseEvent();
  /// Iterate to next NUISANCE Base Event. Returns NULL when entry > fNEvents.
  BaseFitEvt *NextBaseEvent();

  /// Register an input file and update event/flux information
  virtual void RegisterJointInput(std::string input, int n, TH1D *f, TH1D *e);
  /// Finalise setup of Input event/flux information and calculate
  /// joint input weights if joint input is provided.
  virtual void SetupJointInputs();
  /// Calculate a weight for the event given the joint input information.
  /// Used to scale the relative proportion of multiple inputs correctly
  /// with respect to one another.
  virtual double GetInputWeight(int entry);

  /// Returns the total predicted event rate for this input given the
  /// low and high energy ranges. intOpt specifies the option the ROOT
  /// TH1D integral should use. e.g. "" or "width"
  double PredictedEventRate(double low = -9999.9, double high = -9999.9,
                            std::string intOpt = "");

  /// Returns the total generated flux for this input given the
  /// low and high energy ranges. intOpt specifies the option the ROOT
  /// TH1D integral should use. e.g. "" or "width"
  double TotalIntegratedFlux(double low = -9999.9, double high = -9999.9,
                             std::string intOpt = "");

  /// Actual data members.
  std::vector<TH1D *> jointfluxinputs;
  std::vector<TH1D *> jointeventinputs;
  std::vector<int> jointindexlow;
  std::vector<int> jointindexhigh;
  std::vector<int> jointindexallowed;
  size_t jointindexswitch;
  bool jointinput;
  std::vector<double> jointindexscale;

  std::string fName;
  TH1D *fFluxHist;
  TH1D *fEventHist;
  TH1D *fXSecHist;
  int fNEvents;
  int fMaxEvents;
  FitEvent *fNUISANCEEvent;
  BaseFitEvt *fBaseEvent;
  int fEventType;
  int fCurrentIndex;
  int fCacheSize;
  bool kRemoveUndefParticles;
  bool kRemoveFSIParticles;
  bool kRemoveNuclearParticles;
  TTreePerfStats *fTTreePerformance;
  int fSkip;
};
/*! @} */
#endif
