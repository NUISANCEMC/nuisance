#include "InputHandler.h"

struct ConstructibleInputHandler : public InputHandlerBase {
  std::vector<FitEvent *> FitEvents;

  ConstructibleInputHandler(
      std::string const& name) {
    fName = name;
  }

  void AddFitEvent(FitEvent *fe) {
    FitEvents.push_back(fe);

    fNEvents = FitEvents.size();

    std::cout << "[INFO]: Added event " << std::endl;
    fe->Print();
  }

  FitEvent* GetNuisanceEvent(
      const UInt_t entry, const bool lightweight) {
    if (entry >= (UInt_t)fNEvents) return NULL;
    return FitEvents[entry];
  }
};
