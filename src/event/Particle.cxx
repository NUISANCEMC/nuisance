#include "event/Particle.hxx"

#include "TLorentzVector.h"

#include <limits>

namespace nuis {
namespace event {
Particle::Particle() : pdg(std::numeric_limits<PDG_t>::max()), P4(0, 0, 0, 0) {}
Particle::Particle(Particle const &other) : pdg(other.pdg), P4(other.P4) {}
} // namespace core
} // namespace nuis
