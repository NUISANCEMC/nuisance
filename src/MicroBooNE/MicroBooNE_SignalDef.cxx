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

#include <cmath>

#include "MicroBooNE_SignalDef.h"
#include "FitUtils.h"

namespace SignalDef {
  namespace MicroBooNE {

    //----------------------------------------//

bool isCC1MuNp(FitEvent* event, double EnuMin, double EnuMax) {
  // Check CC inclusive
  if (!SignalDef::isCCINC(event, 14, EnuMin, EnuMax)) return false;

  // Veto events which don't have exactly 1 FS muon
  if (event->NumFSMuon() != 1) return false;

  // Veto events with FS mesons
  if (event->NumFSPions() != 0) return false;

  // Veto events with FS electrons
  if (event->NumFSElectron() != 0) return false;

  // Veto events with FS photons
  if (event->NumFSPhoton() != 0) return false;

  // Muon momentum above threshold
  if (event->GetHMFSParticle(13)->fP.Vect().Mag() < 100) return false;

  // Leading proton within momentum range
  if (event->NumFSParticle(2212) == 0) return false;
  double plead = event->GetHMFSParticle(2212)->fP.Vect().Mag();
  if (plead > 300 && plead < 1200) return true;

  return false;
}

    //----------------------------------------//

std::vector<FitParticle*> GetCC1Mu1pProtonsInPS(FitEvent* event){
  std::vector<FitParticle*> protons_in_ps;
  for (auto proton : event->GetAllFSParticle(2212)) {
    double mom = proton->p();
    if (mom > 300 && mom < 1000) { protons_in_ps.push_back(proton); }
  }
  return protons_in_ps;
}

bool isCC1Mu1p(FitEvent* event, double EnuMin, double EnuMax) {
  // Check CC inclusive
  if (!SignalDef::isCCINC(event, 14, EnuMin, EnuMax)) return false;

  // Veto events which don't have exactly 1 FS muon
  if (event->NumFSMuon() != 1) return false;

  // Muon momentum range
  auto mu_mom = event->GetHMFSParticle(13)->p();
  if ((mu_mom < 100) || (mu_mom > 1200)) return false;

  // Check for only a single proton in the momentum PS
  if (GetCC1Mu1pProtonsInPS(event).size() != 1) { return false; }

  // Reject events with neutral pions of any momenta
  if (event->NumFSParticle(111) != 0) return false;

  // Reject events with positively charged pions above 70 MeV/c
  if (event->NumFSParticle(211) != 0) {
    double ppiplus = event->GetHMFSParticle(211)->p();
    if (ppiplus > 70) { return false; }
  }

  // Reject events with negatively charged pions above 70 MeV/c
  if (event->NumFSParticle(-211) != 0) {
    double ppiminus = event->GetHMFSParticle(-211)->p();
    if (ppiminus > 70) { return false; }
  }

  return true;
}

bool isCC1mu2p0pi(FitEvent* event, double EnuMin, double EnuMax) {

  // ==========================================================================
  // Check CC inclusive
  if (!SignalDef::isCCINC(event, 14, EnuMin, EnuMax)) return false;

  // ==========================================================================
  // Veto events which don't have exactly one muon with momentum 0.1 < X (GeV) < 1.2

  if (event->NumFSMuon() != 1) return false;
  double MuonMomentum = event->GetHMFSParticle(13)->fP.Vect().Mag();
  if (!(MuonMomentum > 100. && MuonMomentum < 1200.)) return false;

  // ==========================================================================
  // Veto events which don't have exactly 2 FS protons with momentum 0.3 > X(GeV) > 1.0
  std::vector<FitParticle*> ProtonParticles = event->GetAllFSProton();

  double ProtonMomMinThreshold = 300.; //MeV
  double ProtonMomMaxThreshold = 1000.0; //MeV

  uint nProtons = 0;
  for (uint i=0;i<ProtonParticles.size();i++) {
    double ProtonMom = ProtonParticles[i]->fP.Vect().Mag();
    if (ProtonMom > ProtonMomMinThreshold && ProtonMom < ProtonMomMaxThreshold) {
      nProtons += 1;
    }
  }
  if (nProtons != 2) return false;

  // ==========================================================================
  // Veto events with any charged pions with momentum > 65MeV
  std::vector<FitParticle*> PiPlusParticles = event->GetAllFSPiPlus();
  std::vector<FitParticle*> PiMinusParticles = event->GetAllFSPiMinus();

  std::vector<FitParticle*> ChargedPionParticles;
  for (uint i=0;i<PiPlusParticles.size();i++) {ChargedPionParticles.push_back(PiPlusParticles[i]);}
  for (uint i=0;i<PiMinusParticles.size();i++) {ChargedPionParticles.push_back(PiMinusParticles[i]);}

  double ChargedPionMomThreshold = 65.0;
  uint nChargedPionsWithMomAboveThreshold = 0;
  for (uint i=0;i<ChargedPionParticles.size();i++) {
    if (ChargedPionParticles[i]->fP.Vect().Mag() > ChargedPionMomThreshold) {
      nChargedPionsWithMomAboveThreshold += 1;
    }
  }
  if (nChargedPionsWithMomAboveThreshold != 0) return false;

  // ==========================================================================
  // Veto events with any neutral pions
  std::vector<FitParticle*> PiZeroParticles = event->GetAllFSPiZero();
  uint nNeutralPions = PiZeroParticles.size();
  if (nNeutralPions != 0) return false;

  // ==========================================================================
  // Events which have not yet failed the selection are defined as CC1mu2p0pi
  return true;
}


bool isNCpi0(FitEvent* event) {

  // Check that we have a NC event
  int nu_pdg = event->GetBeamNeutrinoPDG();
  if( !event->HasFSParticle(nu_pdg) ) return false;

  // Check that we have a pi0 in ther right momentum range
  if (event->NumFSParticle(111) != 1) return false;
  double p = event->GetHMFSParticle(111)->fP.Vect().Mag();
  if (p > 1200 || p<=0) return false;
  return true;

}


bool isNueCC0piNp(FitEvent* event, double EnuMin, double EnuMax) {
  // Check CC nue
  if (!SignalDef::isCCINC(event, 12, EnuMin, EnuMax)) return false;

  // Veto events which don't have exactly 1 FS electron
  if (event->NumFSParticle(11) != 1 ) return false;

  // Veto events where the electron doesn't have KE > 30 MeV
  if (event->GetHMFSParticle(11)->KE() <= 30.0) return false;

  // Veto events with neutral pions of any energy
  if (event->NumFSParticle(111) != 0) return false;

  // Veto events with charged pions with KE > 40MeV
  if (event->NumFSParticle(211) != 0 && event->GetHMFSParticle(211)->KE() > 40.0) return false;
  if (event->NumFSParticle(-211) != 0 && event->GetHMFSParticle(-211)->KE() > 40.0) return false;

  // Np events must have at least 1 proton in FS with KE >= 50MeV
  if (event->NumFSParticle(2212) == 0) return false;
  if (event->GetHMFSParticle(2212)->KE() < 50.0) return false; 

  // Events that pass selection are NueCC0piNp
  return true;
}

bool isNueCC0piProtonKE(FitEvent* event, double EnuMin, double EnuMax) {
  // Check CC nue
  if (!SignalDef::isCCINC(event, 12, EnuMin, EnuMax)) return false;
  
  // Veto events which don't have exactly 1 FS electron
  if (event->NumFSParticle(11) != 1 ) return false;
  
  // Veto events where the electron doesn't have KE > 30 MeV
  if (event->GetHMFSParticle(11)->KE() <= 30.0) return false;
  
  // Veto events with neutral pions of any energy
  if (event->NumFSParticle(111) != 0) return false;
  
  // Veto events with charged pions with KE > 40MeV
  if (event->NumFSParticle(211) != 0 && event->GetHMFSParticle(211)->KE() > 40.0) return false;
  if (event->NumFSParticle(-211) != 0 && event->GetHMFSParticle(-211)->KE() > 40.0) return false;

  // Proton KE distribution includes Np and 0p events
  // 0p events either have no final state proton or the leading proton has KE < 50MeV
  if ((event->NumFSParticle(2212) != 0 && event->GetHMFSParticle(2212)->KE() < 50.) || event->NumFSParticle(2212) == 0 ) {
    // additional phase space requirements on FS electron 
    // Electron energy > 0.5 GeV and cos th_e > 0.6
    TLorentzVector pe = event->GetHMFSParticle(11)->fP;
    if (pe.E() <= 500. || pe.CosTheta() < 0.6 ) return false;
  }
  
  // Events that pass selection are either NueCC0piNp or NueCC0pi0p
  return true;
}

// Helper function that returns true if a given PDG code represents a meson or
// antimeson. Otherwise returns false. Based on points 10, 12, and 13 of the
// Particle Data Group's "Monte Carlo Particle Numbering Scheme"
// (2019 revision).
bool isMesonOrAntimeson( int pdg_code ) {

  // Ignore differences between mesons and antimesons for this test. Mesons
  // will have positive PDG codes, while antimesons will have negative ones.
  int abs_pdg = std::abs( pdg_code );

  // Meson PDG codes have no more than seven digits. Seven-digit
  // codes beginning with "99" are reserved for generator-specific
  // particles
  if ( abs_pdg >= 9900000 ) return false;

  // Mesons have a value of zero for $n_{q1}$, the thousands digit
  int thousands_digit = ( abs_pdg / 1000 ) % 10;
  if ( thousands_digit != 0 ) return false;

  // They also have a nonzero value for $n_{q2}$, the hundreds digit
  int hundreds_digit = ( abs_pdg / 100 ) % 10;
  if ( hundreds_digit == 0 ) return false;

  // Reserved codes for Standard Model parton distribution functions
  if ( abs_pdg >= 901 && abs_pdg <= 930 ) return false;

  // Reggeon and pomeron
  if ( abs_pdg == 110 || abs_pdg == 990 ) return false;

  // Reserved codes for GEANT tracking purposes
  if ( abs_pdg == 998 || abs_pdg == 999 ) return false;

  // Reserved code for generator-specific pseudoparticles
  if ( abs_pdg == 100 ) return false;

  // If we've passed all of the tests above, then the particle is a meson
  return true;
}

// Updated signal definition used for the newer 2025 CCNp cross-section analysis.
// It is similar to the original isCC1MuNp definition but has a few small
// differences.
bool isCC1MuNpFor2025Analysis( FitEvent* event, double EnuMin, double EnuMax ) {

  // PDG codes of interest
  const int MUON = 13;
  const int MUON_NEUTRINO = 14;
  const int PROTON = 2212;

  // Require the event to be a numu CC inclusive interaction
  if ( !SignalDef::isCCINC(event, MUON_NEUTRINO, EnuMin, EnuMax) ) return false;

  // Require at least one proton in the final state
  if ( event->NumFSProton() < 1 ) return false;

  // Impose kinematic limits in the signal definition
  double p_mu = event->GetHMFSParticle( MUON )->fP.Vect().Mag(); // MeV
  double p_lead_p = event->GetHMFSParticle( PROTON )->fP.Vect().Mag(); // MeV

  // The muon momentum must be at least 100 MeV/c and at most 1.2 GeV/c
  if ( p_mu < 100. || p_mu > 1200. ) return false;

  // The leading proton momentum must lie on the interval [250, 1000] MeV/c
  if ( p_lead_p < 250. || p_lead_p > 1000. ) return false;

  // Veto events with final-state mesons or antimesons of any kind. Do this by
  // looping over every particle in the event.
  size_t num_particles = event->NParticles();
  for ( size_t p = 0u; p < num_particles; ++p ) {
    // Skip the check for any particle that is not part of the final state
    int state = event->GetParticleState( p );
    if ( state != kFinalState ) continue;
    // Otherwise, decide whether it is a meson or antimeson. If it is, then
    // veto the event.
    int pdg = event->GetParticlePDG( p );
    bool is_meson = isMesonOrAntimeson( pdg );
    if ( is_meson ) return false;
  }

  // If we've made it here, then the current event has passed all of the
  // requirements in the signal definition
  return true;
}

  }  // namespace MicroBooNE
}  // namespace SignalDef
