#include "core/InputManager.hxx"

namespace nuis {
namespace core {

InputManager::Input_id_t
InputManager::EnsureInputLoaded(std::string const &file_name) {
  for (size_t i = 0; i < Inputs.size(); ++i) {
    if (Inputs[i].name == file_name) {
      return i;
    }
  }

  Inputs.push_back(NamedInputHandler{
      file_name,
  });
}
InputManager::Input_id_t
InputManager::GetInputId(std::string const &file_name) {}
IInputHandler &InputManager::GetInputHandler(InputManager::Input_id_t id) {}
} // namespace core
} // namespace nuis
