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
  BaseFitEvt* GetSignalPointer(){ return signal_event; };
  
  int GetNEvents(){ return this->nEvents; };
  int GetGenEvents();
  
  void PrintStartInput();
  void ReadEvent(unsigned int i);
  
  TH1D* GetFluxHistogram(){return this->fluxHist;};
  TH1D* GetEventHistogram(){return this->eventHist;};
  TH1D* GetXSecHistogram(){return this->xsecHist;};

  std::vector<TH1*> GetFluxList(){ return this->fluxList;};
  std::vector<TH1*> GetEventList(){ return this->eventList;};
  std::vector<TH1*> GetXSecList(){ return this->xsecList;};

  int GetType(){return eventType;};
  bool CanIGoFast();
  void GetTreeEntry(const Long64_t entry);
  std::string GetInputStateString();

  int eventType;
  double GetInputWeight(const int entry=-1);
  
 protected:

  TChain* tn;
  BaseFitEvt* signal_event;
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
  std::string inType; ///!< Input Type
  
  std::vector<BaseFitEvt*> all_events;
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

  std::vector<int> joint_index_low;
  std::vector<int> joint_index_high;
  std::vector<TH1D*> joint_index_hist;
  std::vector<double> joint_index_weight;
  bool isJointInput;
  int cur_entry;

  std::vector<TH1*> xsecList;
  std::vector<TH1*> eventList;
  std::vector<TH1*> fluxList;

  std::vector<TArrayD> spline_list;
  
  ClassDef(InputHandler, 1);
};
#endif
