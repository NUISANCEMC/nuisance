#ifndef INPUTHANDLER2_H
#define INPUTHANDLER2_H

#include "TH1D.h"
#include "FitEvent.h"
#include "BaseFitEvt.h"

class InputHandlerBase {
public:
  InputHandlerBase() {
    fName = "";
    fFluxHist = NULL;
    fEventHist = NULL;
    fNEvents = 0;
    fNUISANCEEvent = NULL;
    fBaseEvent = NULL;
  };
  ~InputHandlerBase() {};

  virtual FitEvent* GetNuisanceEvent(const UInt_t entry) = 0;
  virtual BaseFitEvt* GetBaseEvent(const UInt_t entry) = 0;
  virtual void Print(){};

  inline int GetNEvents(void) { return fNEvents; }
  inline TH1D* GetFluxHistogram(void) {return fFluxHist;};
  inline TH1D* GetEventHistogram(void) {return fEventHist;};
  inline std::string GetName(void) {return fName;};
  inline int GetType(void) {return fEventType;};

  inline TH1D* GetXSecHistogram(void) {
    fXSecHist = (TH1D*)fFluxHist->Clone();
    fXSecHist->Divide(fEventHist);
    return fXSecHist;
  };

//********************************************************************
  inline double PredictedEventRate(double low, double high,
                                   std::string intOpt) {
    //********************************************************************

    int minBin = fFluxHist->GetXaxis()->FindBin(low);
    int maxBin = fFluxHist->GetXaxis()->FindBin(high);

    return fEventHist->Integral(minBin, maxBin + 1, intOpt.c_str());
  };

  //********************************************************************
  inline double TotalIntegratedFlux(double low, double high,
                                    std::string intOpt) {
    //********************************************************************

    Int_t minBin = fFluxHist->GetXaxis()->FindFixBin(low);
    Int_t maxBin = fFluxHist->GetXaxis()->FindFixBin(high);

    if ((fFluxHist->IsBinOverflow(minBin) && (low != -9999.9))) {
      minBin = 1;
    }

    if ((fFluxHist->IsBinOverflow(maxBin) && (high != -9999.9))) {
      maxBin = fFluxHist->GetXaxis()->GetNbins() + 1;
    }


    // If we are within a single bin
    if (minBin == maxBin) {
      // Get the contained fraction of the single bin's width
      return ((high - low) / fFluxHist->GetXaxis()->GetBinWidth(minBin)) *
             fFluxHist->Integral(minBin, minBin, intOpt.c_str());
    }

    double lowBinUpEdge = fFluxHist->GetXaxis()->GetBinUpEdge(minBin);
    double highBinLowEdge = fFluxHist->GetXaxis()->GetBinLowEdge(maxBin);

    double lowBinfracIntegral =
      ((lowBinUpEdge - low) / fFluxHist->GetXaxis()->GetBinWidth(minBin)) *
      fFluxHist->Integral(minBin, minBin, intOpt.c_str());
    double highBinfracIntegral =
      ((high - highBinLowEdge) / fFluxHist->GetXaxis()->GetBinWidth(maxBin)) *
      fFluxHist->Integral(maxBin, maxBin, intOpt.c_str());

    // If they are neighbouring bins
    if ((minBin + 1) == maxBin) {
      // Get the contained fraction of the two bin's width
      return lowBinfracIntegral + highBinfracIntegral;
    }

    // If there are filled bins between them
    return lowBinfracIntegral + highBinfracIntegral +
           fFluxHist->Integral(minBin + 1, maxBin - 1, intOpt.c_str());
  }


  std::vector<TH1*> GetFluxList(void) { return std::vector<TH1*>(1, fFluxHist); };
  std::vector<TH1*> GetEventList(void) { return std::vector<TH1*>(1, fEventHist); };
  std::vector<TH1*> GetXSecList(void) { return std::vector<TH1*>(1, GetXSecHistogram()); };

  virtual FitEvent* FirstNuisanceEvent() {
    fCurrentIndex = 0;
    return GetNuisanceEvent(fCurrentIndex);
  };



  virtual FitEvent* NextNuisanceEvent() {
    fCurrentIndex++;

    if (jointinput and fMaxEvents != -1) {
      while ( fCurrentIndex < jointindexlow[jointindexswitch] ||
              fCurrentIndex >= jointindexhigh[jointindexswitch] ) {
        jointindexswitch++;

        // Loop Around
        if (jointindexswitch == jointindexlow.size()) {
          jointindexswitch = 0;
        }
      }


      if (fCurrentIndex > jointindexlow[jointindexswitch] + jointindexallowed[jointindexswitch]) {
        fCurrentIndex = jointindexlow[jointindexswitch];
      }
    }

    return GetNuisanceEvent(fCurrentIndex);
  };


  virtual BaseFitEvt* FirstBaseEvent() {
    fCurrentIndex = 0;
    return GetBaseEvent(fCurrentIndex);
  };

  virtual BaseFitEvt* NextBaseEvent() {
    fCurrentIndex++;

    if (jointinput and fMaxEvents != -1) {
      while ( fCurrentIndex < jointindexlow[jointindexswitch] ||
              fCurrentIndex >= jointindexhigh[jointindexswitch] ) {
        jointindexswitch++;

        // Loop Around
        if (jointindexswitch == jointindexlow.size()) {
          jointindexswitch = 0;
        }
      }


      if (fCurrentIndex > jointindexlow[jointindexswitch] + jointindexallowed[jointindexswitch]) {
        fCurrentIndex = jointindexlow[jointindexswitch];
      }
    }

    return GetBaseEvent(fCurrentIndex);
  };




  std::vector<TH1D*> jointfluxinputs;
  std::vector<TH1D*> jointeventinputs;
  std::vector<int> jointindexlow;
  std::vector<int> jointindexhigh;
  std::vector<int> jointindexallowed;
  size_t jointindexswitch;
  bool jointinput;
  std::vector<double> jointindexscale;

  std::string fName;
  TH1D* fFluxHist;
  TH1D* fEventHist;
  TH1D* fXSecHist;
  int fNEvents;
  int fMaxEvents;
  FitEvent* fNUISANCEEvent;
  BaseFitEvt* fBaseEvent;
  int fEventType;
  int fCurrentIndex;
};




#endif