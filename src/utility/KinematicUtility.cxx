#include "utility/KinematicUtility.hxx"

#include "event/FullEvent.hxx"
#include "event/Particle.hxx"

#include "utility/FullEventUtility.hxx"
#include "utility/PDGCodeUtility.hxx"

using namespace nuis::event;

namespace nuis {
namespace utility {

double GetNeutrinoEQERec(FullEvent const &fev, double SeparationEnergy_MeV) {

  Particle const &neutrino = GetHMISNeutralLepton(fev);
  if (!neutrino) {
    throw Particle::invalid_particle()
        << "[ERROR]: In GetNeutrinoEQERec, expected to be able to get IS "
           "neutral lepton, but found none:"
        << "\n"
        << fev.to_string();
  }

  Particle const &lepton = GetHMFSLepton(fev);
  if (!lepton) {
    throw Particle::invalid_particle()
        << "[ERROR]: In GetNeutrinoEQERec, expected to be able to get FS "
           "charged lepton, but found none:"
        << "\n"
        << fev.to_string();
  }

  double mass_ISN =
      (IsMatter(neutrino.pdg)
           ? (pdgmasses::kNeutronMass_MeV - SeparationEnergy_MeV)
           : (pdgmasses::kProtonMass_MeV - SeparationEnergy_MeV)) /
      1000.0;

  double mass_FSN = (IsMatter(neutrino.pdg) ? pdgmasses::kProtonMass_MeV
                                            : pdgmasses::kNeutronMass_MeV) /
                    1000.0;

  double el_GeV = lepton.E() / 1000.0;
  double pl_GeV = lepton.P() / 1000.0; // momentum of lepton
  double ml_GeV = lepton.M() / 1000.0; // lepton mass

  double Theta_nu_mu = neutrino.P3().Angle(lepton.P3());

  return (2.0 * mass_ISN * el_GeV - ml_GeV * ml_GeV + mass_FSN * mass_FSN -
          mass_ISN * mass_ISN) /
         (2.0 * (mass_ISN - el_GeV + pl_GeV * cos(Theta_nu_mu)));
}

double GetNeutrinoQ2QERec(FullEvent const &fev, double SeparationEnergy_MeV) {

  Particle const &neutrino = GetHMISNeutralLepton(fev);
  if (!neutrino) {
    throw Particle::invalid_particle()
        << "[ERROR]: In GetNeutrinoQ2QERec, expected to be able to get IS "
           "neutral lepton, but found none:"
        << "\n"
        << fev.to_string();
  }

  Particle const &lepton = GetHMFSLepton(fev);
  if (!lepton) {
    throw Particle::invalid_particle()
        << "[ERROR]: In GetNeutrinoQ2QERec, expected to be able to get FS "
           "charged lepton, but found none:"
        << "\n"
        << fev.to_string();
  }

  double el_GeV = lepton.E() / 1000.0;
  double pl_GeV = lepton.P() / 1000.0; // momentum of lepton
  double ml_GeV = lepton.M() / 1000.0; // lepton mass

  double Theta_nu_mu = neutrino.P3().Angle(lepton.P3());

  return -ml_GeV * ml_GeV + 2.0 * GetNeutrinoEQERec(fev, SeparationEnergy_MeV) *
                                (el_GeV - pl_GeV * cos(Theta_nu_mu));
}

double GetNeutrinoWRec(event::FullEvent const &fev) {
  Particle const &neutrino = GetHMISNeutralLepton(fev);
  if (!neutrino) {
    throw Particle::invalid_particle()
        << "[ERROR]: In GetNeutrinoWRec, expected to be able to get IS "
           "neutral lepton, but found none:"
        << "\n"
        << fev.to_string();
  }

  Particle const &lepton = GetHMFSLepton(fev);
  if (!lepton) {
    throw Particle::invalid_particle()
        << "[ERROR]: In GetNeutrinoWRec, expected to be able to get FS "
           "charged lepton, but found none:"
        << "\n"
        << fev.to_string();
  }

  double E_mu = lepton.E();
  double p_mu = lepton.P();
  double m_mu = sqrt(E_mu * E_mu - p_mu * p_mu);
  double th_nu_mu = neutrino.P3().Angle(lepton.P3());

  // The factor of 1000 is necessary for downstream functions
  double m_p = pdgmasses::kProtonMass_MeV;

  // MINERvA cut on W_exp which is tuned to W_true; so use true Enu from
  // generators
  double E_nu = neutrino.E();

  double w_rec = sqrt(m_p * m_p + m_mu * m_mu - 2 * m_p * E_mu +
                      2 * E_nu * (m_p - E_mu + p_mu * cos(th_nu_mu)));

  return w_rec;
}

double GetEAvailProxy(event::FullEvent const &fev) {
  Particle ISNu = GetHMISNeutralLepton(fev);
  if (!ISNu) {
    return 0;
  }

  Particle FSLep = GetHMFSLepton(fev);
  if (!FSLep) {
    return 0;
  }

  TLorentzVector EMTransfer_lep = ISNu.P4 - FSLep.P4;
  double eav = EMTransfer_lep.E();

  for (auto const &neutron : GetFSNeutrons(fev)) {
    eav -= neutron.KE();
  }

  for (auto const &pion : GetFSPions(fev)) {
    eav -= pion.M();
  }

  return eav;
}

TVector3 GetVectorInTPlane(const TVector3 &inp, const TVector3 &planarNormal) {
  TVector3 pnUnit = planarNormal.Unit();
  double inpProjectPN = inp.Dot(pnUnit);

  TVector3 InPlanarInput = inp - (inpProjectPN * pnUnit);
  return InPlanarInput;
}

TVector3 GetUnitVectorInTPlane(const TVector3 &inp,
                               const TVector3 &planarNormal) {
  return GetVectorInTPlane(inp, planarNormal).Unit();
}

double GetDeltaPhiT(TVector3 const &V_lepton, TVector3 const &V_other,
                    TVector3 const &Normal, bool PiMinus) {
  TVector3 V_lepton_T = GetUnitVectorInTPlane(V_lepton, Normal);

  TVector3 V_other_T = GetUnitVectorInTPlane(V_other, Normal);

  return PiMinus ? acos(V_lepton_T.Dot(V_other_T))
                 : (M_PI - acos(V_lepton_T.Dot(V_other_T)));
}

TVector3 GetDeltaPT(TVector3 const &V_lepton, TVector3 const &V_other,
                    TVector3 const &Normal) {
  TVector3 V_lepton_T = GetVectorInTPlane(V_lepton, Normal);

  TVector3 V_other_T = GetVectorInTPlane(V_other, Normal);

  return V_lepton_T + V_other_T;
}

double GetDeltaAlphaT(TVector3 const &V_lepton, TVector3 const &V_other,
                      TVector3 const &Normal, bool PiMinus) {
  TVector3 DeltaPT = GetDeltaPT(V_lepton, V_other, Normal);

  return GetDeltaPhiT(V_lepton, DeltaPT, Normal, PiMinus);
}

TVector3 GetDeltaPT_CC0PiN(event::FullEvent const &fev, event::PDG_t islep_pdg,
                           event::PDG_t fslep_pdg, event::PDG_t fsnuc_pdg) {
  if (fslep_pdg == nuis::utility::pdgcodes::kDefault) { // Assume neutrino CC
    fslep_pdg = islep_pdg > 0 ? islep_pdg - 1 : islep_pdg + 1;
  }
  if (fsnuc_pdg == nuis::utility::pdgcodes::kDefault) {
    fsnuc_pdg = islep_pdg > 0 ? nuis::utility::pdgcodes::kProton
                              : nuis::utility::pdgcodes::kNeutron;
  }

  Particle ISSLep = GetHMISParticle(fev, {islep_pdg});
  if (!ISSLep) {
    return {-std::numeric_limits<double>::max(),
            -std::numeric_limits<double>::max(),
            -std::numeric_limits<double>::max()};
  }
  Particle FSLep = GetHMFSParticle(fev, {fslep_pdg});
  if (!FSLep) {
    return {-std::numeric_limits<double>::max(),
            -std::numeric_limits<double>::max(),
            -std::numeric_limits<double>::max()};
  }
  Particle FSNuc = GetHMFSParticle(fev, {fsnuc_pdg});
  if (!FSNuc) {
    return {-std::numeric_limits<double>::max(),
            -std::numeric_limits<double>::max(),
            -std::numeric_limits<double>::max()};
  }

  return GetDeltaPT(FSLep.P3(), FSNuc.P3(), ISSLep.P3());
}
double GetDeltaPhiT_CC0PiN(event::FullEvent const &fev, event::PDG_t islep_pdg,
                           event::PDG_t fslep_pdg, event::PDG_t fsnuc_pdg) {
  if (fslep_pdg == nuis::utility::pdgcodes::kDefault) { // Assume neutrino CC
    fslep_pdg = islep_pdg > 0 ? islep_pdg - 1 : islep_pdg + 1;
  }
  if (fsnuc_pdg == nuis::utility::pdgcodes::kDefault) {
    fsnuc_pdg = islep_pdg > 0 ? nuis::utility::pdgcodes::kProton
                              : nuis::utility::pdgcodes::kNeutron;
  }

  Particle ISSLep = GetHMISParticle(fev, {islep_pdg});
  if (!ISSLep) {
    return -std::numeric_limits<double>::max();
  }
  Particle FSLep = GetHMFSParticle(fev, {fslep_pdg});
  if (!FSLep) {
    return -std::numeric_limits<double>::max();
  }
  Particle FSNuc = GetHMFSParticle(fev, {fsnuc_pdg});
  if (!FSNuc) {
    return -std::numeric_limits<double>::max();
  }

  return GetDeltaPhiT(FSLep.P3(), FSNuc.P3(), ISSLep.P3());
}
double GetDeltaAlphaT_CC0PiN(event::FullEvent const &fev,
                             event::PDG_t islep_pdg, event::PDG_t fslep_pdg,
                             event::PDG_t fsnuc_pdg) {
  if (fslep_pdg == nuis::utility::pdgcodes::kDefault) { // Assume neutrino CC
    fslep_pdg = islep_pdg > 0 ? islep_pdg - 1 : islep_pdg + 1;
  }
  if (fsnuc_pdg == nuis::utility::pdgcodes::kDefault) {
    fsnuc_pdg = islep_pdg > 0 ? nuis::utility::pdgcodes::kProton
                              : nuis::utility::pdgcodes::kNeutron;
  }

  Particle ISSLep = GetHMISParticle(fev, {islep_pdg});
  if (!ISSLep) {
    return -std::numeric_limits<double>::max();
  }
  Particle FSLep = GetHMFSParticle(fev, {fslep_pdg});
  if (!FSLep) {
    return -std::numeric_limits<double>::max();
  }
  Particle FSNuc = GetHMFSParticle(fev, {fsnuc_pdg});
  if (!FSNuc) {
    return -std::numeric_limits<double>::max();
  }

  return GetDeltaAlphaT(FSLep.P3(), FSNuc.P3(), ISSLep.P3());
}

TLorentzVector GetEnergyMomentumTransfer(event::FullEvent const &fev) {
  Particle const &neutrino = GetHMISNeutralLepton(fev);
  if (!neutrino) {
    throw Particle::invalid_particle()
        << "[ERROR]: In GetEnergyMomentumTransfer, expected to be able to get "
           "IS neutral lepton, but found none: \n"
        << fev.to_string();
  }

  Particle const &fs_lepton = GetHMFSLepton(fev);
  if (!fs_lepton) {
    throw Particle::invalid_particle()
        << "[ERROR]: In GetEnergyMomentumTransfer, expected to be able to get "
           "FS lepton, but found none: \n"
        << fev.to_string();
  }

  return (neutrino.P4 - fs_lepton.P4);
}

} // namespace utility
} // namespace nuis