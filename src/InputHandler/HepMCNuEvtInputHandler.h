#pragma once

#include "HepMCNuEvtTools/ReaderTools"

#include "InputHandler.h"
#include "PlotUtils.h"
#include "TargetUtils.h"

class HepMCNuEvtInputHandler : public InputHandlerBase {
public:
  HepMCNuEvtInputHandler(std::string const &handle,
                         std::string const &rawinputs);
  ~HepMCNuEvtInputHandler();

  void HepMCNuEvtInputHandler::Reset();

  FitEvent *GetNuisanceEvent(const UInt_t entry);

  UInt_t fEntriesUsed;
  std::string fInputFile;
  HepMC3Nu::ReaderRootTree *rdr;
};
