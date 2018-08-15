#include "core/FullEvent.hxx"

namespace nuis {
namespace core {
FullEvent::FullEvent() : MinimalEvent() {
  ParticleStack.resize(static_cast<size_t>(Particle::Status_t::kNParticleStatus));
}
FullEvent::FullEvent(FullEvent &&other)
    : MinimalEvent(std::move(other)),
      ParticleStack(std::move(other.ParticleStack)) {}

} // namespace core
} // namespace nuis
