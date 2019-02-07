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

#ifndef EVENT_PARTICLE_HXX_SEEN
#define EVENT_PARTICLE_HXX_SEEN

#include "event/types.hxx"

#include "TLorentzVector.h"

namespace nuis {
namespace event {
class Particle {
public:
  NEW_NUIS_EXCEPT(invalid_particle);

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
  TLorentzVector P4;

  bool operator!() const { return (pdg == std::numeric_limits<PDG_t>::max()); }

  double E() const { return P4.E(); }
  double P() const { return P4.Vect().Mag(); }
  TVector3 P3() const { return P4.Vect(); }
  double M() const { return P4.M(); }
  double Theta() const { return P4.Vect().Theta(); }
  double CosTheta() const { return P4.Vect().CosTheta(); }
};
} // namespace event
} // namespace nuis

#define X(A, B)                                                                \
  case nuis::event::Particle::Status_t::A: {                                   \
    return os << #A;                                                           \
  }

inline std::ostream &operator<<(std::ostream &os,
                                nuis::event::Particle::Status_t te) {
  switch (te) { STATUS_LIST }
  return os;
}
#undef X
#undef STATUS_LIST

#endif
