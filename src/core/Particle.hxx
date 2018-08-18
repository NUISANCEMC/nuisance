// Copyright 2018 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
 *    This file is part of NUISANCE.
 *
 *    NUISANCE is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    NUISANCE is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with NUISANCE.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/

#ifndef CORE_PARTICLE_HXX_SEEN
#define CORE_PARTICLE_HXX_SEEN

#include "core/types.hxx"

#include "TLorentzVector.h"

namespace nuis {
namespace core {
class Particle {
public:
#define STATUS_LIST                                                            \
  X(kNuclearLeaving, 0)                                                        \
  X(kPrimaryInitialState, 1)                                                   \
  X(kPrimaryFinalState, 2)                                                     \
  X(kIntermediate, 3)                                                          \
  X(kUnknown, 4)                                                               \
  X(kBlocked, 5)                                                               \
  X(kNParticleStatus, 6)

#define X(A, B) A = B,
  enum class Status_t { STATUS_LIST };
#undef X

  Particle();
  Particle(Particle const &);

  PDG_t pdg;
  Status_t status;
  TLorentzVector P4;
};
} // namespace core
} // namespace nuis

#define X(A, B)                                                                \
  case nuis::core::Particle::Status_t::A: {                                    \
    return os << #A;                                                           \
  }

inline std::ostream &operator<<(std::ostream &os,
                                nuis::core::Particle::Status_t te) {
  switch (te) { STATUS_LIST }
  return os;
}
#undef X
#undef STATUS_LIST

#endif
