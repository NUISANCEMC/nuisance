#include "samples/ISample.hxx"

#include "core/FullEvent.hxx"
#include "core/InputManager.hxx"

#include <iostream>
#include <limits>

using namespace nuis::core;

class VerboseEventSummary : public ISample {
public:
  InputManager::Input_id_t fIH_id;

  VerboseEventSummary()
      : fIH_id(std::numeric_limits<InputManager::Input_id_t>::max()) {}

  void Initialize(fhicl::ParameterSet const &ps) {
    fIH_id = InputManager::Get().EnsureInputLoaded(ps);
  }

  void ProcessEvent(FullEvent const &ps) {
    std::cout << "Event: Interaction mode = " << ps.mode
              << ", probe: { PDG: " << ps.probe_pdg
              << ", Energy: " << ps.probe_E << "}." << std::endl;
    for (auto &status_stack : ps.ParticleStack) {
      std::cout << "\t[" << status_stack.status << "]" << std::endl;

      for (nuis::core::Particle const &part : status_stack.particles) {
        std::cout << "\t\t{ PDG: " << part.pdg << ", P3: [ " << part.P4[0]
                  << ", " << part.P4[1] << ", " << part.P4[2]
                  << "], E: " << part.P4[3] << ", M: " << part.P4.M()
                  << std::endl
                  << std::endl;
      }
    }
  }

  void ProcessSample(size_t nmax) {
    if (fIH_id == std::numeric_limits<InputManager::Input_id_t>::max()) {
      throw uninitialized_ISample();
    }

    IInputHandler const &IH = InputManager::Get().GetInputHandler(fIH_id);
    size_t n = 0;
    for (auto const &fe : IH) {
      if (++n > nmax) {
        break;
      }
      ProcessEvent(fe);
    }
  }

  void Write() {}
  std::string Name() { return "VerboseEventSummary"; }
};

DECLARE_PLUGIN(ISample, VerboseEventSummary);
