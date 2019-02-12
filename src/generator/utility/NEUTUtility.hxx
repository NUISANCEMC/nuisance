#pragma once

#include "event/Particle.hxx"
#include "event/types.hxx"

#include "exception/exception.hxx"

#include <memory>

class NeutPart;

namespace nuis {
namespace neuttools {
nuis::event::Particle::Status_t GetNeutParticleStatus(NeutPart const &,
                                                      nuis::event::Channel_t);
}
} // namespace nuis
