#include "utility/experimental/MINERvAUtility.hxx"

#include "event/FullEvent.hxx"

#include "utility/EventTopologyUtility.hxx"
#include "utility/FullEventUtility.hxx"
#include "utility/KinematicUtility.hxx"

using namespace nuis::event;

namespace nuis {
namespace utility {
namespace mnv {

SimpleParticlePhaseSpaceRestriction CC0PiNProt_ProtonPS =
    SimpleParticlePhaseSpaceRestriction::MomentumTheta_deg({450, 1200}, {0, 70});

bool IsCCIncLowRecoil(event::FullEvent const &ev) {
  if (!nuis::utility::IsCCInc(ev)) {
    return false;
  }

  static SimpleParticlePhaseSpaceRestriction muon_ps =
      SimpleParticlePhaseSpaceRestriction::EnergyTheta_deg(
          {1500, std::numeric_limits<double>::max()}, {0, 20});

  if (GetNParticlesInPhaseSpace(ev, {pdgcodes::kMu}, muon_ps) != 1) {
    return false;
  }

  return true;
}

std::pair<bool, size_t> IsCC0Pi_NumProtons(event::FullEvent const &ev) {
  if (!nuis::utility::IsCC0Pi(ev)) {
    return {false, 0};
  }

  static SimpleParticlePhaseSpaceRestriction proton_ps =
      SimpleParticlePhaseSpaceRestriction::Momentum(450);

  return {true, GetNParticlesInPhaseSpace(ev, {pdgcodes::kProton}, proton_ps)};
}

bool IsCC0PiNp(event::FullEvent const &ev) {
  auto res = IsCC0Pi_NumProtons(ev);
  return res.first && (res.second >= 1);
}

bool IsCC0PiNp_STV(event::FullEvent const &ev) {
  if (!nuis::utility::IsCC0Pi(ev)) {
    return false;
  }

  static SimpleParticlePhaseSpaceRestriction muon_ps =
      SimpleParticlePhaseSpaceRestriction::EnergyTheta_deg({1500, 10E3},
                                                           {0, 20});

  if (GetNParticlesInPhaseSpace(ev, {pdgcodes::kMu}, muon_ps) != 1) {
    return false;
  }

  if (GetNParticlesInPhaseSpace(ev, {pdgcodes::kProton}, CC0PiNProt_ProtonPS) ==
      0) {
    return false;
  }

  return true;
}

bool IsCC1Pi0_2016(event::FullEvent const &ev) {
  if (!nuis::utility::IsCC1Pi(ev, {pdgcodes::kPi0})) {
    return false;
  }
  return true;
}
bool IsCC1CPi_2017(event::FullEvent const &ev) {
  if (!nuis::utility::IsCC1Pi(ev, {pdgcodes::kPiPlus, pdgcodes::kPiMinus})) {
    return false;
  }

  if (nuis::utility::GetNeutrinoWRec(ev) > 1400) {
    return false;
  }

  return true;
}

TVector3 GetDeltaPT_CC0PiN_mnv(event::FullEvent const &fev) {

  Particle ISSLep = GetHMISParticle(fev, {pdgcodes::kNuMu});
  if (!ISSLep) {
    return {-std::numeric_limits<double>::max(),
            -std::numeric_limits<double>::max(),
            -std::numeric_limits<double>::max()};
  }
  Particle FSLep = GetHMFSParticle(fev, {pdgcodes::kMu});
  if (!FSLep) {
    return {-std::numeric_limits<double>::max(),
            -std::numeric_limits<double>::max(),
            -std::numeric_limits<double>::max()};
  }

  Particle FSNuc = GetHMFSProtonInPhaseSpace(fev, CC0PiNProt_ProtonPS);
  if (!FSNuc) {
    return {-std::numeric_limits<double>::max(),
            -std::numeric_limits<double>::max(),
            -std::numeric_limits<double>::max()};
  }

  return GetDeltaPT(FSLep.P3(), FSNuc.P3(), ISSLep.P3());
}
double GetDeltaPhiT_CC0PiN_mnv(event::FullEvent const &fev) {

  Particle ISSLep = GetHMISParticle(fev, {pdgcodes::kNuMu});
  if (!ISSLep) {
    return -std::numeric_limits<double>::max();
  }
  Particle FSLep = GetHMFSParticle(fev, {pdgcodes::kMu});
  if (!FSLep) {
    return -std::numeric_limits<double>::max();
  }

  Particle FSNuc = GetHMFSProtonInPhaseSpace(fev, CC0PiNProt_ProtonPS);
  if (!FSNuc) {
    return -std::numeric_limits<double>::max();
  }

  return GetDeltaPhiT(FSLep.P3(), FSNuc.P3(), ISSLep.P3());
}
double GetDeltaAlphaT_CC0PiN_mnv(event::FullEvent const &fev) {

  Particle ISSLep = GetHMISParticle(fev, {pdgcodes::kNuMu});
  if (!ISSLep) {
    return -std::numeric_limits<double>::max();
  }
  Particle FSLep = GetHMFSParticle(fev, {pdgcodes::kMu});
  if (!FSLep) {
    return -std::numeric_limits<double>::max();
  }

  Particle FSNuc = GetHMFSProtonInPhaseSpace(fev, CC0PiNProt_ProtonPS);
  if (!FSNuc) {
    return -std::numeric_limits<double>::max();
  }

  return GetDeltaAlphaT(FSLep.P3(), FSNuc.P3(), ISSLep.P3());
}

double GetNeutronMomentumReco_CC0PiN_mnv(event::FullEvent const &fev) {

  Particle ISSLep = GetHMISParticle(fev, {pdgcodes::kNuMu});
  if (!ISSLep) {
    return -std::numeric_limits<double>::max();
  }
  Particle FSLep = GetHMFSParticle(fev, {pdgcodes::kMu});
  if (!FSLep) {
    return -std::numeric_limits<double>::max();
  }

  Particle FSNuc = GetHMFSProtonInPhaseSpace(fev, CC0PiNProt_ProtonPS);
  if (!FSNuc) {
    return -std::numeric_limits<double>::max();
  }

  double const el = FSLep.E();
  double const eh = FSNuc.E();

  TVector3 dpt = GetDeltaPT(FSLep.P3(), FSNuc.P3(), ISSLep.P3());
  double dptMag = dpt.Mag();

  double ma = 6 * pdgmasses::kNeutronMass_MeV + 6 * pdgmasses::kProtonMass_MeV -
              92.16; // target mass (E is from PhysRevC.95.065501)
  double map = ma - pdgmasses::kNeutronMass_MeV + 27.13; // remnant mass

  double pmul = FSLep.P3().Dot(ISSLep.P3().Unit());
  double phl = FSNuc.P3().Dot(ISSLep.P3().Unit());

  double R = ma + pmul + phl - el - eh;

  double dpl = 0.5 * R - (map * map + dptMag * dptMag) / (2 * R);
  // double dpl = ((R*R)-(dptMag*dptMag)-(map*map))/(2*R); // as in in
  // PhysRevC.95.065501 - gives same result

  double pn_reco = sqrt((dptMag * dptMag) + (dpl * dpl));

  return pn_reco;
}

} // namespace mnv
} // namespace utility
} // namespace nuis
