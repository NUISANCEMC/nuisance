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

  Particle const &charged_lepton = GetHMFSChargedLepton(fev);
  if (!charged_lepton) {
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

  double el_GeV = charged_lepton.E() / 1000.0;
  double pl_GeV = charged_lepton.P() / 1000.0; // momentum of lepton
  double ml_GeV = charged_lepton.M() / 1000.0; // lepton mass

  double Theta_nu_mu = neutrino.P3().Angle(charged_lepton.P3());

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

  Particle const &charged_lepton = GetHMFSChargedLepton(fev);
  if (!charged_lepton) {
    throw Particle::invalid_particle()
        << "[ERROR]: In GetNeutrinoQ2QERec, expected to be able to get FS "
           "charged lepton, but found none:"
        << "\n"
        << fev.to_string();
  }

  double el_GeV = charged_lepton.E() / 1000.0;
  double pl_GeV = charged_lepton.P() / 1000.0; // momentum of lepton
  double ml_GeV = charged_lepton.M() / 1000.0; // lepton mass

  double Theta_nu_mu = neutrino.P3().Angle(charged_lepton.P3());

  return -ml_GeV * ml_GeV + 2.0 * GetNeutrinoEQERec(fev, SeparationEnergy_MeV) *
                                (el_GeV - pl_GeV * cos(Theta_nu_mu));
}

TLorentzVector GetEnergyMomentumTransfer(event::FullEvent const &fev) {
  Particle const &neutrino = GetHMISNeutralLepton(fev);
  if (!neutrino) {
    throw Particle::invalid_particle()
        << "[ERROR]: In GetEnergyMomentumTransfer, expected to be able to get "
           "IS neutral lepton, but found none: \n"
        << fev.to_string();
  }

  Particle const &charged_lepton = GetHMFSChargedLepton(fev);
  if (!charged_lepton) {
    throw Particle::invalid_particle()
        << "[ERROR]: In GetEnergyMomentumTransfer, expected to be able to get "
           "FS charged lepton, but found none: \n"
        << fev.to_string();
  }

    return (neutrino.P4 - charged_lepton.P4);
}

} // namespace utility
} // namespace nuis
