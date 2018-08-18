#include "core/InputManager.hxx"

#include "plugins/Instantiate.hxx"

#include "fhiclcpp/ParameterSet.h"

#include <iomanip>

namespace nuis {
namespace core {

InputManager *InputManager::_global_inst = nullptr;

InputManager::NamedInputHandler::NamedInputHandler(
    std::string const &file,
    plugins::plugin_traits<IInputHandler>::unique_ptr_t &&IH) {
  name = file;
  handler = std::move(IH);
}

InputManager::InputManager() : Inputs() {}

InputManager &InputManager::Get() {
  if (!_global_inst) {
    _global_inst = new InputManager();
  }
  return *_global_inst;
}

InputManager::Input_id_t
InputManager::EnsureInputLoaded(fhicl::ParameterSet const &ps) {
  std::string const &file_name = ps.get<std::string>("file");
  for (size_t i = 0; i < Inputs.size(); ++i) {
    if (Inputs[i].name == file_name) {
      return i;
    }
  }

  Inputs.emplace_back(file_name,
                      nuis::plugins::Instantiate<IInputHandler>(
                          ps.get<std::string>("input_type") + "InputHandler"));
  Inputs.back().handler->Initialize(ps);
  return (Inputs.size() - 1);
}
InputManager::Input_id_t
InputManager::GetInputId(std::string const &file_name) const {
  for (size_t i = 0; i < Inputs.size(); ++i) {
    if (Inputs[i].name == file_name) {
      return i;
    }
  }
  throw unknown_input() << "[ERROR]: Input file " << std::quoted(file_name)
                        << " has not been loaded.";
}
IInputHandler const &
InputManager::GetInputHandler(InputManager::Input_id_t id) const {
  if (id >= Inputs.size()) {
    throw unknown_input() << "[ERROR]: Attempted to get input with id " << id
                          << ", but only have " << Inputs.size()
                          << " loaded inputs.";
  }
  return *Inputs[id].handler.get();
}
} // namespace core
} // namespace nuis
