#include "samples/ISample.hxx"

#include "core/InputManager.hxx"
#include "core/FullEvent.hxx"

#include <limits>
#include <iostream>

using namespace nuis::core;

class NuisToNuWro : public ISample {
public:
  InputManager::Input_id_t fIH_id;

  NuisToNuWro()
      : fIH_id(std::numeric_limits<InputManager::Input_id_t>::max()) {}

  void Initialize(fhicl::ParameterSet const &ps) {
    fIH_id = InputManager::Get().EnsureInputLoaded(ps);
  }

  void Process(FullEvent const &ps) {
    std::cout << ps.fNuWroEvent->dyn << std::endl;
  }

  void ProcessSample() {
    if (fIH_id == std::numeric_limits<InputManager::Input_id_t>::max()) {
      throw uninitialized_ISample();
    }

    IInputHandler const &IH = InputManager::Get().GetInputHandler(fIH_id);
    for (auto const &fe : IH) {
      Process(fe);
    }
  }

  void Write() {}
};

DECLARE_PLUGIN(ISample, NuisToNuWro);
