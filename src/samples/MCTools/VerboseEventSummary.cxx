#include "samples/ISample.hxx"

#include "event/FullEvent.hxx"

#include "input/InputManager.hxx"

#include <iostream>
#include <limits>

using namespace nuis::event;
using namespace nuis::input;

class VerboseEventSummary : public ISample {
public:
  InputManager::Input_id_t fIH_id;

  VerboseEventSummary()
      : fIH_id(std::numeric_limits<InputManager::Input_id_t>::max()) {}

  void Initialize(fhicl::ParameterSet const &ps) {
    fIH_id = InputManager::Get().EnsureInputLoaded(ps);
  }

  void ProcessEvent(FullEvent const &ps) {
    std::cout << ps.to_string() << std::endl;
  }

  void ProcessSample(size_t nmax) {
    if (fIH_id == std::numeric_limits<InputManager::Input_id_t>::max()) {
      throw uninitialized_ISample();
    }

    IInputHandler const &IH = InputManager::Get().GetInputHandler(fIH_id);
    size_t NEvsToProcess = std::min(nmax, IH.GetNEvents());
    size_t NToShout = NEvsToProcess / 10;
    std::cout << "[INFO]: Processing " << NEvsToProcess << " input events."
              << std::endl;

    size_t n = 0;
    for (auto const &fe : IH) {
      if (++n > nmax) {
        break;
      }
      if (NToShout && !(n % NToShout)) {
        std::cout << "[INFO]: Processed " << n << "/" << NEvsToProcess
                  << " events." << std::endl;
      }
      ProcessEvent(fe);
    }
  }

  void Write() {}
  std::string Name() { return "VerboseEventSummary"; }
};

DECLARE_PLUGIN(ISample, VerboseEventSummary);
