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

#include "generator/utility/GENIEUtility.hxx"

#include "utility/PDGCodeUtility.hxx"

#ifdef GENIE_V3_INTERFACE
#include "Framework/GHEP/GHepParticle.h"
#include "Framework/GHEP/GHepRecord.h"
#include "Framework/GHEP/GHepUtils.h"
#include "Framework/ParticleData/PDGCodes.h"
#else
#include "GHEP/GHepParticle.h"
#include "GHEP/GHepRecord.h"
#include "GHEP/GHepUtils.h"
#include "PDG/PDGCodes.h"
#endif

#include "TGraph.h"

namespace nuis {

using namespace event;

namespace genietools {

static std::map<std::string, TGraph> SplineCache;

TGraph dum;

TGraph const &GetGENIESpline(std::string const &SplineFile,
                             std::string const &SplineIdentifier) {
  if (SplineCache.find(SplineFile + SplineIdentifier) != SplineCache.end()) {
    return SplineCache.find(SplineFile + SplineIdentifier)->second;
  }
  return dum;
}

struct NFSParticleCount {
  size_t NProton;
  size_t NNeutron;
  size_t NPip;
  size_t NPi0;
  size_t NPim;
  size_t NOther;
};

NFSParticleCount CountPreFSIParticles(genie::GHepRecord const &ev) {
  // This code in this method is adapted from the GENIE source code found in
  // GHep/GHepUtils.cxx This method therefore carries the GENIE copyright
  // licence as copied below:
  //
  /// Copyright (c) 2003-2017, GENIE Neutrino MC Generator Collaboration
  /// For the full text of the license visit http://copyright.genie-mc.org
  /// or see $GENIE/LICENSE
  //

  genie::Target const &tgt = ev.Summary()->InitState().Tgt();
  if (!tgt.HitNucIsSet()) {
    throw invalid_GENIE_event()
        << "[ERROR]: Failed to get hit nucleon kinematics as it was not "
           "included in this GHep event. This is a fatal error.";
  }

  genie::GHepParticle *FSLep = ev.FinalStatePrimaryLepton();
  genie::GHepParticle *ISLep = ev.Probe();

  if (!FSLep || !ISLep) {
    throw invalid_GENIE_event()
        << "[ERROR]: Failed to find IS and FS lepton in event: "
        << ev.Summary()->AsString();
  }

  size_t NPi0 = 0, NPip = 0, NPim = 0, NProton = 0, NNeutron = 0, NOther = 0;

  bool nuclear_target = tgt.IsNucleus();

  TIter event_iter(&ev);
  genie::GHepParticle *p = 0;

  while ((p = dynamic_cast<genie::GHepParticle *>(event_iter.Next()))) {
    genie::GHepStatus_t ghep_ist = (genie::GHepStatus_t)p->Status();
    int ghep_pdgc = p->Pdg();
    int ghep_fm = p->FirstMother();
    int ghep_fmpdgc = (ghep_fm == -1) ? 0 : ev.Particle(ghep_fm)->Pdg();

    // For nuclear targets use hadrons marked as 'hadron in the nucleus'
    // which are the ones passed in the intranuclear rescattering
    // For free nucleon targets use particles marked as 'final state'
    // but make an exception for decayed pi0's,eta's (count them and not their
    // daughters)

    bool decayed =
        (ghep_ist == genie::kIStDecayedState &&
         (ghep_pdgc == genie::kPdgPi0 || ghep_pdgc == genie::kPdgEta));
    bool parent_included =
        (ghep_fmpdgc == genie::kPdgPi0 || ghep_fmpdgc == genie::kPdgEta);

    bool count_it =
        (nuclear_target && ghep_ist == genie::kIStHadronInTheNucleus) ||
        (!nuclear_target && decayed) ||
        (!nuclear_target && ghep_ist == genie::kIStStableFinalState &&
         !parent_included);

    if (!count_it) {
      continue;
    }

    if (ghep_pdgc == genie::kPdgPiP) {
      NPip++;
    } else if (ghep_pdgc == genie::kPdgPiM) {
      NPim++;
    } else if (ghep_pdgc == genie::kPdgPi0) {
      NPi0++;
    } else if (ghep_pdgc == genie::kPdgProton) {
      NProton++;
    } else if (ghep_pdgc == genie::kPdgNeutron) {
      NNeutron++;
    } else if (!utility::IsNeutralLepton(
                   ghep_pdgc, utility::pdgcodes::kMatterAntimatter) &&
               !utility::IsChargedLepton(
                   ghep_pdgc, utility::pdgcodes::kMatterAntimatter)) {
      NOther++;
    }
  }

  return NFSParticleCount{NProton, NNeutron, NPip, NPi0, NPim, NOther};
}

Channel_t GetEventChannel(genie::GHepRecord const &gev) {
  // Electron Scattering
  if (gev.Summary()->ProcInfo().IsEM()) {
    if (gev.Summary()->InitState().ProbePdg() == utility::pdgcodes::kElectron) {
      if (gev.Summary()->ProcInfo().IsQuasiElastic()) {
        NFSParticleCount fsparts = CountPreFSIParticles(gev);
        if (fsparts.NProton) {
          return Channel_t::kNCELP;
        } else {
          return Channel_t::kNCELN;
        }
      } else if (gev.Summary()->ProcInfo().IsMEC()) {
        return Channel_t::kNC2p2h;
      } else if (gev.Summary()->ProcInfo().IsResonant()) {

        NFSParticleCount fsparts = CountPreFSIParticles(gev);
        if (fsparts.NOther ||
            ((fsparts.NPip + fsparts.NPi0 + fsparts.NPim) > 1)) {
          return Channel_t::kNCTransitionMPi;
        } else if (fsparts.NPip) {
          return Channel_t::kNCSPP_NPip;
        } else if (fsparts.NPi0) {
          return fsparts.NProton ? Channel_t::kNCSPP_PPi0
                                 : Channel_t::kNCSPP_NPi0;
        } else if (fsparts.NPim) {
          return Channel_t::kNCSPP_PPim;
        }

        return Channel_t::kNCTransitionMPi;

      } else if (gev.Summary()->ProcInfo().IsDeepInelastic()) {
        return Channel_t::kNCDIS;
      } else {
        std::cout << "Unknown GENIE Electron Scattering Mode!" << std::endl
                  << "ScatteringTypeId = "
                  << gev.Summary()->ProcInfo().ScatteringTypeId() << " "
                  << "InteractionTypeId = "
                  << gev.Summary()->ProcInfo().InteractionTypeId() << std::endl
                  << genie::ScatteringType::AsString(
                         gev.Summary()->ProcInfo().ScatteringTypeId())
                  << " "
                  << genie::InteractionType::AsString(
                         gev.Summary()->ProcInfo().InteractionTypeId())
                  << " " << gev.Summary()->ProcInfo().IsMEC() << std::endl;
        return Channel_t::kUndefined;
      }
    }

    // Weak CC
  } else if (gev.Summary()->ProcInfo().IsWeakCC()) {
    // CC MEC
    if (gev.Summary()->ProcInfo().IsMEC()) {
      if (utility::IsNeutralLepton(gev.Summary()->InitState().ProbePdg(),
                                   utility::pdgcodes::kMatter)) {
        return Channel_t::kCC2p2h;
      } else if (utility::IsNeutralLepton(gev.Summary()->InitState().ProbePdg(),
                                          utility::pdgcodes::kAntimatter)) {
        return Channel_t::kCC2p2h_nub;
      }
      // CC OTHER
    } else {
      return FromNEUTCode(genie::utils::ghep::NeutReactionCode(&gev));
    }

    // Weak NC
  } else if (gev.Summary()->ProcInfo().IsWeakNC()) {
    // NC MEC
    if (gev.Summary()->ProcInfo().IsMEC()) {
      if (utility::IsNeutralLepton(gev.Summary()->InitState().ProbePdg(),
                                   utility::pdgcodes::kMatter)) {
        return Channel_t::kNC2p2h;
      } else if (utility::IsNeutralLepton(gev.Summary()->InitState().ProbePdg(),
                                          utility::pdgcodes::kAntimatter)) {
        return Channel_t::kNC2p2h_nub;
      }

      // NC OTHER
    } else {
      return FromNEUTCode(genie::utils::ghep::NeutReactionCode(&gev));
    }
  }

  return Channel_t::kUndefined;
}

Particle::Status_t GetParticleStatus(genie::GHepParticle const &p,
                                       Channel_t chan) {
  /*
     kIStUndefined                  = -1,
     kIStInitialState               =  0,   / generator-level initial state /
     kIStStableFinalState           =  1,   / generator-level final state:
     particles to be tracked by detector-level MC /
     kIStIntermediateState          =  2,
     kIStDecayedState               =  3,
     kIStCorrelatedNucleon          = 10,
     kIStNucleonTarget              = 11,
     kIStDISPreFragmHadronicState   = 12,
     kIStPreDecayResonantState      = 13,
     kIStHadronInTheNucleus         = 14,   / hadrons inside the nucleus: marked
     for hadron transport modules to act on /
     kIStFinalStateNuclearRemnant   = 15,   / low energy nuclear fragments
     entering the record collectively as a 'hadronic blob' pseudo-particle /
     kIStNucleonClusterTarget       = 16,   // for composite nucleons before
     phase space decay
     */

  Particle::Status_t state = Particle::Status_t::kUnknown;
  switch (p.Status()) {
  case genie::kIStNucleonTarget:
  case genie::kIStInitialState:
  case genie::kIStCorrelatedNucleon:
  case genie::kIStNucleonClusterTarget: {
    state = Particle::Status_t::kPrimaryInitialState;
    break;
  }

  case genie::kIStStableFinalState: {
    state = Particle::Status_t::kNuclearLeaving;
    break;
  }

  case genie::kIStHadronInTheNucleus: {
    state = Is2p2h(chan) ? Particle::Status_t::kPrimaryInitialState
                         : Particle::Status_t::kIntermediate;
    break;
  }

  case genie::kIStPreDecayResonantState:
  case genie::kIStDISPreFragmHadronicState:
  case genie::kIStIntermediateState: {
    state = Particle::Status_t::kIntermediate;
    break;
  }

  case genie::kIStFinalStateNuclearRemnant:
  case genie::kIStUndefined:
  case genie::kIStDecayedState:
  default: { state = Particle::Status_t::kUnknown; }
  }

  if (utility::IsNuclearPDG(p.Pdg())) {
    if (state == Particle::Status_t::kPrimaryInitialState) {
      state = Particle::Status_t::kPrimaryInitialState;
    } else if (state == Particle::Status_t::kNuclearLeaving) {
      state = Particle::Status_t::kPrimaryFinalState;
    }
  }
  return state;
}

} // namespace genietools
} // namespace nuis
