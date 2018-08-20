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

void FullEvent::ClearParticleStack() {
  for (auto &status_stack : ParticleStack) {
    status_stack.particles.clear();
  }
}

} // namespace core
} // namespace nuis
