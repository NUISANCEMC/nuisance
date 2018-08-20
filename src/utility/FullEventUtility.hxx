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

#ifndef UTILITY_FULLEVENTUTILITY_HXX_SEEN
#define UTILITY_FULLEVENTUTILITY_HXX_SEEN

#include "event/types.hxx"
#include "event/Particle.hxx"

#include <vector>

namespace nuis {
namespace event {
class FullEvent;
} // namespace event
} // namespace nuis

namespace nuis {
namespace utility {

std::vector<event::Particle>
GetParticles(event::FullEvent const &, std::vector<event::PDG_t> const &,
             event::Particle::Status_t status =
                 event::Particle::Status_t::kNuclearLeaving,
             bool include_matching_pdg = true);

std::vector<event::Particle> const &GetISParticles(event::FullEvent const &);
std::vector<event::Particle> const &
GetPrimaryFSParticles(event::FullEvent const &);
std::vector<event::Particle> const &
GetNuclearLeavingParticles(event::FullEvent const &);

event::Particle GetHMParticle(event::FullEvent const &,
                              std::vector<event::PDG_t> const &,
                              event::Particle::Status_t status =
                                  event::Particle::Status_t::kNuclearLeaving,
                              bool include_matching_pdg = true);

std::vector<event::Particle> GetFSChargedLeptons(event::FullEvent const &);
std::vector<event::Particle> GetFSNeutralLeptons(event::FullEvent const &);
std::vector<event::Particle> GetISNeutralLeptons(event::FullEvent const &);
std::vector<event::Particle> GetFSChargedPions(event::FullEvent const &);
std::vector<event::Particle> GetFSNeutralPions(event::FullEvent const &);
std::vector<event::Particle> GetFSPions(event::FullEvent const &);
std::vector<event::Particle> GetFSProtons(event::FullEvent const &);
std::vector<event::Particle> GetFSNeutrons(event::FullEvent const &);
std::vector<event::Particle> GetFSNucleons(event::FullEvent const &);
std::vector<event::Particle> GetFSOthers(event::FullEvent const &);

event::Particle GetFSChargedLepton(event::FullEvent const &);
event::Particle GetFSNeutralLepton(event::FullEvent const &);
event::Particle GetISNeutralLepton(event::FullEvent const &);
event::Particle GetFSChargedPion(event::FullEvent const &);
event::Particle GetFSNeutralPion(event::FullEvent const &);
event::Particle GetFSPion(event::FullEvent const &);
event::Particle GetFSProton(event::FullEvent const &);
event::Particle GetFSNeutron(event::FullEvent const &);
event::Particle GetFSNucleon(event::FullEvent const &);
event::Particle GetFSOther(event::FullEvent const &);

} // namespace utility
} // namespace nuis

#endif
