#include "utility/experimental/T2KUtility.hxx"

#include "event/FullEvent.hxx"

#include "utility/EventTopologyUtility.hxx"
#include "utility/FullEventUtility.hxx"
#include "utility/PhaseSpaceRestriction.hxx"

namespace nuis {
namespace utility {
namespace t2k {
std::pair<bool, size_t> IsCC0Pi_NumProtons(event::FullEvent const &ev) {
  if (!nuis::utility::IsCC0Pi(ev)) {
    return {false, 0};
  }

  static SimpleParticlePhaseSpaceRestriction prot_mom_cut =
      SimpleParticlePhaseSpaceRestriction::Momentum(500);

  return {true,
          GetNParticlesInPhaseSpace(ev, {pdgcodes::kProton}, prot_mom_cut)};
}

bool IsCC0PiNp(event::FullEvent const &ev) {
  auto res = IsCC0Pi_NumProtons(ev);
  return res.first && (res.second > 1);
}
bool IsCC0Pi1p(event::FullEvent const &ev) {
  auto res = IsCC0Pi_NumProtons(ev);
  return res.first && (res.second == 1);
}
bool IsCC0Pi0p(event::FullEvent const &ev) {
  auto res = IsCC0Pi_NumProtons(ev);
  return res.first && (res.second == 0);
}
bool IsCC0Pi_STV(event::FullEvent const &ev) {
  if (!nuis::utility::IsCC0Pi(ev)) {
    return false;
  }

  //! Cut on kinematic properties of the true final state.
  static SimpleParticlePhaseSpaceRestriction muon_PS =
      SimpleParticlePhaseSpaceRestriction::MomentumCosTheta(
          {250, std::numeric_limits<double>::max()}, {-0.6, 1});
  // Muon phase space
  // Pmu > 250 MeV, cos(theta_mu) > -0.6 (Sweet phase space!)
  if (GetNParticlesInPhaseSpace(ev, {pdgcodes::kMu}, muon_PS) != 1) {
    return false;
  }

  static SimpleParticlePhaseSpaceRestriction prot_PS =
      SimpleParticlePhaseSpaceRestriction::MomentumCosTheta({450, 1E3},
                                                            {0.4, 1});

  // Proton phase space
  if (GetNParticlesInPhaseSpace(ev, {pdgcodes::kProton}, prot_PS) < 1) {
    return false;
  }

  return true;
}

bool IsCC1Pip_CH_MichTag(event::FullEvent const &ev) {
  if (!nuis::utility::IsCC1Pi(ev, {pdgcodes::kPiPlus})) {
    return false;
  }

  static SimpleParticlePhaseSpaceRestriction muon_ps =
      SimpleParticlePhaseSpaceRestriction::MomentumCosTheta(
          {200, std::numeric_limits<double>::max()}, {0.2, 1});

  if (GetNParticlesInPhaseSpace(ev, {pdgcodes::kMu}, muon_ps) != 1) {
    return false;
  }
  return true;
}

bool IsCC1Pip_CH_RecPi(event::FullEvent const &ev) {
  if (!IsCC1Pip_CH_MichTag(ev)) {
    return false;
  }

  static SimpleParticlePhaseSpaceRestriction pi_ps =
      SimpleParticlePhaseSpaceRestriction::MomentumCosTheta(
          {200, std::numeric_limits<double>::max()}, {0.2, 1});

  if (GetNParticlesInPhaseSpace(ev, {pdgcodes::kPiPlus}, pi_ps) != 1) {
    return false;
  }
  return true;
}

bool IsCC1Pip_H20(event::FullEvent const &ev) {
  if (!nuis::utility::IsCC1Pi(ev, {pdgcodes::kPiPlus})) {
    return false;
  }

  static SimpleParticlePhaseSpaceRestriction muon_pi_ps =
      SimpleParticlePhaseSpaceRestriction::MomentumCosTheta(
          {200, std::numeric_limits<double>::max()}, {0.3, 1});

  if (GetNParticlesInPhaseSpace(ev, {pdgcodes::kMu}, muon_pi_ps) != 1) {
    return false;
  }
  if (GetNParticlesInPhaseSpace(ev, {pdgcodes::kPiPlus}, muon_pi_ps) != 1) {
    return false;
  }
  return true;
}

} // namespace t2k
} // namespace utility
} // namespace nuis
