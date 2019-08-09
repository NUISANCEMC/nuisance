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

#pragma once

#include "event/Particle.hxx"
#include "event/types.hxx"

#include <vector>

namespace nuis {
namespace event {
class FullEvent;
} // namespace event
} // namespace nuis

namespace nuis {
namespace utility {
class IParticlePhaseSpaceRestriction;

event::Particle GetHMParticle(std::vector<event::Particle>);

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

event::Particle GetHMISParticle(event::FullEvent const &,
                                std::vector<event::PDG_t> const &);
event::Particle GetHMFSParticle(event::FullEvent const &,
                                std::vector<event::PDG_t> const &);

///***********Getters that include copies

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

event::Particle GetHMFSChargedLepton(event::FullEvent const &);
event::Particle GetHMFSNeutralLepton(event::FullEvent const &);
event::Particle GetHMFSLepton(event::FullEvent const &);
event::Particle GetHMISChargedLepton(event::FullEvent const &);
event::Particle GetHMISNeutralLepton(event::FullEvent const &);
event::Particle GetHMISLepton(event::FullEvent const &);
event::Particle GetHMFSChargedPion(event::FullEvent const &);
event::Particle GetHMFSNeutralPion(event::FullEvent const &);
event::Particle GetHMFSPion(event::FullEvent const &);
event::Particle GetHMFSProton(event::FullEvent const &);
event::Particle GetHMFSNeutron(event::FullEvent const &);
event::Particle GetHMFSNucleon(event::FullEvent const &);
event::Particle GetHMFSOther(event::FullEvent const &);

event::Particle
GetHMParticleInPhaseSpace(event::FullEvent const &ev,
                          std::vector<event::PDG_t> const &pdgs,
                          IParticlePhaseSpaceRestriction const &ps,
                          event::Particle::Status_t status =
                              event::Particle::Status_t::kNuclearLeaving,
                          bool include_matching_pdg = true);
event::Particle
GetHMFSProtonInPhaseSpace(event::FullEvent const &ev,
                          IParticlePhaseSpaceRestriction const &ps);

///***********Counters requiring no copies
size_t GetNParticles(event::FullEvent const &ev,
                     std::vector<event::PDG_t> const &pdgs,
                     event::Particle::Status_t status =
                         event::Particle::Status_t::kNuclearLeaving,
                     bool include_matching_pdg = true);
size_t GetNParticlesInPhaseSpace(event::FullEvent const &ev,
                                 std::vector<event::PDG_t> const &pdgs,
                                 IParticlePhaseSpaceRestriction const &ps,
                                 event::Particle::Status_t status =
                                     event::Particle::Status_t::kNuclearLeaving,
                                 bool include_matching_pdg = true);

size_t GetNFSChargedLeptons(event::FullEvent const &ev);
size_t GetNFSNeutralLeptons(event::FullEvent const &ev);
size_t GetNFSLeptons(event::FullEvent const &ev);
size_t GetNISNeutralLeptons(event::FullEvent const &ev);
size_t GetNISElectrons(event::FullEvent const &ev);
size_t GetNFSChargedPions(event::FullEvent const &ev);
size_t GetNFSNeutralPions(event::FullEvent const &ev);
size_t GetNFSPions(event::FullEvent const &ev);
size_t GetNFSProtons(event::FullEvent const &ev);
size_t GetNFSNeutrons(event::FullEvent const &ev);
size_t GetNFSNucleons(event::FullEvent const &ev);
size_t GetNFSOthers(event::FullEvent const &ev);

} // namespace utility
} // namespace nuis