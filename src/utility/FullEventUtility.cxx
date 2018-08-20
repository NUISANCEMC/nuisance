#include "utility/FullEventUtility.hxx"

#include "event/FullEvent.hxx"

#include "utility/PDGCodeUtility.hxx"

using namespace nuis::event;

namespace nuis {
namespace utility {

std::vector<Particle> GetParticles(FullEvent const &ev,
                                   std::vector<PDG_t> const &pdgs,
                                   Particle::Status_t status,
                                   bool include_matching_pdg) {
  std::vector<Particle> selected_particles;
  for (auto const &parts : ev.ParticleStack) {
    if (parts.status != status) {
      continue;
    }
    for (Particle const &part : parts.particles) {
      bool matched_pdg = false;
      for (PDG_t pdg : pdgs) {
        matched_pdg = matched_pdg || (part.pdg == pdg);
      }
      bool keep = ((include_matching_pdg && matched_pdg) ||
                   (!include_matching_pdg && !matched_pdg));
      if (!keep) {
        continue;
      }
      selected_particles.push_back(part);
    }
  }
  return selected_particles;
}

std::vector<Particle> const &GetISParticles(FullEvent const &ev) {
  return ev
      .ParticleStack[static_cast<size_t>(
          Particle::Status_t::kPrimaryInitialState)]
      .particles;
}
std::vector<Particle> const &GetPrimaryFSParticles(FullEvent const &ev) {
  return ev
      .ParticleStack[static_cast<size_t>(
          Particle::Status_t::kPrimaryFinalState)]
      .particles;
}
std::vector<Particle> const &GetNuclearLeavingParticles(FullEvent const &ev) {
  return ev
      .ParticleStack[static_cast<size_t>(Particle::Status_t::kNuclearLeaving)]
      .particles;
}

Particle GetHMParticle(FullEvent const &ev, std::vector<PDG_t> const &pdgs,
                       Particle::Status_t status, bool include_matching_pdg) {
  Particle HMParticle;
  for (auto const &parts : ev.ParticleStack) {
    if (parts.status != status) {
      continue;
    }
    for (Particle const &part : parts.particles) {
      bool matched_pdg = false;
      for (PDG_t pdg : pdgs) {
        matched_pdg = matched_pdg || (part.pdg == pdg);
      }
      bool keep = ((include_matching_pdg && matched_pdg) ||
                   (!include_matching_pdg && !matched_pdg));
      if (!keep) {
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