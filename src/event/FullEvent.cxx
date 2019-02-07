#include "event/FullEvent.hxx"

namespace nuis {
namespace event {
FullEvent::FullEvent() : MinimalEvent() {

  for (size_t status_it = 0;
       status_it < static_cast<size_t>(Particle::Status_t::kNParticleStatus);
       ++status_it) {
    ParticleStack.push_back({static_cast<Particle::Status_t>(status_it), {}});
  }
}
FullEvent::FullEvent(FullEvent &&other)
    : MinimalEvent(std::move(other)),
      ParticleStack(std::move(other.ParticleStack)) {}

FullEvent &FullEvent::operator=(FullEvent &&other) {
  MinimalEvent::operator=(std::move(other));
  ParticleStack = std::move(other.ParticleStack);
  return *this;
}

FullEvent FullEvent::Clone() const {
  FullEvent clone;
  clone.MinimalEvent::operator=(MinimalEvent::Clone());
  clone.ParticleStack = ParticleStack;
  return clone;
}

void FullEvent::ClearParticleStack() {
  for (auto &status_stack : ParticleStack) {
    status_stack.particles.clear();
  }
}

std::string FullEvent::to_string() const {
  std::stringstream ss("");
  ss << "Event: Interaction mode = " << mode << ", probe: { PDG: " << probe_pdg
     << ", Energy: " << probe_E << " MeV }." << std::endl;
  for (auto &status_stack : ParticleStack) {
    ss << "\t[" << status_stack.status << "]" << std::endl;

    for (Particle const &part : status_stack.particles) {
      ss << "\t\t{ PDG: " << part.pdg << ", P3: [ " << part.P4[0] << ", "
         << part.P4[1] << ", " << part.P4[2] << "], E: " << part.P4[3]
         << ", M: " << part.P4.M() << " }" << std::endl;
    }
  }
  ss << std::endl;
  return ss.str();
}

} // namespace event
} // namespace nuis
