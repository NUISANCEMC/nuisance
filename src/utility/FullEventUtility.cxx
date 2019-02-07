#include "utility/FullEventUtility.hxx"

#include "event/FullEvent.hxx"

#include "utility/PDGCodeUtility.hxx"

using namespace nuis::event;

namespace nuis {
namespace utility {

event::Particle GetHMParticle(std::vector<event::Particle> const &parts) {
  if (parts.size()) {
    return event::Particle();
  }

  return *std::max_element(
      parts.begin(), parts.end(),
      [](event::Particle const &l, event::Particle const &r) {
        return l.P() < r.P();
      });
}

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
      if (part.P() > HMParticle.P()) {
        HMParticle = part;
      }
    }
  }
  return HMParticle;
}

event::Particle GetHMISParticle(event::FullEvent const &ev,
                                std::vector<event::PDG_t> const &pdgs) {
  return GetHMParticle(ev, pdgs, Particle::Status_t::kPrimaryInitialState);
}

event::Particle GetHMFSParticle(event::FullEvent const &ev,
                                std::vector<event::PDG_t> const &pdgs) {
  return GetHMParticle(ev, pdgs);
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

Particle GetHMFSChargedLepton(FullEvent const &ev) {
  return GetHMParticle(ev, pdgcodes::ChargedLeptons);
}
Particle GetHMFSNeutralLepton(FullEvent const &ev) {
  return GetHMParticle(ev, pdgcodes::NeutralLeptons);
}
Particle GetHMISNeutralLepton(FullEvent const &ev) {
  return GetHMParticle(ev, pdgcodes::NeutralLeptons,
                       Particle::Status_t::kPrimaryInitialState);
}
Particle GetHMFSChargedPion(FullEvent const &ev) {
  return GetHMParticle(ev, pdgcodes::ChargedPions);
}
Particle GetHMFSNeutralPion(FullEvent const &ev) {
  return GetHMParticle(ev, pdgcodes::NeutralPions);
}
Particle GetHMFSPion(FullEvent const &ev) {
  return GetHMParticle(ev, pdgcodes::Pions);
}
Particle GetHMFSProton(FullEvent const &ev) {
  return GetHMParticle(ev, pdgcodes::Protons);
}
Particle GetHMFSNeutron(FullEvent const &ev) {
  return GetHMParticle(ev, pdgcodes::Neutron);
}
Particle GetHMFSNucleon(FullEvent const &ev) {
  return GetHMParticle(ev, pdgcodes::Nucleons);
}
Particle GetHMFSOther(FullEvent const &ev) {
  return GetHMParticle(ev, pdgcodes::CommonParticles,
                       Particle::Status_t::kNuclearLeaving, false);
}

} // namespace utility
} // namespace nuis
