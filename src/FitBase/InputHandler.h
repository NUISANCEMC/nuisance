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

#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "stdint.h"

#include "TObject.h"

#include "FitEvent.h"
#include "FitParameters.h"
#include "FitUtils.h"
#include "FitWeight.h"
#include "PlotUtils.h"
#include "InputUtils.h"

#ifdef __GENIE_ENABLED__
#include "Conventions/Units.h"
#endif

class InputHandler {
 public:
  InputHandler(void){};
  ~InputHandler(void){};

  InputHandler(std::string const& handle, InputUtils::InputType inpType,
               std::string const& inputs);

  void ReadBinSplineFile(void);
  void ReadHistogramFile(void);
  void ReadNeutFile(void);
  void ReadNuanceFile(void);
  void ReadGenieFile(void);
  void ReadNuWroFile(void);
  void ReadEventSplineFile(void);
  void ReadJointFile(void);
  void ReadGiBUUFile(void);
  void ReadEmptyEvents(void);  // For Validations
  void ReadFitEvents(void);
  //  FitSplineHead* GetSplineHead(void);

  double PredictedEventRate(double low, double high,
                            std::string intOpt = "width");
  double TotalIntegratedFlux(double low, double high,
                             std::string intOpt = "width");

  inline FitEvent* GetEventPointer(void) { return fEvent; };
  inline BaseFitEvt* GetSignalPointer(void) { return fSignalEvent; };

  inline int GetNEvents(void) const { return fNEvents; };
  int GetGenEvents(void);
  std::string GetInputFileName(){return fInputFile; }
  InputUtils::InputType GetInputFileType(){return fInputType; }

  void PrintStartInput(void);
  void ReadEvent(unsigned int i);

  inline TH1D* GetFluxHistogram(void) { return fFluxHist; };
  inline TH1D* GetEventHistogram(void) { return fEventHist; };
  inline TH1D* GetXSecHistogram(void) { return fXSecHist; };

  std::vector<TH1*> GetFluxList(void) { return fFluxList; };
  std::vector<TH1*> GetEventList(void) { return fEventList; };
  std::vector<TH1*> GetXSecList(void) { return fXSecList; };

  inline int GetType(void) const { return fEventType; };

  bool CanIGoFast(void);
  void GetTreeEntry(const Long64_t entry);
  std::string GetInputStateString(void);
  double GetInputWeight(const int entry = -1);
  void ReadNewSplines();

  void SetupCache();

 protected:
  FitEvent* fEvent;
  int fEventType;
  BaseFitEvt* fSignalEvent;

  //  FitSplineHead* fSplineHead;

  int fMaxEvents;
  int fNEvents;
  int fEventIndex;

  TH1D* fFluxHist;
  TH1D* fEventHist;
  TH1D* fXSecHist;

  std::string fName;
  std::string fInput;

  InputUtils::InputType fInputType;
  std::string fInputFile;
  TFile* fInputRootFile;

  std::vector<BaseFitEvt*> fAllBaseEvents;

  std::vector<int> fJointIndexLow;
  std::vector<int> fJointIndexHigh;
  std::vector<TH1D*> fJointIndexHist;
  std::vector<double> fJointIndexScale;
  bool fIsJointInput;
  bool fIsExplicitJointInput; // Kept for backwards compatibility

  std::vector<TH1*> fXSecList;
  std::vector<TH1*> fEventList;
  std::vector<TH1*> fFluxList;

  double** fSplineArray;
//  std::vector<double*> fAllSplines;

// Horribly Wrapped Event Objects!
// Have to add this hear incase one generator isn't supported.
#ifdef __NEUT_ENABLED__
  NeutVect* fNeutVect;  //!< Pointer to NEUT Events
#endif
#ifdef __NUWRO_ENABLED__
  event*
      fNuwroEvent;  //!< Pointer to NuWro Events (Set to bool if NUWRO disabled)
#endif
#ifdef __GENIE_ENABLED__
  GHepRecord* fGenieGHep;
  NtpMCEventRecord* fGenieNtpl;
#endif
#ifdef __NUANCE_ENABLED__
  NuanceEvent* fNuanceEvt;
#endif

  TTree* fSplineTree;
  TChain* tn;
};
#endif
