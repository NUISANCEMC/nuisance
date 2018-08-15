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

namespace nuis {
namespace utility {
namespace pdgcodes {
static std::vector<PDG_t> const ChargedLeptons{11, 13, 15, -11, -13, -15};
static std::vector<PDG_t> const ChargedLeptons_matter{11, 13, 15};
static std::vector<PDG_t> const ChargedLeptons_antimatter{-11, -13, -15};

static std::vector<PDG_t> const NeutralLeptons{12, 14, 16, -12, -14, -16};
static std::vector<PDG_t> const NeutralLeptons_matter{12, 14, 16};
static std::vector<PDG_t> const NeutralLeptons_antimatter{-12, -14, -16};

static std::vector<PDG_t> const ChargedPions{211, -211};
static std::vector<PDG_t> const NeutralPions{111};
static std::vector<PDG_t> const Pions{211, -211, 111};
static std::vector<PDG_t> const Protons{2212};
static std::vector<PDG_t> const Neutron{2112};
static std::vector<PDG_t> const Nucleons{2212, 2112};

static std::vector<PDG_t> const CommonParticles{11,  13,   15,  -11,  -13, -15,
                                                12,  14,   16,  -12,  -14, -16,
                                                211, -211, 111, 2212, 2112};
} // namespace pdgcodes
} // namespace utility
} // namespace nuis

#endif
