#include "core/Particle.hxx"

#include "TLorentzVector.h"

namespace nuis {
namespace core {
Particle::Particle() : pdg(0), status(Status_t::kUnknown), P4(0, 0, 0, 0) {}
Particle::Particle(Particle const &other)
    : pdg(other.pdg), status(other.status), P4(other.P4) {}
} // namespace core
} // namespace nuis
