#include "generator/GeneratorManager.hxx"

#include <iomanip>

namespace nuis {

GeneratorManager *GeneratorManager::_global_inst = nullptr;

GeneratorManager::GeneratorManager() : Generators() {}

GeneratorManager &GeneratorManager::Get() {
  if (!_global_inst) {
    _global_inst = new GeneratorManager();
  }
  return *_global_inst;
}

GeneratorManager::Generator_id_t
GeneratorManager::EnsureGeneratorRegistered(std::string const &gname) {
  for (size_t i = 0; i < Generators.size(); ++i) {
    if (Generators[i] == gname) {
      return i;
    }
  }

  Generator_id_t iid = Generators.size();
  Generators.push_back(gname);
  return iid;
}
GeneratorManager::Generator_id_t
GeneratorManager::GetGeneratorId(std::string const &gname) const {
  for (size_t i = 0; i < Generators.size(); ++i) {
    if (Generators[i] == gname) {
      return i;
    }
  }
  throw unknown_generator()
      << "[ERROR]: Generator " << std::quoted(gname) << " has not been loaded.";
}
bool GeneratorManager::AreCompatible(Generator_id_t g1, Generator_id_t g2) {
  if ((g1 == GeneratorManager::kAll) || (g2 == GeneratorManager::kAll)) {
    return true;
  }
  return (g1 == g2);
}
} // namespace nuis
