#include "variation/WeightManager.hxx"

#include "fhiclcpp/ParameterSet.h"

#include "plugins/Instantiate.hxx"

namespace nuis {
namespace variation {

WeightManager::NamedWeightProvider::NamedWeightProvider(
    std::string const &_name,
    plugins::plugin_traits<IWeightProvider>::unique_ptr_t &&_handler)
    : name(_name), handler(std::move(_handler)) {}

WeightManager::NamedWeightProvider::NamedWeightProvider(
    NamedWeightProvider &&other)
    : name(std::move(other.name)), handler(std::move(other.handler)) {}

WeightManager *WeightManager::_global_inst = nullptr;

WeightManager::WeightManager() {}

WeightManager &WeightManager::Get() {
  if (!_global_inst) {
    _global_inst = new WeightManager();
  }
  return *_global_inst;
}

WeightManager::WeightProv_id_t
WeightManager::EnsureWeightProviderLoaded(fhicl::ParameterSet const &ps) {
  std::string const &prov_name = ps.get<std::string>("name");
  for (size_t i = 0; i < WeightEngines.size(); ++i) {
    if (WeightEngines[i].name == prov_name) {
      return i;
    }
  }

  WeightProv_id_t wid = WeightEngines.size();
  WeightEngines.emplace_back(
      prov_name, nuis::plugins::Instantiate<IWeightProvider>(prov_name));
  WeightEngines.back().handler->Initialize(ps);

  return wid;
}
double WeightManager::GetEventWeight(nuis::event::MinimalEvent const & me) {
  double w = 1;
  for (auto &we : WeightEngines) {
    w *= we.handler->GetEventWeight(me);
  }
  return w;
}

void WeightManager::ReconfigureWeightEngines() {
  for (auto &we : WeightEngines) {
    we.handler->Reconfigure();
  }
}
} // namespace variation
} // namespace nuis
