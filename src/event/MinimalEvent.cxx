#include "event/MinimalEvent.hxx"

namespace nuis {
namespace event {
MinimalEvent::MinimalEvent()
    : mode(Channel_t::kUndefined), probe_E(0), probe_pdg(0), XSecWeight(1),
      RWWeight(1) {
#ifdef __NUWRO_ENABLED__
  fNuWroEvent = nullptr;
#endif
}

MinimalEvent::MinimalEvent(MinimalEvent &&other)
    : mode(other.mode), probe_E(other.probe_E), probe_pdg(other.probe_pdg),
      XSecWeight(other.XSecWeight), RWWeight(other.RWWeight) {
#ifdef __NUWRO_ENABLED__
  fNuWroEvent = other.fNuWroEvent;
  other.fNuWroEvent = nullptr;
#endif
}
} // namespace core
} // namespace nuis
