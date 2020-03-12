#pragma once

#include "NuHepMC/ReaderTools"

#include "InputHandler.h"
#include "PlotUtils.h"
#include "TargetUtils.h"

class NuHepMCInputHandler : public InputHandlerBase {
public:
  NuHepMCInputHandler(std::string const &handle,
                         std::string const &rawinputs);
  ~NuHepMCInputHandler();

  void Reset();

  FitEvent *GetNuisanceEvent(const UInt_t entry, const bool lightweight = false);

  UInt_t fEntriesUsed;
  std::string fInputFile;
  NuHepMC::ReaderRootTree *rdr;
};
