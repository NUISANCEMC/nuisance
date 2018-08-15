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
  enum class Status_t {
    kNuclearLeaving = 0,
    kPrimaryInitialState,
    kPrimaryFinalState,
    kIntermediate,
    kUnknown,
    kNParticleStatus
  };
  Particle();
  Particle(Particle const &);

  PDG_t pdg;
  Status_t status;
  TLorentzVector P4;
};
} // namespace core
} // namespace nuis
#endif
