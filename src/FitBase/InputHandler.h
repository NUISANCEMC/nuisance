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

#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include "TObject.h"
#include "FitEvent.h"
#include "FitWeight.h"
#include "FitParameters.h"
#include "PlotUtils.h"
#include "FitUtils.h"
#include "stdint.h"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cstring>

#ifdef __GENIE_ENABLED__
#include "Conventions/Units.h"
#endif

class InputHandler : public TObject {

 public:
  InputHandler(){};
  ~InputHandler(){};

  InputHandler(std::string handle, std::string infile_name);
  std::string ParseInputFile(std::string inputfile);
  
  void ReadBinSplineFile(){};
  void ReadHistogramFile(){};
  void ReadNeutFile();
  void ReadGenieFile();
  void ReadNuWroFile();
  void ReadEventSplineFile();
  void ReadJointFile();
  FitSplineHead* GetSplineHead();

  double PredictedEventRate(double low, double high, std::string intOpt="width");
  double TotalIntegratedFlux(double low, double high, std::string intOpt="width");
  FitEvent* GetEventPointer(){ return cust_event; };
  FitEventBase* GetSignalPointer(){ return signal_event; };
  
  int GetNEvents(){ return this->nEvents; };
  int GetGenEvents();
  
  void PrintStartInput();
  void ReadEvent(unsigned int i);
  
  TH1D* GetFluxHistogram(){return this->fluxHist;};
  TH1D* GetEventHistogram(){return this->eventHist;};
  TH1D* GetXSecHistogram(){return this->xsecHist;};

  int GetType(){return eventType;};
  bool CanIGoFast();
  void GetTreeEntry(const Long64_t entry);
  
  int eventType;
  double GetInputWeight(const int entry=-1);
  
 protected:

  TChain* tn;
  FitEventBase* signal_event;
  FitEvent* cust_event;
  FitSplineHead* splhead;
  
  int maxEvents;
  int nEvents;
  int curevt_i;

  // Input Event rate flux/event histograms
  TH1D* fluxHist;  //!< Flux Histogram
  TH1D* eventHist; //!< Event Histogram
  TH1D* xsecHist;  //!< XSec Histogram

  // input root files
  TFile* inRootFile; //!< Input ROOT file (e.g NEUT MC)
  std::string inFile; ///!< Name of input ROOT file
  std::string inType;
  
  std::vector<FitEventBase*> signal_events;
  std::string handleName;
  
#ifdef __NEUT_ENABLED__
  NeutVect *neut_event; //!< Pointer to NEUT Events  
#endif
  
#ifdef __NUWRO_ENABLED__
  event* nuwro_event; //!< Pointer to NuWro Events (Set to bool if NUWRO disabled)                                     
#endif

#ifdef __GENIE_ENABLED__
  GHepRecord* genie_event;  //!< Pointer to GENIE GHepRecord                                                           
  NtpMCEventRecord * mcrec; //!< Pointer to GENIE NTuple Record                                                        
#endif

  std::vector< int >         input_startindex;
  std::map< TUUID, double >  input_weights;
  std::map< TUUID, int >     input_nMAXevents;
  std::map< TUUID, TH1D*  >  input_flux;
  std::map< TUUID, TH1D*  >  input_evtrt;
  std::map< TUUID, TH1D*  >  input_xsec;
  
  bool isJointInput;
  int cur_entry;
  
  ClassDef(InputHandler, 1);
};
#endif
