#pragma once

#include "InputHandler.h"

#include "TH1D.h"

/// An input handler type that can be used where an input handler is required by
/// the framework, but no events need to be read.
class DummyInputHandler : public InputHandlerBase {
public:
  DummyInputHandler() {
    dummyh = std::make_unique<TH1D>("dummyh", "", 1, 0, 100);
    dummyh->SetDirectory(nullptr);
    fFluxHist = dummyh.get();
  }
  ~DummyInputHandler() {}

  FitEvent *GetNuisanceEvent(const UInt_t, bool) { return nullptr; }

  BaseFitEvt *GetBaseEvent(const UInt_t) { return nullptr; }

  std::unique_ptr<TH1D> dummyh;

  TH1D *GetFluxHistogram() { return dummyh.get(); };
  TH1D *GetEventHistogram() { return dummyh.get(); };
  TH1D *GetXSecHistogram() { return dummyh.get(); };

  double GetInputWeight(const UInt_t entry) { return 1; }
};
