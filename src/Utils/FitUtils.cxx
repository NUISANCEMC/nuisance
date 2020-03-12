// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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
#include "FitUtils.h"

/*
  MISC Functions
*/

//********************************************************************
double *FitUtils::GetArrayFromMap(std::vector<std::string> invals,
                                  std::map<std::string, double> inmap) {
  //********************************************************************

  double *outarr = new double[invals.size()];
  int count = 0;

  for (size_t i = 0; i < invals.size(); i++) {
    outarr[count++] = inmap[invals.at(i)];
  }

  return outarr;
}
/*
  MISC Event
*/

//********************************************************************
// Returns the kinetic energy of a particle in GeV
double FitUtils::T(TLorentzVector part) {
  //********************************************************************
  double E_part = part.E() / 1000.;
  double p_part = part.Vect().Mag() / 1000.;
  double m_part = sqrt(E_part * E_part - p_part * p_part);

  double KE_part = E_part - m_part;
  return KE_part;
};

//********************************************************************
// Returns the momentum of a particle in GeV
double FitUtils::p(TLorentzVector part) {
  //********************************************************************
  double p_part = part.Vect().Mag() / 1000.;
  return p_part;
};

double FitUtils::p(FitParticle *part) { return FitUtils::p(part->fP); };

//********************************************************************
// Returns the angle between two particles in radians
double FitUtils::th(TLorentzVector part1, TLorentzVector part2) {
  //********************************************************************
  double th = part1.Vect().Angle(part2.Vect());
  return th;
};

double FitUtils::th(FitParticle *part1, FitParticle *part2) {
  return FitUtils::th(part1->fP, part2->fP);
};

// T2K CC1pi+ helper functions
//
//********************************************************************
// Returns the angle between q3 and the pion defined in Raquel's CC1pi+ on CH
// paper
// Uses "MiniBooNE formula" for Enu, here called EnuCC1pip_T2K_MB
//********************************************************************
double FitUtils::thq3pi_CC1pip_T2K(TLorentzVector pnu, TLorentzVector pmu,
                                   TLorentzVector ppi) {
  // Want this in GeV
  TVector3 p_mu = pmu.Vect() * (1. / 1000.);

  // Get the reconstructed Enu
  // We are not using Michel e sample, so we have pion kinematic information
  double Enu = EnuCC1piprec(pnu, pmu, ppi, true);

  // Get neutrino unit direction, multiply by reconstructed Enu
  TVector3 p_nu = pnu.Vect() * (1. / (pnu.Vect().Mag())) * Enu;
  TVector3 p_pi = ppi.Vect() * (1. / 1000.);

  // This is now in GeV
  TVector3 q3 = (p_nu - p_mu);
  // Want this in GeV

  double th_q3_pi = q3.Angle(p_pi);

  return th_q3_pi;
}

//********************************************************************
// Returns the q3 defined in Raquel's CC1pi+ on CH paper
// Uses "MiniBooNE formula" for Enu
//********************************************************************
double FitUtils::q3_CC1pip_T2K(TLorentzVector pnu, TLorentzVector pmu,
                               TLorentzVector ppi) {
  // Can't use the true Enu here; need to reconstruct it
  // We do have Michel e- here so reconstruct Enu by "MiniBooNE formula" without
  // pion kinematics
  // The bool false refers to that we don't have pion kinematics
  // Last bool refers to if we have pion kinematic information or not
  double Enu = EnuCC1piprec(pnu, pmu, ppi, false);

  TVector3 p_mu = pmu.Vect() * (1. / 1000.);
  TVector3 p_nu = pnu.Vect() * (1. / pnu.Vect().Mag()) * Enu;

  double q3 = (p_nu - p_mu).Mag();

  return q3;
}

//********************************************************************
// Returns the W reconstruction from Raquel CC1pi+ CH thesis
// Uses the MiniBooNE formula Enu
//********************************************************************
double FitUtils::WrecCC1pip_T2K_MB(TLorentzVector pnu, TLorentzVector pmu,
                                   TLorentzVector ppi) {
  double E_mu = pmu.E() / 1000.;
  double p_mu = pmu.Vect().Mag() / 1000.;
  double E_nu = EnuCC1piprec(pnu, pmu, ppi, false);

  double a1 = (E_nu + PhysConst::mass_neutron) - E_mu;
  double a2 = E_nu - p_mu;

  double wrec = sqrt(a1 * a1 - a2 * a2);

  return wrec;
}

//********************************************************
double FitUtils::ProtonQ2QErec(double pE, double binding) {
  //********************************************************

  const double V = binding / 1000.;          // binding potential
  const double mn = PhysConst::mass_neutron; // neutron mass
  const double mp = PhysConst::mass_proton;  // proton mass
  const double mn_eff = mn - V;              // effective proton mass
  const double pki = (pE / 1000.0) - mp;

  double q2qe = mn_eff * mn_eff - mp * mp + 2 * mn_eff * (pki + mp - mn_eff);

  return q2qe;
};

//********************************************************************
double FitUtils::EnuQErec(TLorentzVector pmu, double costh, double binding,
                          bool neutrino) {
  //********************************************************************

  // Convert all values to GeV
  const double V = binding / 1000.;          // binding potential
  const double mn = PhysConst::mass_neutron; // neutron mass
  const double mp = PhysConst::mass_proton;  // proton mass

  double mN_eff = mn - V;
  double mN_oth = mp;

  if (!neutrino) {
    mN_eff = mp - V;
    mN_oth = mn;
  }

  double el = pmu.E() / 1000.;
  double pl = (pmu.Vect().Mag()) / 1000.; // momentum of lepton
  double ml = sqrt(el * el - pl * pl);    // lepton mass

  double rEnu =
      (2 * mN_eff * el - ml * ml + mN_oth * mN_oth - mN_eff * mN_eff) /
      (2 * (mN_eff - el + pl * costh));

  return rEnu;
};

// Another good old helper function
double FitUtils::EnuQErec(TLorentzVector pmu, TLorentzVector pnu,
                          double binding, bool neutrino) {
  return EnuQErec(pmu, cos(pnu.Vect().Angle(pmu.Vect())), binding, neutrino);
}

double FitUtils::Q2QErec(TLorentzVector pmu, double costh, double binding,
                         bool neutrino) {
  double el = pmu.E() / 1000.;
  double pl = (pmu.Vect().Mag()) / 1000.; // momentum of lepton
  double ml = sqrt(el * el - pl * pl);    // lepton mass

  double rEnu = EnuQErec(pmu, costh, binding, neutrino);
  double q2 = -ml * ml + 2. * rEnu * (el - pl * costh);

  return q2;
};

double FitUtils::Q2QErec(TLorentzVector Pmu, TLorentzVector Pnu, double binding,
                         bool neutrino) {
  double q2qe =
      Q2QErec(Pmu, cos(Pnu.Vect().Angle(Pmu.Vect())), binding, neutrino);
  return q2qe;
}

double FitUtils::EnuQErec(double pl, double costh, double binding,
                          bool neutrino) {
  if (pl < 0)
    return 0.; // Make sure nobody is silly

  double mN_eff = PhysConst::mass_neutron - binding / 1000.;
  double mN_oth = PhysConst::mass_proton;

  if (!neutrino) {
    mN_eff = PhysConst::mass_proton - binding / 1000.;
    mN_oth = PhysConst::mass_neutron;
  }
  double ml = PhysConst::mass_muon;
  double el = sqrt(pl * pl + ml * ml);

  double rEnu =
      (2 * mN_eff * el - ml * ml + mN_oth * mN_oth - mN_eff * mN_eff) /
      (2 * (mN_eff - el + pl * costh));

  return rEnu;
};

double FitUtils::Q2QErec(double pl, double costh, double binding,
                         bool neutrino) {
  if (pl < 0)
    return 0.; // Make sure nobody is silly

  double ml = PhysConst::mass_muon;
  double el = sqrt(pl * pl + ml * ml);

  double rEnu = EnuQErec(pl, costh, binding, neutrino);
  double q2 = -ml * ml + 2. * rEnu * (el - pl * costh);

  return q2;
};

//********************************************************************
// Reconstructs Enu for CC1pi0
// Very similar for CC1pi+ reconstruction
double FitUtils::EnuCC1pi0rec(TLorentzVector pnu, TLorentzVector pmu,
                              TLorentzVector ppi0) {
  //********************************************************************

  double E_mu = pmu.E() / 1000;
  double p_mu = pmu.Vect().Mag() / 1000;
  double m_mu = sqrt(E_mu * E_mu - p_mu * p_mu);
  double th_nu_mu = pnu.Vect().Angle(pmu.Vect());

  double E_pi0 = ppi0.E() / 1000;
  double p_pi0 = ppi0.Vect().Mag() / 1000;
  double m_pi0 = sqrt(E_pi0 * E_pi0 - p_pi0 * p_pi0);
  double th_nu_pi0 = pnu.Vect().Angle(ppi0.Vect());

  const double m_n = PhysConst::mass_neutron; // neutron mass
  const double m_p = PhysConst::mass_proton;  // proton mass
  double th_pi0_mu = ppi0.Vect().Angle(pmu.Vect());

  double rEnu = (m_mu * m_mu + m_pi0 * m_pi0 + m_n * m_n - m_p * m_p -
                 2 * m_n * (E_pi0 + E_mu) + 2 * E_pi0 * E_mu -
                 2 * p_pi0 * p_mu * cos(th_pi0_mu)) /
                (2 * (E_pi0 + E_mu - p_pi0 * cos(th_nu_pi0) -
                      p_mu * cos(th_nu_mu) - m_n));

  return rEnu;
};

//********************************************************************
// Reconstruct Q2 for CC1pi0
// Beware: uses true Enu, not reconstructed Enu
double FitUtils::Q2CC1pi0rec(TLorentzVector pnu, TLorentzVector pmu,
                             TLorentzVector ppi0) {
  //********************************************************************

  double E_mu = pmu.E() / 1000.;                 // energy of lepton in GeV
  double p_mu = pmu.Vect().Mag() / 1000.;        // momentum of lepton
  double m_mu = sqrt(E_mu * E_mu - p_mu * p_mu); // lepton mass
  double th_nu_mu = pnu.Vect().Angle(pmu.Vect());

  // double rEnu = EnuCC1pi0rec(pnu, pmu, ppi0); //reconstructed neutrino energy
  // Use true neutrino energy
  double rEnu = pnu.E() / 1000.;
  double q2 = -m_mu * m_mu + 2. * rEnu * (E_mu - p_mu * cos(th_nu_mu));

  return q2;
};

//********************************************************************
// Reconstruct Enu for CC1pi+
// pionInfo reflects if we're using pion kinematics or not
// In T2K CC1pi+ CH the Michel tag is used for pion in which pion kinematic info
// is lost and Enu is reconstructed without pion kinematics
double FitUtils::EnuCC1piprec(TLorentzVector pnu, TLorentzVector pmu,
                              TLorentzVector ppi, bool pionInfo) {
  //********************************************************************

  double E_mu = pmu.E() / 1000.;
  double p_mu = pmu.Vect().Mag() / 1000.;
  double m_mu = sqrt(E_mu * E_mu - p_mu * p_mu);

  double E_pi = ppi.E() / 1000.;
  double p_pi = ppi.Vect().Mag() / 1000.;
  double m_pi = sqrt(E_pi * E_pi - p_pi * p_pi);

  const double m_n = PhysConst::mass_neutron; // neutron/proton mass
  // should really take proton mass for proton interaction, neutron for neutron
  // interaction. However, difference is pretty much negligable here!

  // need this angle too
  double th_nu_pi = pnu.Vect().Angle(ppi.Vect());
  double th_nu_mu = pnu.Vect().Angle(pmu.Vect());
  double th_pi_mu = ppi.Vect().Angle(pmu.Vect());

  double rEnu = -999;
  // If experiment doesn't have pion kinematic information (T2K CC1pi+ CH
  // example of this; Michel e sample has no directional information on pion)
  // We'll need to modify the reconstruction variables
  if (!pionInfo) {
    // Assumes that pion angle contribution contributes net zero
    // Also assumes the momentum of reconstructed pions when using Michel
    // electrons is 130 MeV
    // So need to redefine E_pi and p_pi
    // It's a little unclear what happens to the pmu . ppi term (4-vector); do
    // we include the angular contribution?
    // This below doesn't
    th_nu_pi = M_PI / 2.;
    p_pi = 0.130;
    E_pi = sqrt(p_pi * p_pi + m_pi * m_pi);
    // rEnu = (m_mu*m_mu + m_pi*m_pi - 2*m_n*(E_mu + E_pi) + 2*E_mu*E_pi -
    // 2*p_mu*p_pi) / (2*(E_mu + E_pi - p_mu*cos(th_nu_mu) - m_n));
  }

  rEnu =
      (m_mu * m_mu + m_pi * m_pi - 2 * m_n * (E_pi + E_mu) + 2 * E_pi * E_mu -
       2 * p_pi * p_mu * cos(th_pi_mu)) /
      (2 * (E_pi + E_mu - p_pi * cos(th_nu_pi) - p_mu * cos(th_nu_mu) - m_n));

  return rEnu;
};

//********************************************************************
// Reconstruct neutrino energy from outgoing particles; will differ from the
// actual neutrino energy. Here we use assumption of a Delta resonance
double FitUtils::EnuCC1piprecDelta(TLorentzVector pnu, TLorentzVector pmu) {
  //********************************************************************

  const double m_Delta =
      PhysConst::mass_delta;                  // PDG value for Delta mass in GeV
  const double m_n = PhysConst::mass_neutron; // neutron/proton mass
  // should really take proton mass for proton interaction, neutron for neutron
  // interaction. However, difference is pretty much negligable here!

  double E_mu = pmu.E() / 1000;
  double p_mu = pmu.Vect().Mag() / 1000;
  double m_mu = sqrt(E_mu * E_mu - p_mu * p_mu);
  double th_nu_mu = pnu.Vect().Angle(pmu.Vect());

  double rEnu = (m_Delta * m_Delta - m_n * m_n - m_mu * m_mu + 2 * m_n * E_mu) /
                (2 * (m_n - E_mu + p_mu * cos(th_nu_mu)));

  return rEnu;
};

// MOVE TO T2K UTILS!
//********************************************************************
// Reconstruct Enu using "extended MiniBooNE" as defined in Raquel's T2K TN
//
// Supposedly includes pion direction and binding energy of target nucleon
// I'm not convinced (yet), maybe
double FitUtils::EnuCC1piprec_T2K_eMB(TLorentzVector pnu, TLorentzVector pmu,
                                      TLorentzVector ppi) {
  //********************************************************************

  // Unit vector for neutrino momentum
  TVector3 p_nu_vect_unit = pnu.Vect() * (1. / pnu.E());

  double E_mu = pmu.E() / 1000.;
  TVector3 p_mu_vect = pmu.Vect() * (1. / 1000.);

  double E_pi = ppi.E() / 1000.;
  TVector3 p_pi_vect = ppi.Vect() * (1. / 1000.);

  double E_bind = 25. / 1000.;
  double m_p = PhysConst::mass_proton;

  // Makes life a little easier, gonna square this one
  double a1 = m_p - E_bind - E_mu - E_pi;
  // Just gets the magnitude square of the muon and pion momentum vectors
  double a2 = (p_mu_vect + p_pi_vect).Mag2();
  // Gets the somewhat complicated scalar product between neutrino and
  // (p_mu+p_pi), scaled to Enu
  double a3 = p_nu_vect_unit * (p_mu_vect + p_pi_vect);

  double rEnu =
      (m_p * m_p - a1 * a1 + a2) / (2 * (m_p - E_bind - E_mu - E_pi + a3));

  return rEnu;
}

//********************************************************************
// Reconstructed Q2 for CC1pi+
//
// enuType describes how the neutrino energy is reconstructed
// 0 uses true neutrino energy; case for MINERvA and MiniBooNE
// 1 uses "extended MiniBooNE" reconstructed (T2K CH)
// 2 uses "MiniBooNE" reconstructed (EnuCC1piprec with pionInfo = true) (T2K CH)
//        "MiniBooNE" reconstructed (EnuCC1piprec with pionInfo = false, the
//        case for T2K when using Michel tag) (T2K CH)
// 3 uses Delta for reconstruction (T2K CH)
double FitUtils::Q2CC1piprec(TLorentzVector pnu, TLorentzVector pmu,
                             TLorentzVector ppi, int enuType, bool pionInfo) {
  //********************************************************************

  double E_mu = pmu.E() / 1000.;                 // energy of lepton in GeV
  double p_mu = pmu.Vect().Mag() / 1000.;        // momentum of lepton
  double m_mu = sqrt(E_mu * E_mu - p_mu * p_mu); // lepton mass
  double th_nu_mu = pnu.Vect().Angle(pmu.Vect());

  // Different ways of reconstructing the neutrino energy; default is just to
  // use the truth (case 0)
  double rEnu = -999;

  switch (enuType) {
  case 0: // True neutrino energy, default; this is the case for when Q2
          // defined as Q2 true (MiniBooNE, MINERvA)
    rEnu = pnu.E() / 1000.;
    break;
  case 1: // Extended MiniBooNE reconstructed, as defined by Raquel's CC1pi+
          // CH T2K analysis
    rEnu = EnuCC1piprec_T2K_eMB(pnu, pmu, ppi);
    break;
  case 2: // MiniBooNE reconstructed, as defined by MiniBooNE and Raquel's
          // CC1pi+ CH T2K analysis and Linda's CC1pi+ H2O
    // Can have this with and without pion info, depending on if Michel tag
    // used (Raquel)
    rEnu = EnuCC1piprec(pnu, pmu, ppi, pionInfo);
    break;
  case 3:
    rEnu = EnuCC1piprecDelta(pnu, pmu);
    break;

  } // No need for default here since default value of enuType = 0, defined in
    // header

  double q2 = -m_mu * m_mu + 2. * rEnu * (E_mu - p_mu * cos(th_nu_mu));

  return q2;
};

//********************************************************************
// Returns the reconstructed W from a nucleon and an outgoing pion
//
// Could do this a lot more clever (pp + ppi).Mag() would do the job, but this
// would be less instructive
//********************************************************************
double FitUtils::MpPi(TLorentzVector pp, TLorentzVector ppi) {
  double E_p = pp.E();
  double p_p = pp.Vect().Mag();
  double m_p = sqrt(E_p * E_p - p_p * p_p);

  double E_pi = ppi.E();
  double p_pi = ppi.Vect().Mag();
  double m_pi = sqrt(E_pi * E_pi - p_pi * p_pi);

  double th_p_pi = pp.Vect().Angle(ppi.Vect());

  // fairly easy thing to derive since bubble chambers measure the proton!
  double invMass = sqrt(m_p * m_p + m_pi * m_pi + 2 * E_p * E_pi -
                        2 * p_pi * p_p * cos(th_p_pi));

  return invMass;
};

//********************************************************

// Reconstruct the hadronic mass using neutrino and muon
// Could technically do E_nu = EnuCC1pipRec(pnu,pmu,ppi) too, but this wwill be
// reconstructed Enu; so gives reconstructed Wrec which most of the time isn't
// used!
//
// Only MINERvA uses this so far; and the Enu is Enu_true
// If we want W_true need to take initial state nucleon motion into account
// Return value is in MeV!!!
double FitUtils::Wrec(TLorentzVector pnu, TLorentzVector pmu) {
  //********************************************************

  double E_mu = pmu.E();
  double p_mu = pmu.Vect().Mag();
  double m_mu = sqrt(E_mu * E_mu - p_mu * p_mu);
  double th_nu_mu = pnu.Vect().Angle(pmu.Vect());

  // The factor of 1000 is necessary for downstream functions
  const double m_p = PhysConst::mass_proton * 1000;

  // MINERvA cut on W_exp which is tuned to W_true; so use true Enu from
  // generators
  double E_nu = pnu.E();

  double w_rec = sqrt(m_p * m_p + m_mu * m_mu - 2 * m_p * E_mu +
                      2 * E_nu * (m_p - E_mu + p_mu * cos(th_nu_mu)));

  return w_rec;
};

//********************************************************
// Reconstruct the true hadronic mass using the initial state and muon
// Could technically do E_nu = EnuCC1pipRec(pnu,pmu,ppi) too, but this wwill be
// reconstructed Enu; so gives reconstructed Wrec which most of the time isn't
// used!
//
// No one seems to use this because it's fairly MC dependent!
// Return value is in MeV!!!
double FitUtils::Wtrue(TLorentzVector pnu, TLorentzVector pmu,
                       TLorentzVector pnuc) {
  //********************************************************

  // Could simply do the TLorentzVector operators here but this is more
  // instructive?
  // ... and prone to errors ...

  double E_mu = pmu.E();
  double p_mu = pmu.Vect().Mag();
  double m_mu = sqrt(E_mu * E_mu - p_mu * p_mu);
  double th_nu_mu = pnu.Vect().Angle(pmu.Vect());

  double E_nuc = pnuc.E();
  double p_nuc = pnuc.Vect().Mag();
  double m_nuc = sqrt(E_nuc * E_nuc - p_nuc * p_nuc);
  double th_nuc_mu = pmu.Vect().Angle(pnuc.Vect());
  double th_nu_nuc = pnu.Vect().Angle(pnuc.Vect());

  double E_nu = pnu.E();

  double w_rec = sqrt(m_nuc * m_nuc + m_mu * m_mu - 2 * E_nu * E_mu +
                      2 * E_nu * p_mu * cos(th_nu_mu) - 2 * E_nuc * E_mu +
                      2 * p_nuc * p_mu * cos(th_nuc_mu) + 2 * E_nu * E_nuc -
                      2 * E_nu * p_nuc * cos(th_nu_nuc));

  return w_rec;
};

double FitUtils::SumKE_PartVect(std::vector<FitParticle *> const fps) {
  double sum = 0.0;
  for (size_t p_it = 0; p_it < fps.size(); ++p_it) {
    sum += fps[p_it]->KE();
  }
  return sum;
}
double FitUtils::SumTE_PartVect(std::vector<FitParticle *> const fps) {
  double sum = 0.0;
  for (size_t p_it = 0; p_it < fps.size(); ++p_it) {
    sum += fps[p_it]->E();
  }
  return sum;
}

/*
  E Recoil
*/
double FitUtils::GetErecoil_TRUE(FitEvent *event) {
  // Get total energy of hadronic system.
  double Erecoil = 0.0;
  for (unsigned int i = 2; i < event->Npart(); i++) {
    // Only final state
    if (!event->PartInfo(i)->fIsAlive)
      continue;
    if (event->PartInfo(i)->fNEUTStatusCode != 0)
      continue;

    // Skip Lepton
    if (abs(event->PartInfo(i)->fPID) == abs(event->PartInfo(0)->fPID) - 1)
      continue;

    // Add Up KE of protons and TE of everything else
    if (event->PartInfo(i)->fPID == 2212 || event->PartInfo(i)->fPID == 2112) {
      Erecoil +=
          fabs(event->PartInfo(i)->fP.E()) - fabs(event->PartInfo(i)->fP.Mag());
    } else {
      Erecoil += event->PartInfo(i)->fP.E();
    }
  }

  return Erecoil;
}

double FitUtils::GetErecoil_CHARGED(FitEvent *event) {
  // Get total energy of hadronic system.
  double Erecoil = 0.0;
  for (unsigned int i = 2; i < event->Npart(); i++) {
    // Only final state
    if (!event->PartInfo(i)->fIsAlive)
      continue;
    if (event->PartInfo(i)->fNEUTStatusCode != 0)
      continue;

    // Skip Lepton
    if (abs(event->PartInfo(i)->fPID) == abs(event->PartInfo(0)->fPID) - 1)
      continue;

    // Skip Neutral particles
    if (event->PartInfo(i)->fPID == 2112 || event->PartInfo(i)->fPID == 111 ||
        event->PartInfo(i)->fPID == 22)
      continue;

    // Add Up KE of protons and TE of everything else
    if (event->PartInfo(i)->fPID == 2212) {
      Erecoil +=
          fabs(event->PartInfo(i)->fP.E()) - fabs(event->PartInfo(i)->fP.Mag());
    } else {
      Erecoil += event->PartInfo(i)->fP.E();
    }
  }

  return Erecoil;
}

// MOVE TO MINERVA Utils!
double FitUtils::GetErecoil_MINERvA_LowRecoil(FitEvent *event) {
  // Get total energy of hadronic system.
  double Erecoil = 0.0;

  for (unsigned int i = 2; i < event->Npart(); i++) {
    // Only final state
    if (!event->PartInfo(i)->fIsAlive)
      continue;
    if (event->PartInfo(i)->Status() != kFinalState)
      continue;

    // Skip Lepton
    if (abs(event->PartInfo(i)->fPID) == 13)
      continue;

    // Skip Neutrons particles
    if (event->PartInfo(i)->fPID == 2112)
      continue;

    int PID = event->PartInfo(i)->fPID;

    // KE of Protons and charged pions
    if (PID == 2212 or PID == 211 or PID == -211) {
      //      Erecoil += FitUtils::T(event->PartInfo(i)->fP);
      Erecoil +=
          fabs(event->PartInfo(i)->fP.E()) - fabs(event->PartInfo(i)->fP.Mag());

      // Total Energy of non-neutrons
      //    } else if (PID != 2112 and PID < 999 and PID != 22 and abs(PID) !=
      //    14) {
    } else if (PID == 111 || PID == 11 || PID == -11 || PID == 22) {
      Erecoil += (event->PartInfo(i)->fP.E());
    }
  }

  return Erecoil;
}

// MOVE TO MINERVA Utils!
// The alternative Eavailble definition takes true q0 and subtracts the kinetic
// energy of neutrons and pion masses returns in MeV
double FitUtils::Eavailable(FitEvent *event) {
  double Eav = 0.0;

  // Now take q0 and subtract Eav
  double q0 = event->GetBeamPart()->fP.E();
  // Get the pdg of incoming neutrino
  int ISPDG = event->GetBeamPartPDG();

  // For CC
  if (event->IsCC())
    q0 -= event->GetHMFSParticle(ISPDG + ((ISPDG < 0) ? 1 : -1))->fP.E();
  else
    q0 -= event->GetHMFSParticle(ISPDG)->fP.E();

  for (unsigned int i = 2; i < event->Npart(); i++) {
    // Only final state
    if (!event->PartInfo(i)->fIsAlive)
      continue;
    if (event->PartInfo(i)->Status() != kFinalState)
      continue;
    int PID = event->PartInfo(i)->fPID;

    // Neutrons
    if (PID == 2112) {
      // Adding kinetic energy of neutron
      Eav += FitUtils::T(event->PartInfo(i)->fP) * 1000.;
      // All pion masses
    } else if (abs(PID) == 211 || PID == 111) {
      Eav += event->PartInfo(i)->fP.M();
    }
  }

  return q0 - Eav;
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

Double_t GetDeltaPhiT(TVector3 const &V_lepton, TVector3 const &V_other,
                      TVector3 const &Normal, bool PiMinus = false) {
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

Double_t GetDeltaAlphaT(TVector3 const &V_lepton, TVector3 const &V_other,
                        TVector3 const &Normal, bool PiMinus = false) {
  TVector3 DeltaPT = GetDeltaPT(V_lepton, V_other, Normal);

  return GetDeltaPhiT(V_lepton, DeltaPT, Normal, PiMinus);
}

double FitUtils::Get_STV_dpt_protonps(FitEvent *event, double ProtonMinCut,
                                      double ProtonMaxCut,
                                      double ProtonCosThetaCut, int ISPDG,
                                      bool Is0pi) {
  // Check that the neutrino exists
  if (event->NumISParticle(ISPDG) == 0) {
    return -9999;
  }
  // Return 0 if the proton or muon are missing
  if (event->NumFSParticle(2212) == 0 ||
      event->NumFSParticle(ISPDG + ((ISPDG < 0) ? 1 : -1)) == 0) {
    return -9999;
  }

  // Now get the TVector3s for each particle
  TVector3 const &NuP = event->GetHMISParticle(ISPDG)->fP.Vect();
  TVector3 const &LeptonP =
      event->GetHMFSParticle(ISPDG + ((ISPDG < 0) ? 1 : -1))->fP.Vect();
  // Find the highest momentum proton in the event between ProtonMinCut and
  // ProtonMaxCut MeV with cos(theta_p) > ProtonCosThetaCut
  TLorentzVector Pprot = FitUtils::GetProtonInRange(
      event, ProtonMinCut, ProtonMaxCut, ProtonCosThetaCut);

  // Get highest momentum proton in allowed proton range
  TVector3 HadronP = Pprot.Vect();

  // If we don't have a CC0pi signal definition we also add in pion momentum
  if (!Is0pi) {
    if (event->NumFSParticle(PhysConst::pdg_pions) == 0) {
      return -9999;
    }
    // Count up pion momentum
    TLorentzVector pp = event->GetHMFSParticle(PhysConst::pdg_pions)->fP;
    HadronP += pp.Vect();
  }
  return GetDeltaPT(LeptonP, HadronP, NuP).Mag();
}

double FitUtils::Get_STV_dphit_protonps(FitEvent *event, double ProtonMinCut,
                                        double ProtonMaxCut,
                                        double ProtonCosThetaCut, int ISPDG,
                                        bool Is0pi) {
  // Check that the neutrino exists
  if (event->NumISParticle(ISPDG) == 0) {
    return -9999;
  }
  // Return 0 if the proton or muon are missing
  if (event->NumFSParticle(2212) == 0 ||
      event->NumFSParticle(ISPDG + ((ISPDG < 0) ? 1 : -1)) == 0) {
    return -9999;
  }

  // Now get the TVector3s for each particle
  TVector3 const &NuP = event->GetHMISParticle(ISPDG)->fP.Vect();
  TVector3 const &LeptonP =
      event->GetHMFSParticle(ISPDG + ((ISPDG < 0) ? 1 : -1))->fP.Vect();

  // Find the highest momentum proton in the event between ProtonMinCut and
  // ProtonMaxCut MeV with cos(theta_p) > ProtonCosThetaCut
  TLorentzVector Pprot = FitUtils::GetProtonInRange(
      event, ProtonMinCut, ProtonMaxCut, ProtonCosThetaCut);
  TVector3 HadronP = Pprot.Vect();
  if (!Is0pi) {
    if (event->NumFSParticle(PhysConst::pdg_pions) == 0) {
      return -9999;
    }
    TLorentzVector pp = event->GetHMFSParticle(PhysConst::pdg_pions)->fP;
    HadronP += pp.Vect();
  }
  return GetDeltaPhiT(LeptonP, HadronP, NuP);
}

double FitUtils::Get_STV_dalphat_protonps(FitEvent *event, double ProtonMinCut,
                                          double ProtonMaxCut,
                                          double ProtonCosThetaCut, int ISPDG,
                                          bool Is0pi) {
  // Check that the neutrino exists
  if (event->NumISParticle(ISPDG) == 0) {
    return -9999;
  }
  // Return 0 if the proton or muon are missing
  if (event->NumFSParticle(2212) == 0 ||
      event->NumFSParticle(ISPDG + ((ISPDG < 0) ? 1 : -1)) == 0) {
    return -9999;
  }

  // Now get the TVector3s for each particle
  TVector3 const &NuP = event->GetHMISParticle(ISPDG)->fP.Vect();
  TVector3 const &LeptonP =
      event->GetHMFSParticle(ISPDG + ((ISPDG < 0) ? 1 : -1))->fP.Vect();

  // Find the highest momentum proton in the event between ProtonMinCut and
  // ProtonMaxCut MeV with cos(theta_p) > ProtonCosThetaCut
  TLorentzVector Pprot = FitUtils::GetProtonInRange(
      event, ProtonMinCut, ProtonMaxCut, ProtonCosThetaCut);
  TVector3 HadronP = Pprot.Vect();
  if (!Is0pi) {
    if (event->NumFSParticle(PhysConst::pdg_pions) == 0) {
      return -9999;
    }
    TLorentzVector pp = event->GetHMFSParticle(PhysConst::pdg_pions)->fP;
    HadronP += pp.Vect();
  }
  return GetDeltaAlphaT(LeptonP, HadronP, NuP);
}

// As defined in PhysRevC.95.065501
// Using prescription from arXiv 1805.05486
// Returns in GeV
double FitUtils::Get_pn_reco_C_protonps(FitEvent *event, double ProtonMinCut,
                                        double ProtonMaxCut,
                                        double ProtonCosThetaCut, int ISPDG,
                                        bool Is0pi) {

  const double mn = PhysConst::mass_neutron; // neutron mass
  const double mp = PhysConst::mass_proton;  // proton mass

  // Check that the neutrino exists
  if (event->NumISParticle(ISPDG) == 0) {
    return -9999;
  }
  // Return 0 if the proton or muon are missing
  if (event->NumFSParticle(2212) == 0 ||
      event->NumFSParticle(ISPDG + ((ISPDG < 0) ? 1 : -1)) == 0) {
    return -9999;
  }

  // Now get the TVector3s for each particle
  TVector3 const &NuP = event->GetHMISParticle(ISPDG)->fP.Vect();
  TVector3 const &LeptonP =
      event->GetHMFSParticle(ISPDG + ((ISPDG < 0) ? 1 : -1))->fP.Vect();

  // Find the highest momentum proton in the event between ProtonMinCut and
  // ProtonMaxCut MeV with cos(theta_p) < ProtonCosThetaCut
  TLorentzVector Pprot = FitUtils::GetProtonInRange(
      event, ProtonMinCut, ProtonMaxCut, ProtonCosThetaCut);
  TVector3 HadronP = Pprot.Vect();

  double const el =
      event->GetHMFSParticle(ISPDG + ((ISPDG < 0) ? 1 : -1))->E() / 1000.;
  double const eh = Pprot.E() / 1000.;

  if (!Is0pi) {
    if (event->NumFSParticle(PhysConst::pdg_pions) == 0) {
      return -9999;
    }
    TLorentzVector pp = event->GetHMFSParticle(PhysConst::pdg_pions)->fP;
    HadronP += pp.Vect();
  }
  TVector3 dpt = GetDeltaPT(LeptonP, HadronP, NuP);
  double dptMag = dpt.Mag() / 1000.;

  double ma =
      6 * mn + 6 * mp - 0.09216;  // target mass (E is from PhysRevC.95.065501)
  double map = ma - mn + 0.02713; // remnant mass

  double pmul = LeptonP.Dot(NuP.Unit()) / 1000.;
  double phl = HadronP.Dot(NuP.Unit()) / 1000.;

  // double pmul = GetVectorInTPlane(LeptonP, dpt).Mag()/1000.;
  // double phl = GetVectorInTPlane(HadronP, dpt).Mag()/1000.;

  double R = ma + pmul + phl - el - eh;

  double dpl = 0.5 * R - (map * map + dptMag * dptMag) / (2 * R);
  // double dpl = ((R*R)-(dptMag*dptMag)-(map*map))/(2*R); // as in in
  // PhysRevC.95.065501 - gives same result

  double pn_reco = sqrt((dptMag * dptMag) + (dpl * dpl));

  // std::cout << "Diagnostics: " << std::endl;
  // std::cout << "mn: " << mn << std::endl;
  // std::cout << "ma: " << ma << std::endl;
  // std::cout << "map: " << map << std::endl;
  // std::cout << "pmu: " << LeptonP.Mag()/1000. << std::endl;
  // std::cout << "ph: " << HadronP.Mag()/1000. << std::endl;
  // std::cout << "pmul: " << pmul << std::endl;
  // std::cout << "phl: " << phl << std::endl;
  // std::cout << "el: " << el << std::endl;
  // std::cout << "eh: " << eh << std::endl;
  // std::cout << "R: " << R << std::endl;
  // std::cout << "dptMag: " << dptMag << std::endl;
  // std::cout << "dpl: " << dpl << std::endl;
  // std::cout << "pn_reco: " << pn_reco << std::endl;

  return pn_reco;
}

double FitUtils::Get_pn_reco_Ar_protonps(FitEvent *event, double ProtonMinCut,
                                         double ProtonMaxCut,
                                         double ProtonCosThetaCut, int ISPDG,
                                         bool Is0pi) {

  const double mn = PhysConst::mass_neutron; // neutron mass
  const double mp = PhysConst::mass_proton;  // proton mass

  // Check that the neutrino exists
  if (event->NumISParticle(ISPDG) == 0) {
    return -9999;
  }
  // Return 0 if the proton or muon are missing
  if (event->NumFSParticle(2212) == 0 ||
      event->NumFSParticle(ISPDG + ((ISPDG < 0) ? 1 : -1)) == 0) {
    return -9999;
  }

  // Now get the TVector3s for each particle
  TVector3 const &NuP = event->GetHMISParticle(ISPDG)->fP.Vect();
  TVector3 const &LeptonP =
      event->GetHMFSParticle(ISPDG + ((ISPDG < 0) ? 1 : -1))->fP.Vect();

  // Find the highest momentum proton in the event between ProtonMinCut and
  // ProtonMaxCut MeV with cos(theta_p) > ProtonCosThetaCut
  TLorentzVector Pprot = FitUtils::GetProtonInRange(
      event, ProtonMinCut, ProtonMaxCut, ProtonCosThetaCut);
  TVector3 HadronP = Pprot.Vect();

  double const el =
      event->GetHMFSParticle(ISPDG + ((ISPDG < 0) ? 1 : -1))->E() / 1000.;
  double const eh = Pprot.E() / 1000.;

  if (!Is0pi) {
    if (event->NumFSParticle(PhysConst::pdg_pions) == 0) {
      return -9999;
    }
    TLorentzVector pp = event->GetHMFSParticle(PhysConst::pdg_pions)->fP;
    HadronP += pp.Vect();
  }
  TVector3 dpt = GetDeltaPT(LeptonP, HadronP, NuP);
  double dptMag = dpt.Mag() / 1000.;

  // double ma = 6*mn + 6*mp - 0.09216; // target mass (E is from
  // PhysRevC.95.065501) double map = ma - mn + 0.02713; // remnant mass
  double ma =
      6 * mn + 6 * mp - 0.34381;  // target mass (E is from PhysRevC.95.065501)
  double map = ma - mn + 0.02713; // remnant mass

  double pmul = LeptonP.Dot(NuP.Unit()) / 1000.;
  double phl = HadronP.Dot(NuP.Unit()) / 1000.;

  // double pmul = GetVectorInTPlane(LeptonP, dpt).Mag()/1000.;
  // double phl = GetVectorInTPlane(HadronP, dpt).Mag()/1000.;

  double R = ma + pmul + phl - el - eh;

  double dpl = 0.5 * R - (map * map + dptMag * dptMag) / (2 * R);
  // double dpl = ((R*R)-(dptMag*dptMag)-(map*map))/(2*R); // as in in
  // PhysRevC.95.065501 - gives same result

  double pn_reco = sqrt((dptMag * dptMag) + (dpl * dpl));

  // std::cout << "Diagnostics: " << std::endl;
  // std::cout << "mn: " << mn << std::endl;
  // std::cout << "ma: " << ma << std::endl;
  // std::cout << "map: " << map << std::endl;
  // std::cout << "pmu: " << LeptonP.Mag()/1000. << std::endl;
  // std::cout << "ph: " << HadronP.Mag()/1000. << std::endl;
  // std::cout << "pmul: " << pmul << std::endl;
  // std::cout << "phl: " << phl << std::endl;
  // std::cout << "el: " << el << std::endl;
  // std::cout << "eh: " << eh << std::endl;
  // std::cout << "R: " << R << std::endl;
  // std::cout << "dptMag: " << dptMag << std::endl;
  // std::cout << "dpl: " << dpl << std::endl;
  // std::cout << "pn_reco: " << pn_reco << std::endl;

  return pn_reco;
}

// Find the highest momentum proton in the event between ProtonMinCut and
// ProtonMaxCut MeV with cos(theta_p) > ProtonCosThetaCut
TLorentzVector FitUtils::GetProtonInRange(FitEvent *event, double ProtonMinCut,
                                          double ProtonMaxCut,
                                          double ProtonCosThetaCut) {
  // Get the neutrino
  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  int HMFSProton = 0;
  double HighestMomentum = 0.0;
  // Get the stack of protons
  std::vector<FitParticle *> Protons = event->GetAllFSProton();
  for (size_t i = 0; i < Protons.size(); ++i) {
    if (Protons[i]->p() > ProtonMinCut && Protons[i]->p() < ProtonMaxCut &&
        cos(Protons[i]->P3().Angle(Pnu.Vect())) > ProtonCosThetaCut &&
        Protons[i]->p() > HighestMomentum) {
      HighestMomentum = Protons[i]->p();
      HMFSProton = i;
    }
  }
  // Now get the proton
  TLorentzVector Pprot = Protons[HMFSProton]->fP;
  return Pprot;
}

// Get Cos theta with Adler angles
double FitUtils::CosThAdler(TLorentzVector Pnu, TLorentzVector Pmu,
                            TLorentzVector Ppi, TLorentzVector Pprot) {
  // Get the "resonance" lorentz vector (pion proton system)
  TLorentzVector Pres = Pprot + Ppi;
  // Boost the particles into the resonance rest frame so we can define the
  // x,y,z axis
  Pnu.Boost(-Pres.BoostVector());
  Pmu.Boost(-Pres.BoostVector());
  Ppi.Boost(-Pres.BoostVector());

  // The z-axis is defined as the axis of three-momentum transfer, \vec{k}
  // Also unit normalise the axis
  TVector3 zAxis =
      (Pnu.Vect() - Pmu.Vect()) * (1.0 / ((Pnu.Vect() - Pmu.Vect()).Mag()));

  // Then the angle between the pion in the "resonance" rest-frame and the
  // z-axis is the theta Adler angle
  double costhAdler = cos(Ppi.Vect().Angle(zAxis));

  return costhAdler;
}

// Get phi with Adler angles, a bit more complicated...
double FitUtils::PhiAdler(TLorentzVector Pnu, TLorentzVector Pmu,
                          TLorentzVector Ppi, TLorentzVector Pprot) {
  // Get the "resonance" lorentz vector (pion proton system)
  TLorentzVector Pres = Pprot + Ppi;
  // Boost the particles into the resonance rest frame so we can define the
  // x,y,z axis
  Pnu.Boost(-Pres.BoostVector());
  Pmu.Boost(-Pres.BoostVector());
  Ppi.Boost(-Pres.BoostVector());

  // The z-axis is defined as the axis of three-momentum transfer, \vec{k}
  // Also unit normalise the axis
  TVector3 zAxis =
      (Pnu.Vect() - Pmu.Vect()) * (1.0 / ((Pnu.Vect() - Pmu.Vect()).Mag()));

  // The y-axis is then defined perpendicular to z and muon momentum in the
  // resonance frame
  TVector3 yAxis = Pnu.Vect().Cross(Pmu.Vect());
  yAxis *= 1.0 / double(yAxis.Mag());

  // And the x-axis is then simply perpendicular to z and x
  TVector3 xAxis = yAxis.Cross(zAxis);
  xAxis *= 1.0 / double(xAxis.Mag());

  // Project the pion on to x and y axes
  double x = Ppi.Vect().Dot(xAxis);
  double y = Ppi.Vect().Dot(yAxis);

  double newphi = atan2(y, x) * (180. / M_PI);
  // Convert negative angles to positive
  if (newphi < 0.0)
    newphi += 360.0;

  return newphi;
}

//********************************************************************
double FitUtils::ppInfK(TLorentzVector pmu, double costh, double binding,
                        bool neutrino) {
  //********************************************************************

  // Convert all values to GeV
  // const double V = binding / 1000.;           // binding potential
  // const double mn = PhysConst::mass_neutron;  // neutron mass
  const double mp = PhysConst::mass_proton; // proton mass
  double el = pmu.E() / 1000.;
  // double pl = (pmu.Vect().Mag()) / 1000.;  // momentum of lepton

  double enu = EnuQErec(pmu, costh, binding, neutrino);

  double ep_inf = enu - el + mp;
  double pp_inf = sqrt(ep_inf * ep_inf - mp * mp);

  return pp_inf;
};

//********************************************************************
TVector3 FitUtils::tppInfK(TLorentzVector pmu, double costh, double binding,
                           bool neutrino) {
  //********************************************************************

  // Convert all values to GeV
  // const double V = binding / 1000.;           // binding potential
  // const double mn = PhysConst::mass_neutron;  // neutron mass
  // const double mp = PhysConst::mass_proton;   // proton mass
  double pl_x = pmu.X() / 1000.;
  double pl_y = pmu.Y() / 1000.;
  double pl_z = pmu.Z() / 1000.;

  double enu = EnuQErec(pmu, costh, binding, neutrino);

  TVector3 tpp_inf(-pl_x, -pl_y, -pl_z + enu);

  return tpp_inf;
};

//********************************************************************
double FitUtils::cthpInfK(TLorentzVector pmu, double costh, double binding,
                          bool neutrino) {
  //********************************************************************

  // Convert all values to GeV
  // const double V = binding / 1000.;           // binding potential
  // const double mn = PhysConst::mass_neutron;  // neutron mass
  const double mp = PhysConst::mass_proton; // proton mass
  double el = pmu.E() / 1000.;
  double pl = (pmu.Vect().Mag()) / 1000.; // momentum of lepton

  double enu = EnuQErec(pmu, costh, binding, neutrino);

  double ep_inf = enu - el + mp;
  double pp_inf = sqrt(ep_inf * ep_inf - mp * mp);

  double cth_inf = (enu * enu + pp_inf * pp_inf - pl * pl) / (2 * enu * pp_inf);

  return cth_inf;
};
