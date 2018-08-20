#ifndef GENERATOR_UTILITY_NEUTUTILITY_HXX_SEEN
#define GENERATOR_UTILITY_NEUTUTILITY_HXX_SEEN

#include "event/Particle.hxx"
#include "event/types.hxx"

class NeutPart;

nuis::event::Particle::Status_t
GetNeutParticleStatus(NeutPart const &, nuis::event::Channel_t);
#endif