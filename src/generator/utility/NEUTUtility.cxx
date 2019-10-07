#include "generator/utility/NEUTUtility.hxx"

#include "generator/input/NEUTInputHandler.hxx"

#include "exception/exception.hxx"

#include "utility/InteractionChannelUtility.hxx"
#include "utility/PDGCodeUtility.hxx"

#include "neutpart.h"
#include "neutvect.h"

using namespace nuis::event;
using namespace nuis::utility;

namespace nuis {
namespace neuttools {
NEW_NUIS_EXCEPT(unexpected_NEUT_particle_state);

// #define DEBUG_NEUT_UTILITY

Particle::Status_t GetNeutParticleStatus(NeutPart const &part, Channel_t mode) {

#ifdef DEBUG_NEUT_UTILITY
  std::cout << "[DEBUG]: Mode: " << mode << ", Part: { Status: " << part.fStatus
            << ", IsAlive: " << part.fIsAlive << ", PDG: " << part.fPID << " }."
            << std::endl;
#endif

  // Remove Pauli blocked events, probably just single pion events
  if (part.fStatus == 5) {
    return Particle::Status_t::kBlocked;

    // fStatus == -1 means initial  state
  } else if (part.fIsAlive == false && part.fStatus == -1) {
    return Particle::Status_t::kPrimaryInitialState;

    // NEUT has a bit of a strange convention for fIsAlive and fStatus
    // combinations
    // for NC and neutrino particle isAlive true/false and status 2 means
    // final state particle
    // for other particles in NC status 2 means it's an FSI particle
    // for CC it means it was an FSI particle
  } else if (part.fStatus == 2) {
    // NC case is a little strange... The outgoing neutrino might be alive or
    // not alive. Remaining particles with status 2 are FSI particles that
    // reinteracted
    if (IsNC(mode) && IsNeutralLepton(part.fPID)) {
      return Particle::Status_t::kNuclearLeaving;
      // The usual CC case
    } else if (part.fIsAlive == true) {
      // return Particle::Status_t::kIntermediate;
      throw unexpected_NEUT_particle_state()
          << "[ERROR] Found unexpected NEUT particle in neutvect stack: Mode: "
          << mode << " (IsNC: " << IsNC(mode)
          << "), Part: { Status: " << part.fStatus
          << ", IsAlive: " << part.fIsAlive << ", PDG: " << part.fPID
          << ", IsNeutralLepton: " << IsNeutralLepton(part.fPID) << " }.";
    }

  } else if ((part.fIsAlive == true) && (part.fStatus == 2) &&
             IsNeutralLepton(part.fPID)) {
    return Particle::Status_t::kNuclearLeaving;

  } else if ((part.fIsAlive == true) && (part.fStatus == 0)) {
    return Particle::Status_t::kNuclearLeaving;
  } else if (!part.fIsAlive && ((part.fStatus == 1) || (part.fStatus == 3) ||
                                (part.fStatus == 4) || (part.fStatus == 7) ||
                                (part.fStatus == 8))) {
    return Particle::Status_t::kIntermediate;

    // There's one hyper weird case where fStatus = -3. This apparently
    // corresponds to a nucleon being ejected via pion FSI when there is "data
    // available"
  } else if (!part.fIsAlive && (part.fStatus == -3)) {
    return Particle::Status_t::kUnknown;
    // NC neutrino outgoing
  } else if (!part.fIsAlive && part.fStatus == 0 &&
             IsNeutralLepton(part.fPID)) {
    return Particle::Status_t::kNuclearLeaving;

    // Warn if we still find alive particles without classifying them
  } else if (part.fIsAlive == true) {
    std::cout << "[WARN]: Undefined NEUT state "
              << " Alive: " << part.fIsAlive << " Status: " << part.fStatus
              << " PDG: " << part.fPID << std::endl;
    return Particle::Status_t::kUnknown;
  }
  // Warn if we find dead particles that we haven't classified
  std::cout << "[WARN]: Undefined NEUT state "
            << " Alive: " << part.fIsAlive << " Status: " << part.fStatus
            << " PDG: " << part.fPID << std::endl;
  return Particle::Status_t::kUnknown;
}

} // namespace neuttools
} // namespace nuis
