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

#ifndef UTILITY_PDGCODEUTILITY_HXX_SEEN
#define UTILITY_PDGCODEUTILITY_HXX_SEEN

#include "core/types.hxx"

namespace nuis {
namespace utility {
namespace pdgcodes {

enum MatterType { kMatter = 1, kMatterAntimatter = 0, kAntimatter = -1 };
static core::PDG_t const kNuMu = 14;
static core::PDG_t const kNuMuBar = -14;

static core::PDG_t const kMu = 13;
static core::PDG_t const kMuPlus = -13;

static core::PDG_t const kNue = 12;
static core::PDG_t const kNueBar = 12;

static core::PDG_t const kPiPlus = 211;
static core::PDG_t const kPiMinus = -211;
static core::PDG_t const kPi0 = 111;

static core::PDG_t const kProton = 2212;
static core::PDG_t const kNeutron = 2112;

static std::vector<core::PDG_t> const ChargedLeptons{11,  kMu,     15,
                                                     -11, kMuPlus, -15};
static std::vector<core::PDG_t> const ChargedLeptons_matter{11, kMu, 15};
static std::vector<core::PDG_t> const ChargedLeptons_antimatter{-11, kMuPlus,
                                                                -15};

static std::vector<core::PDG_t> const NeutralLeptons{kNue,    kNuMu,    16,
                                                     kNueBar, kNuMuBar, -16};
static std::vector<core::PDG_t> const NeutralLeptons_matter{kNue, kNuMu, 16};
static std::vector<core::PDG_t> const NeutralLeptons_antimatter{kNueBar,
                                                                kNuMuBar, -16};

static std::vector<core::PDG_t> const ChargedPions{kPiPlus, kPiMinus};
static std::vector<core::PDG_t> const NeutralPions{kPi0};
static std::vector<core::PDG_t> const Pions{kPiPlus, kPiMinus, kPi0};
static std::vector<core::PDG_t> const Protons{2212, -2122};
static std::vector<core::PDG_t> const Proton_matter{2212};
static std::vector<core::PDG_t> const Proton_antimatter{-2212};
static std::vector<core::PDG_t> const Neutron{kNeutron};
static std::vector<core::PDG_t> const Nucleons{kProton, 2112, -2212};
static std::vector<core::PDG_t> const Nucleons_matter{kProton, kNeutron};
static std::vector<core::PDG_t> const Nucleons_antimatter{-2212, 2112};

static std::vector<core::PDG_t> const CommonParticles{
    11,      kMu,      15,  -11,     kMuPlus,  -15,  kNue,    kNuMu,   16,
    kNueBar, kNuMuBar, -16, kPiPlus, kPiMinus, kPi0, kProton, kNeutron};
} // namespace pdgcodes

inline bool
IsInPDGList(core::PDG_t pdg, std::vector<core::PDG_t> const &MatterList,
            std::vector<core::PDG_t> const &AntiMatterList,
            pdgcodes::MatterType type = pdgcodes::kMatterAntimatter) {
  switch (type) {
  case pdgcodes::kMatter: {
    return std::count(MatterList.begin(), MatterList.end(), pdg);
  }
  case pdgcodes::kMatterAntimatter: {
    return std::count(MatterList.begin(), MatterList.end(), pdg) ||
           std::count(AntiMatterList.begin(), AntiMatterList.end(), pdg);
  }
  case pdgcodes::kAntimatter: {
    return std::count(AntiMatterList.begin(), AntiMatterList.end(), pdg);
  }
  }
}
inline bool
IsNeutralLepton(core::PDG_t pdg,
                pdgcodes::MatterType type = pdgcodes::kMatterAntimatter) {
  return IsInPDGList(pdg, pdgcodes::NeutralLeptons_matter,
                     pdgcodes::NeutralLeptons_antimatter, type);
}
inline bool
IsChargedLepton(core::PDG_t pdg,
                pdgcodes::MatterType type = pdgcodes::kMatterAntimatter) {
  return IsInPDGList(pdg, pdgcodes::ChargedLeptons_matter,
                     pdgcodes::ChargedLeptons_antimatter, type);
}
inline bool IsProton(core::PDG_t pdg,
                     pdgcodes::MatterType type = pdgcodes::kMatterAntimatter) {
  return IsInPDGList(pdg, pdgcodes::Proton_matter, pdgcodes::Proton_antimatter,
                     type);
}
inline bool IsNeutron(core::PDG_t pdg) { return pdg == pdgcodes::Neutron[0]; }
inline bool IsChargedPion(core::PDG_t pdg) {
  return std::count(pdgcodes::ChargedPions.begin(),
                    pdgcodes::ChargedPions.end(), pdg);
}
inline bool IsNeutralPion(core::PDG_t pdg) {
  return std::count(pdgcodes::NeutralPions.begin(),
                    pdgcodes::NeutralPions.end(), pdg);
}
inline bool IsPion(core::PDG_t pdg) {
  return std::count(pdgcodes::Pions.begin(), pdgcodes::Pions.end(), pdg);
}
inline bool IsOther(core::PDG_t pdg) {
  return !std::count(pdgcodes::CommonParticles.begin(),
                     pdgcodes::CommonParticles.end(), pdg);
}

inline core::PDG_t MakeNuclearPDG(size_t A, size_t Z) {
  return 1000 * Z + 10 * A + 1000000000;
}

inline size_t GetA(core::PDG_t pdg) { return ((pdg / 10) % 1000); }
inline size_t GetZ(core::PDG_t pdg) { return ((pdg / 1000) % 1000); }

} // namespace utility
} // namespace nuis

#endif
