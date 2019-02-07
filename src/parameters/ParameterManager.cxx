#include "parameters/ParameterManager.hxx"

#include "fhiclcpp/ParameterSet.h"

namespace nuis {
namespace params {

ParameterManager::NamedParameter::NamedParameter()
    : name(""), type(""), value(kDefaultValue), start(kDefaultValue),
      min(kDefaultValue), max(kDefaultValue), step(kDefaultValue),
      Penalty([](double) -> double { return 0; }) {}

ParameterManager::NamedParameter::NamedParameter(NamedParameter &&other)
    : name(std::move(other.name)), type(std::move(other.type)),
      value(other.value), start(other.start), min(other.min), max(other.max),
      step(other.step), Penalty(std::move(other.Penalty)) {}

ParameterManager *ParameterManager::_global_inst = nullptr;

ParameterManager::ParameterManager() : locked(false) {}

ParameterManager &ParameterManager::Get() {
  if (!_global_inst) {
    _global_inst = new ParameterManager();
  }
  return *_global_inst;
}

void ParameterManager::ValidateParamId(paramId_t pid) {
  if (pid >= Parameters.size()) {
    throw invalid_parameter_id()
        << "[ERROR]: Passed parameter id " << pid
        << ", but the ParameterManager only knows about " << Parameters.size()
        << " parameters.";
  }
}

void ParameterManager::LockParameterList() { locked = true; }
void ParameterManager::UnlockParameterList() { locked = false; }

paramId_t
ParameterManager::EnsureParameterRegistered(fhicl::ParameterSet const &ps) {

  if (locked) {
    throw parameter_list_is_locked()
        << "[ERROR]: Attempted to register parameter: " << ps.to_string()
        << " when global ParameterManager was locked in state: "
        << StateString();
  }

  NamedParameter np;
  np.name = ps.get<std::string>("name");
  np.type = ps.get<std::string>("type");
  paramId_t pid = GetParameterId(np.name, np.type);
  if (pid != kParamUnhandled) {
    return pid;
  }

  np.start = ps.get<double>("start");
  np.value = np.start;
  np.min = ps.get<double>("min", kDefaultLimit);
  np.max = ps.get<double>("max", kDefaultLimit);
  np.step = ps.get<double>("step");

  pid = Parameters.size();
  Parameters.emplace_back(std::move(np));

  return pid;
}
paramId_t ParameterManager::GetParameterId(std::string const &name,
                                           std::string const &type) {
  paramId_t pid = kParamUnhandled;
  for (size_t p_it = 0; p_it < Parameters.size(); ++p_it) {
    if (name != Parameters[p_it].name) {
      continue;
    }
    if (type.size() && (type != Parameters[p_it].type)) {
      continue;
    }
    // matches search, check if it is the first to match the search.
    if (pid != kParamUnhandled) {
      throw ambiguous_parameter_specified()
          << "[ERROR]: When searching for parameter by name-only, found at "
             "least two matching parameters: { PID: "
          << pid << ", name: " << Parameters[pid].name
          << ", type: " << Parameters[pid].type << " } and { PID: " << p_it
          << ", name: " << Parameters[p_it].name
          << ", type: " << Parameters[p_it].type << " }";
    }
    pid = p_it;
  }
  return pid;
}
void ParameterManager::SetParameterValue(paramId_t pid, double val) {
  ValidateParamId(pid);

  if (!IsValidParameterValue(pid, val)) {
    throw param_value_out_of_bounds()
        << "[ERROR]: Attempting to set parameter  { PID: " << pid
        << ", name: " << Parameters[pid].name
        << ", type: " << Parameters[pid].type << " } to " << val
        << ", but this is out of the allowed range ["
        << ((Parameters[pid].min == kDefaultLimit)
                ? "unbounded"
                : std::to_string(Parameters[pid].min))
        << ","
        << ((Parameters[pid].max == kDefaultLimit)
                ? "unbounded"
                : std::to_string(Parameters[pid].max))
        << "]";
  }

  Parameters[pid].value = val;
}
double ParameterManager::GetParameterValue(paramId_t pid) {
  ValidateParamId(pid);
  return Parameters[pid].value;
}
double ParameterManager::GetParameterStep(paramId_t pid) {
  ValidateParamId(pid);
  return Parameters[pid].step;
}
double ParameterManager::GetParameterStart(paramId_t pid) {
  ValidateParamId(pid);
  return Parameters[pid].start;
}
double ParameterManager::GetParameterMin(paramId_t pid) {
  ValidateParamId(pid);
  return Parameters[pid].min;
}
double ParameterManager::GetParameterMax(paramId_t pid) {
  ValidateParamId(pid);
  return Parameters[pid].max;
}
bool ParameterManager::IsValidParameterValue(paramId_t pid, double val) {
  ValidateParamId(pid);
  return (
      ((Parameters[pid].min == kDefaultLimit) || (Parameters[pid].min < val)) &&
      ((Parameters[pid].max == kDefaultLimit) || (Parameters[pid].max > val)));
}

std::string ParameterManager::StateString() {
  return "Parameter Manager state:";
}
} // namespace params
} // namespace nuis
