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

namespace nuis {
namespace utility {

template <size_t N>
std::vector<Particle>
GetParticles(FullEvent const &ev, std::vector<PDG_t> const &pdgs,
             Particle::Status_t status = Particle::Status_t::kNuclearLeaving,
             bool include_matching_pdg = true) {
  std::vector<Particle> selected_particles;
  for (StatusParticles const &parts : ev.ParticleStack) {
    if (parts.status != status) {
      continue;
    }
    for (Particle const &part : parts.particles) {
      bool matched_pdg = false;
      for (pdg : pdgs) {
        matched_pdg = matched_pdg || (part.pdg == pdg);
      }
      bool keep = ((include_matching_pdg && matched_pdg) ||
                   (!include_matching_pdg && !matched_pdg));
      if (!matched_pdg) {
        continue;
      }
      selected_particles.push_back(part);
    }
  }
  return selected_particles;
}

template <size_t N>
Particle
GetHMParticle(FullEvent const &ev, std::vector<PDG_t> const &pdgs,
              Particle::Status_t status = Particle::Status_t::kNuclearLeaving,
              bool include_matching_pdg = true) {
  Particle HMParticle;
  for (StatusParticles const &parts : ev.ParticleStack) {
    if (parts.status != status) {
      continue;
    }
    for (Particle const &part : parts.particles) {
      bool matched_pdg = false;
      for (pdg : pdgs) {
        matched_pdg = matched_pdg || (part.pdg == pdg);
      }
      bool keep = ((include_matching_pdg && matched_pdg) ||
                   (!include_matching_pdg && !matched_pdg));
      if (!matched_pdg) {
        continue;
      }
      if (part.P4.Vect().Mag() > HMParticle.P4.Vect().Mag()) {
        HMParticle = part;
      }
    }
  }
  return HMParticle;
}

std::vector<Particle> GetFSChargedLeptons(FullEvent const &ev) {
  return GetParticles(ev, pdgcodes::ChargedLeptons);
}
std::vector<Particle> GetFSNeutralLeptons(FullEvent const &ev) {
  return GetParticles(ev, pdgcodes::NeutralLeptons);
}
std::vector<Particle> GetISNeutralLeptons(FullEvent const &ev) {
  return GetParticles(ev, pdgcodes::NeutralLeptons,
                      Particle::Status_t::kPrimaryInitialState);
}
std::vector<Particle> GetFSChargedPions(FullEvent const &ev) {
  return GetParticles(ev, pdgcodes::ChargedPions);
}
std::vector<Particle> GetFSNeutralPions(FullEvent const &ev) {
  return GetParticles(ev, pdgcodes::NeutralPions);
}
std::vector<Particle> GetFSPions(FullEvent const &ev) {
  return GetParticles(ev, pdgcodes::Pions);
}
std::vector<Particle> GetFSProtons(FullEvent const &ev) {
  return GetParticles(ev, pdgcodes::Protons);
}
std::vector<Particle> GetFSNeutrons(FullEvent const &ev) {
  return GetParticles(ev, pdgcodes::Neutron);
}
std::vector<Particle> GetFSNucleons(FullEvent const &ev) {
  return GetParticles(ev, pdgcodes::Nucleons);
}
std::vector<Particle> GetFSOthers(FullEvent const &ev) {
  return GetParticles(ev, pdgcodes::CommonParticles,
                      Particle::Status_t::kNuclearLeaving, false);
}

Particle GetFSChargedLepton(FullEvent const &ev) {
  return GetHMParticle(ev, pdgcodes::ChargedLeptons);
}
Particle GetFSNeutralLepton(FullEvent const &ev) {
  return GetHMParticle(ev, pdgcodes::NeutralLeptons);
}
Particle GetISNeutralLepton(FullEvent const &ev) {
  return GetHMParticle(ev, pdgcodes::NeutralLeptons,
                       Particle::Status_t::kPrimaryInitialState);
}
Particle GetFSChargedPion(FullEvent const &ev) {
  return GetHMParticle(ev, pdgcodes::ChargedPions);
}
Particle GetFSNeutralPion(FullEvent const &ev) {
  return GetHMParticle(ev, pdgcodes::NeutralPions);
}
Particle GetFSPion(FullEvent const &ev) {
  return GetHMParticle(ev, pdgcodes::Pions);
}
Particle GetFSProton(FullEvent const &ev) {
  return GetHMParticle(ev, pdgcodes::Protons);
}
Particle GetFSNeutron(FullEvent const &ev) {
  return GetHMParticle(ev, pdgcodes::Neutron);
}
Particle GetFSNucleon(FullEvent const &ev) {
  return GetHMParticle(ev, pdgcodes::Nucleons);
}
Particle GetFSOther(FullEvent const &ev) {
  return GetHMParticle(ev, pdgcodes::CommonParticles,
                       Particle::Status_t::kNuclearLeaving, false);
}

} // namespace utility
} // namespace nuis

#endif
