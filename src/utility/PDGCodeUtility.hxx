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

#pragma once

#include "event/types.hxx"

#include "exception/exception.hxx"

#include <vector>

namespace nuis {
namespace utility {
namespace pdgcodes {

NEW_NUIS_EXCEPT(unhandled_pdg_code);

enum MatterType { kMatter = 1, kMatterAntimatter = 0, kAntimatter = -1 };

static event::PDG_t const kDefault = 0;

static event::PDG_t const kNuMu = 14;
static event::PDG_t const kNuMuBar = -14;

static event::PDG_t const kMu = 13;
static event::PDG_t const kMuPlus = -13;

static event::PDG_t const kElectron = 11;
static event::PDG_t const kPositron = -11;

static event::PDG_t const kNue = 12;
static event::PDG_t const kNueBar = -12;

static event::PDG_t const kPiPlus = 211;
static event::PDG_t const kPiMinus = -211;
static event::PDG_t const kPi0 = 111;

static event::PDG_t const kK0Long = 130;
static event::PDG_t const kK0Short = 130;
static event::PDG_t const kK0 = 311;
static event::PDG_t const kKPlus = 321;
static event::PDG_t const kKMinus = -321;

static event::PDG_t const kProton = 2212;
static event::PDG_t const kNeutron = 2112;

static event::PDG_t const kGamma = 22;

static std::vector<event::PDG_t> const ChargedLeptons{kElectron, kMu,     15,
                                                      kPositron, kMuPlus, -15};
static std::vector<event::PDG_t> const ChargedLeptons_matter{kElectron, kMu,
                                                             15};
static std::vector<event::PDG_t> const ChargedLeptons_antimatter{kPositron,
                                                                 kMuPlus, -15};

static std::vector<event::PDG_t> const NeutralLeptons{kNue,    kNuMu,    16,
                                                      kNueBar, kNuMuBar, -16};
static std::vector<event::PDG_t> const NeutralLeptons_matter{kNue, kNuMu, 16};
static std::vector<event::PDG_t> const NeutralLeptons_antimatter{kNueBar,
                                                                 kNuMuBar, -16};
static std::vector<event::PDG_t> const Leptons{
    kElectron, kMu,   15, kPositron, kMuPlus,  -15,
    kNue,      kNuMu, 16, kNueBar,   kNuMuBar, -16};

static std::vector<event::PDG_t> const ChargedPions{kPiPlus, kPiMinus};
static std::vector<event::PDG_t> const NeutralPions{kPi0};
static std::vector<event::PDG_t> const Pions{kPiPlus, kPiMinus, kPi0};
static std::vector<event::PDG_t> const ChargedKaons{kKPlus, kKMinus};
static std::vector<event::PDG_t> const NeutralKaons{kK0, kK0Long, kK0Short};
static std::vector<event::PDG_t> const Kaons{kKPlus, kKMinus, kK0, kK0Long,
                                             kK0Short};
static std::vector<event::PDG_t> const Protons{kProton, -kProton};
static std::vector<event::PDG_t> const Proton_matter{kProton};
static std::vector<event::PDG_t> const Proton_antimatter{-kProton};
static std::vector<event::PDG_t> const Neutron{kNeutron};
static std::vector<event::PDG_t> const Nucleons{kProton, kNeutron, -kProton};
static std::vector<event::PDG_t> const Nucleons_matter{kProton, kNeutron};
static std::vector<event::PDG_t> const Nucleons_antimatter{-kProton, kNeutron};

static std::vector<event::PDG_t> const CommonParticles{
    kElectron, kMu,      15,  kPositron, kMuPlus,  -15,  kNue,    kNuMu,   16,
    kNueBar,   kNuMuBar, -16, kPiPlus,   kPiMinus, kPi0, kProton, kNeutron};
} // namespace pdgcodes

namespace pdgmasses {
static double const kNuMuMass_MeV = 0;
static double const kNuMuBarMass_MeV = 0;

static double const kMuMass_MeV = 105.65;
static double const kMuPlusMass_MeV = 105.65;

static double const kNueMass_MeV = 0;
static double const kNueBarMass_MeV = 0;

static double const kPiPlusMass_MeV = 139.57;
static double const kPiMinusMass_MeV = 139.57;
static double const kPi0Mass_MeV = 134.97;

static double const kNeutronMass_MeV = 939.56;
static double const kProtonMass_MeV = 938.27;
static double const kNucleonAverageMass_MeV = 938.27;
} // namespace pdgmasses

double GetPDGMass(event::PDG_t);

bool IsInPDGList(event::PDG_t pdg, std::vector<event::PDG_t> const &MatterList,
                 std::vector<event::PDG_t> const &AntiMatterList,
                 pdgcodes::MatterType type = pdgcodes::kMatterAntimatter);
bool IsNeutralLepton(event::PDG_t pdg,
                     pdgcodes::MatterType type = pdgcodes::kMatterAntimatter);
bool IsChargedLepton(event::PDG_t pdg,
                     pdgcodes::MatterType type = pdgcodes::kMatterAntimatter);
bool IsProton(event::PDG_t pdg,
              pdgcodes::MatterType type = pdgcodes::kMatterAntimatter);

bool IsNeutron(event::PDG_t pdg);
bool IsChargedPion(event::PDG_t pdg);
bool IsPositivePion(event::PDG_t pdg);
bool IsNegativePion(event::PDG_t pdg);
bool IsNeutralPion(event::PDG_t pdg);
bool IsPion(event::PDG_t pdg);
bool IsChargedKaon(event::PDG_t pdg);
bool IsPositiveKaon(event::PDG_t pdg);
bool IsNegativeKaon(event::PDG_t pdg);
bool IsNeutralKaon(event::PDG_t pdg);
bool IsKaon(event::PDG_t pdg);
bool IsGamma(event::PDG_t pdg);
bool IsOther(event::PDG_t pdg);

bool IsMatter(event::PDG_t pdg);
bool IsAntiMatter(event::PDG_t pdg);

bool IsNuclearPDG(event::PDG_t pdg);

event::PDG_t MakeNuclearPDG(size_t A, size_t Z);

size_t GetA(event::PDG_t);
size_t GetZ(event::PDG_t);

} // namespace utility
} // namespace nuis
