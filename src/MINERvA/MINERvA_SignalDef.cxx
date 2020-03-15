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
#include "SignalDef.h"

#include "MINERvA_SignalDef.h"
#include "MINERvAUtils.h"

namespace SignalDef {

// *********************************
// MINERvA CC1pi+/- signal definition (2015 release)
// Note:  There is a 2016 release which is different to this (listed below), but
// it is CCNpi+ and has a different W cut
// Note2: The W cut is implemented in the class implementation in MINERvA/
// rather than here so we can draw events that don't pass the W cut (W cut is
// 1.4 GeV)
//        Could possibly be changed for slight speed increase since less events
//        would be used
//
// MINERvA signal is slightly different to MiniBooNE
//
// Exactly one negative muon
// Exactly one charged pion (both + and -); however, there is a Michel e-
// requirement but UNCLEAR IF UNFOLDED OR NOT (so don't know if should be
// applied)
// Exactly 1 charged pion exits (so + and - charge), however, has Michel
// electron requirement, so look for + only here?
// No restriction on neutral pions or other mesons
// MINERvA has unfolded and not unfolded muon phase space for 2015
//
// Possible issues with the data:
// 1) pi- is allowed in signal even when Michel cut included; most pi- is
// efficiency corrected in GENIE 2) There is a T_pi < 350 MeV cut coming from
// requiring a stopping pion; this is efficiency corrected in GENIE 3) There is
// a 1.5 < Enu < 10.0 cut in signal definition 4) There is an angular muon cut
// which is sometimes efficiency corrected (why we have bool isRestricted below)
//
// Nice things:
// Much data given: with and without muon angle cuts and with and without shape
// only data + covariance
//
bool isCC1pip_MINERvA(FitEvent *event, double EnuMin, double EnuMax,
                      bool isRestricted) {
  // *********************************

  // Signal is both pi+ and pi-
  // WARNING: PI- CONTAMINATION IS FULLY GENIE BECAUSE THE MICHEL TAG
  // First, make sure it's CCINC
  if (!isCCINC(event, 14, EnuMin, EnuMax))
    return false;

  // Allow pi+/pi-
  int piPDG[] = {211, -211};
  int nLeptons = event->NumFSLeptons();
  int nPion = event->NumFSParticle(piPDG);

  // Check that the desired pion exists and is the only meson
  if (nPion != 1)
    return false;

  // Check that there is only one final state lepton
  if (nLeptons != 1)
    return false;

  // MINERvA released another CC1pi+ xsec without muon unfolding!
  // here the muon angle is < 20 degrees (seen in MINOS)
  TLorentzVector pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;

  if (isRestricted) {
    double th_nu_mu = FitUtils::th(pmu, pnu) * 180. / M_PI;
    if (th_nu_mu >= 20)
      return false;
  }

  // Extract Hadronic Mass
  double hadMass = FitUtils::Wrec(pnu, pmu);

  // Actual cut is True GENIE Ws! Arg.! Use gNtpcConv definition.
#ifdef __GENIE_ENABLED__
  if (event->fType == kGENIE) {
    EventRecord *gevent = static_cast<EventRecord *>(event->genie_event->event);
    const Interaction *interaction = gevent->Summary();
    const Kinematics &kine = interaction->Kine();
    double Ws = kine.W(true);
    //    std::cout << "Ws versus WRec = " << Ws << " vs " << hadMass << " " <<
    //    kine.W(false) << std::endl;
    hadMass = Ws * 1000.0;
  }
#endif
  if (hadMass > 1400.0)
    return false;

  return true;
};

// Updated MINERvA 2017 Signal using Wexp and no restriction on angle
bool isCC1pip_MINERvA_2017(FitEvent *event, double EnuMin, double EnuMax) {

  // Signal is both pi+ and pi-
  // WARNING: PI- CONTAMINATION IS FULLY GENIE BECAUSE THE MICHEL TAG
  // First, make sure it's CCINC
  if (!isCCINC(event, 14, EnuMin, EnuMax))
    return false;

  // Allow pi+/pi-
  int piPDG[] = {211, -211};
  int nLeptons = event->NumFSLeptons();
  int nPion = event->NumFSParticle(piPDG);

  // Check that the desired pion exists and is the only meson
  if (nPion != 1)
    return false;

  // Check that there is only one final state lepton
  if (nLeptons != 1)
    return false;

  // Get Muon and Lepton Kinematics
  TLorentzVector pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;

  // Extract Hadronic Mass
  double hadMass = FitUtils::Wrec(pnu, pmu);
  // Cut on 2017 data is still 1.4 GeV
  if (hadMass > 1400.0)
    return false;

  return true;
};

// *********************************
// MINERvA CCNpi+/- signal definition from 2016 publication
// Different to CC1pi+/- listed above; additional has W < 1.8 GeV
//
// For notes on strangeness of signal definition, see CC1pip_MINERvA
//
// One negative muon
// At least one charged pion
// 1.5 < Enu < 10
// No restrictions on pi0 or other mesons or baryons
// W_reconstructed (ignoring initial state motion) cut at 1.8 GeV
//
// Also writes number of pions (nPions) if studies on this want to be done...
bool isCCNpip_MINERvA(FitEvent *event, double EnuMin, double EnuMax,
                      bool isRestricted, bool isWtrue) {
  // *********************************

  // First, make sure it's CCINC
  if (!isCCINC(event, 14, EnuMin, EnuMax))
    return false;

  int nLeptons = event->NumFSLeptons();

  // Write the number of pions to the measurement class...
  // Maybe better to just do that inside the class?
  int nPions = event->NumFSParticle(PhysConst::pdg_charged_pions);

  // Check that there is a pion!
  if (nPions == 0)
    return false;

  // Check that there is only one final state lepton
  if (nLeptons != 1)
    return false;

  // Need the muon and the neutrino to check angles and W

  TLorentzVector pnu = event->GetNeutrinoIn()->fP;
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;
  // MINERvA released some data with restricted muon angle
  // Here the muon angle is < 20 degrees (seen in MINOS)
  if (isRestricted) {

    double th_nu_mu = FitUtils::th(pmu, pnu) * 180. / M_PI;
    if (th_nu_mu >= 20.)
      return false;
  }

  // Lastly check the W cut (always at 1.8 GeV)
  double Wrec = FitUtils::Wrec(pnu, pmu) + 0.;

  // Actual cut is True GENIE Ws! Arg.! Use gNtpcConv definition.
  if (isWtrue) {
#ifdef __GENIE_ENABLED__
    if (event->fType == kGENIE) {
      GHepRecord *ghep = static_cast<GHepRecord *>(event->genie_event->event);
      const Interaction *interaction = ghep->Summary();
      const Kinematics &kine = interaction->Kine();
      double Ws = kine.W(true);
      Wrec = Ws * 1000.0; // Say Wrec is Ws
    }
#endif
  }

  if (Wrec > 1800. || Wrec < 0.0)
    return false;

  return true;
};

//********************************************************************
bool isCCQEnumu_MINERvA(FitEvent *event, double EnuMin, double EnuMax,
                        bool fullphasespace) {
  //********************************************************************

  if (!isCCQELike(event, 14, EnuMin, EnuMax))
    return false;

  TLorentzVector pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;

  double ThetaMu = pnu.Vect().Angle(pmu.Vect());
  double Enu_rec = FitUtils::EnuQErec(pmu, cos(ThetaMu), 34., true);

  // If Restricted phase space
  if (!fullphasespace && ThetaMu > 0.34906585)
    return false;

  // restrict energy range
  if (Enu_rec < EnuMin || Enu_rec > EnuMax)
    return false;

  return true;
};

//********************************************************************
bool isCCQEnumubar_MINERvA(FitEvent *event, double EnuMin, double EnuMax,
                           bool fullphasespace) {
  //********************************************************************

  if (!isCCQELike(event, -14, EnuMin, EnuMax))
    return false;

  TLorentzVector pnu = event->GetHMISParticle(-14)->fP;
  TLorentzVector pmu = event->GetHMFSParticle(-13)->fP;

  double ThetaMu = pnu.Vect().Angle(pmu.Vect());
  double Enu_rec = FitUtils::EnuQErec(pmu, cos(ThetaMu), 30., true);

  // If Restricted phase space
  if (!fullphasespace && ThetaMu > 0.34906585)
    return false;

  // restrict energy range
  if (Enu_rec < EnuMin || Enu_rec > EnuMax)
    return false;

  return true;
}

//********************************************************************
bool isCCincLowRecoil_MINERvA(FitEvent *event, double EnuMin, double EnuMax) {
  //********************************************************************

  if (!isCCINC(event, 14, EnuMin, EnuMax))
    return false;

  // Need at least one muon
  if (event->NumFSParticle(13) < 1)
    return false;
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector pnu = event->GetHMISParticle(14)->fP;

  // Cut on muon angle greated than 20deg
  if (cos(pnu.Vect().Angle(pmu.Vect())) < 0.93969262078)
    return false;

  // Cut on muon energy < 1.5 GeV
  if (pmu.E() / 1000.0 < 1.5)
    return false;

  return true;
}

// Used in 2014 muon+proton analysis
// Events with muon angles up to 70 degrees
// One right sign muon, at least one proton, no pions
// proton kinetic energies greater than 100 MeV
bool isCC0pi1p_MINERvA(FitEvent *event, double enumin, double enumax) {
  bool signal =
      (isCC0pi(event, 14, enumin, enumax) &&      // Require numu CC0pi event
       HasProtonKEAboveThreshold(event, 110.0) && // With proton above threshold
       (event->GetHMFSMuon())->P3().Angle((event->GetNeutrinoIn())->P3()) *
               180. / M_PI <
           70 // And muon within production angle
      );

  return signal;
}

// 2015 analysis just asks for 1pi0 and no pi+/pi-
bool isCC1pi0_MINERvA_2015(FitEvent *event, double EnuMin, double EnuMax) {
  bool CC1pi0_anu = SignalDef::isCC1pi(event, -14, 111, EnuMin, EnuMax);
  return CC1pi0_anu;
}

// 2016 analysis just asks for 1pi0 and no other charged tracks. Half-open to
// interpretation: we go with "charged tracks" meaning pions. You'll be forgiven
// for thinking proton tracks should be included here too but we checked with
// MINERvA
bool isCC1pi0_MINERvA_2016(FitEvent *event, double EnuMin, double EnuMax) {
  bool CC1pi0_anu = SignalDef::isCC1pi(event, -14, 111, EnuMin, EnuMax);

  /*
  // Additionally look for charged proton track
  // Comment: This is _NOT_ in the signal definition but was tested
  bool HasProton = event->HasFSParticle(2212);

  if (CC1pi0_anu) {
  if (!HasProton) {
  return true;
  } else {
  return false;
  }
  } else {
  return false;
  }
  */

  return CC1pi0_anu;
}

// 2016 analysis just asks for 1pi0 and no other charged tracks
bool isCC1pi0_MINERvA_nu(FitEvent *event, double EnuMin, double EnuMax) {
  bool CC1pi0_nu = SignalDef::isCC1pi(event, 14, 111, EnuMin, EnuMax);
  return CC1pi0_nu;
}

//********************************************************************
bool isCC0pi_MINERvAPTPZ(FitEvent *event, int nuPDG, double emin, double emax) {
  //********************************************************************
  // Check it's CCINC
  if (!SignalDef::isCCINC(event, nuPDG, emin, emax))
    return false;

  // Make Angle Cut > 20.0
  TLorentzVector pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;
  double th_nu_mu = FitUtils::th(pmu, pnu) * 180. / M_PI;
  if (th_nu_mu >= 20.0)
    return false;

  int genie_n_muons = 0;
  int genie_n_mesons = 0;
  int genie_n_heavy_baryons_plus_pi0s = 0;
  int genie_n_photons = 0;

  for (unsigned int i = 0; i < event->NParticles(); ++i) {
    FitParticle *p = event->GetParticle(i);
    if (p->Status() != kFinalState)
      continue;

    int pdg = p->fPID;
    double energy = p->fP.E();

    if (pdg == 13) {
      genie_n_muons++;
    } else if (pdg == 22 && energy > 10.0) {
      genie_n_photons++;
    } else if (abs(pdg) == 211 || abs(pdg) == 321 || abs(pdg) == 323 ||
               pdg == 111 || pdg == 130 || pdg == 310 || pdg == 311 ||
               pdg == 313 || abs(pdg) == 221 || abs(pdg) == 331) {
      genie_n_mesons++;
    } else if (pdg == 3112 || pdg == 3122 || pdg == 3212 || pdg == 3222 ||
               pdg == 4112 || pdg == 4122 || pdg == 4212 || pdg == 4222 ||
               pdg == 411 || pdg == 421 || pdg == 111) {
      genie_n_heavy_baryons_plus_pi0s++;
    }
  }

  if (genie_n_muons == 1 && genie_n_mesons == 0 &&
      genie_n_heavy_baryons_plus_pi0s == 0 && genie_n_photons == 0)
    return true;
  return false;
}

// **************************************************
// Section VI Event Selection of
// https://journals.aps.org/prd/pdf/10.1103/PhysRevD.97.052002 Anti-neutrino
// charged-current Post-FSI final states without
//                              mesons,
//                              prompt photons above nuclear deexcitation
//                              energies heavy baryons protons above kinetic
//                              energy of 120 MeV
// Muon-neutrino angle of 20 degrees
// Parallel muon momentum: 1.5 GeV < P|| < 15 GeV --- N.B. APPARENTLY NOT
// INCLUDED, see below Transverse muon momentum: pT < 1.5 GeV --- N.B.
// APPARENTLY NOT INCLUDED, see below
bool isCC0pi_anti_MINERvAPTPZ(FitEvent *event, int nuPDG, double emin,
                              double emax) {
  // **************************************************

  // Check it's CCINC
  if (!SignalDef::isCCINC(event, nuPDG, emin, emax))
    return false;
  TLorentzVector pnu = event->GetNeutrinoIn()->fP;
  TLorentzVector pmu = event->GetHMFSParticle(-13)->fP;
  // Make Angle Cut > 20.0
  double th_nu_mu = FitUtils::th(pmu, pnu) * 180. / M_PI;
  if (th_nu_mu >= 20.0)
    return false;

  // Heidi Schellman (schellmh@science.oregonstate.edu) assured me that the p_t
  // and p_z (or p_||) cuts aren't actually implemented as a signal definition:
  // they're only implemented in the binning for p_t and p_z (but not Q2QE and
  // EnuQE)
  /*
  // Cut on pT and pZ
  Double_t px = pmu.X()/1.E3;
  Double_t py = pmu.Y()/1.E3;
  Double_t pt = sqrt(px*px+py*py);

  // Don't want to assume the event generators all have neutrino coming along z
  // pz is muon momentum projected onto the neutrino direction
  Double_t pz = pmu.Vect().Dot(pnu.Vect()*(1.0/pnu.Vect().Mag()))/1.E3;
  if (pz > 15 || pz < 1.5) return false;
  if (pt > 1.5) return false;
  */

  // Find if there are any protons above 120 MeV kinetic energy
  if (HasProtonKEAboveThreshold(event, 120.0))
    return false;

  // Particle counters
  int genie_n_muons = 0;
  int genie_n_mesons = 0;
  int genie_n_heavy_baryons_plus_pi0s = 0;
  int genie_n_photons = 0;
  // Loop over the particles in the event and count them up
  for (unsigned int i = 0; i < event->NParticles(); ++i) {
    FitParticle *p = event->GetParticle(i);
    if (p->Status() != kFinalState)
      continue;

    int pdg = p->fPID;
    double energy = p->fP.E();

    // Any charged muons
    if (abs(pdg) == 13) {
      genie_n_muons++;
      // De-excitation photons
    } else if (pdg == 22 && energy > 10.0) {
      genie_n_photons++;
      // Mesons
    } else if (abs(pdg) == 211 || abs(pdg) == 321 || abs(pdg) == 323 ||
               pdg == 111 || pdg == 130 || pdg == 310 || pdg == 311 ||
               pdg == 313 || abs(pdg) == 221 || abs(pdg) == 331) {
      genie_n_mesons++;
      // Heavy baryons and pi0s
    } else if (abs(pdg) == 3112 || abs(pdg) == 3122 || abs(pdg) == 3212 ||
               abs(pdg) == 3222 || abs(pdg) == 4112 || abs(pdg) == 4122 ||
               abs(pdg) == 4212 || abs(pdg) == 4222 || abs(pdg) == 411 ||
               abs(pdg) == 421 || abs(pdg) == 111) {
      genie_n_heavy_baryons_plus_pi0s++;
    }
  }

  // Look for one muon with no mesons, heavy baryons or deexcitation photons
  if (genie_n_muons == 1 && genie_n_mesons == 0 &&
      genie_n_heavy_baryons_plus_pi0s == 0 && genie_n_photons == 0)
    return true;
  return false;
}

// MINERvA CC0pi transverse variables signal defintion
bool isCC0piNp_MINERvA_STV(FitEvent *event, double EnuMin, double EnuMax) {

  // Require a numu CC0pi event
  if (!isCC0pi(event, 14, EnuMin, EnuMax))
    return false;

  // Require at least one FS proton
  if (event->NumFSParticle(2212) == 0)
    return false;

  TLorentzVector pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;

  // Muon momentum cuts
  if (pmu.Vect().Mag() < 1500 || pmu.Vect().Mag() > 10000)
    return false;
  // Muon angle cuts
  if (pmu.Vect().Angle(pnu.Vect()) > (M_PI / 180.0) * 20.0)
    return false;

  const double ctheta_cut = cos((M_PI / 180.0) * 70.0);
  //Did you find a proton in the PS?
  if (MINERvAUtils::GetProtonInRange(event, 450, 1200, ctheta_cut).E() == 0)
    return false;
  return true;
};

} // namespace SignalDef
