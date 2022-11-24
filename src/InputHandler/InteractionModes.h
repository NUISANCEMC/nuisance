// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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
#ifndef INTERACTION_MODES_H
#define INTERACTION_MODES_H

#include <iostream>

namespace InputHandler {

enum InteractionModes {
  kCCQE = 1,
  kCC2p2h = 2,
  kCC1piponp = 11,
  kCC1pi0onn = 12,
  kCC1piponn = 13,
  kCCDiffSPP = 15,
  kCCCoherent = 16,
  kCC1gamma = 17,
  kCCmultipi = 21,
  kCC1etaonn = 22,
  kCC1kaonp = 23,
  kCCDIS = 26,
  kNC1pi0onn = 31,
  kNC1pi0onp = 32,
  kNC1pimonn = 33,
  kNC1piponp = 34,
  kNCDiffSPP = 35,
  kNCCoherent = 36,
  kNC1gamman = 38,
  kNC1gammap = 39,
  kNCmultipi = 41,
  kNC1etaonn = 42,
  kNC1etaonp = 43,
  kNC1kaon0 = 44,
  kNC1kaonp = 45,
  kNCDIS = 46,
  kNCELonp = 51,
  kNCELonn = 52,
  kNC2p2h = 53,
  kNuElectronElastic = 54,
  kInvMuonDecay = 55
};
}

inline std::ostream &operator<<(std::ostream &os,
                                InputHandler::InteractionModes it) {

  switch (it) {
  case InputHandler::kCCQE:
    return os << "CCQE";
  case InputHandler::kCC2p2h:
    return os << "CC2p2h";
  case InputHandler::kCC1piponp:
    return os << "CC1piponp";
  case InputHandler::kCC1pi0onn:
    return os << "CC1pi0onn";
  case InputHandler::kCC1piponn:
    return os << "CC1piponn";
  case InputHandler::kCCCoherent:
    return os << "CCCoherent";
  case InputHandler::kCC1gamma:
    return os << "CC1gamma";
  case InputHandler::kCCmultipi:
    return os << "CCmultipi";
  case InputHandler::kCC1etaonn:
    return os << "CC1etaonn";
  case InputHandler::kCC1kaonp:
    return os << "CC1kaonp";
  case InputHandler::kCCDIS:
    return os << "CCDIS";
  case InputHandler::kNC1pi0onn:
    return os << "NC1pi0onn";
  case InputHandler::kNC1pi0onp:
    return os << "NC1pi0onp";
  case InputHandler::kNC1pimonn:
    return os << "NC1pimonn";
  case InputHandler::kNC1piponp:
    return os << "NC1piponp";
  case InputHandler::kNCCoherent:
    return os << "NCCoherent";
  case InputHandler::kNC1gamman:
    return os << "NC1gamman";
  case InputHandler::kNC1gammap:
    return os << "NC1gammap";
  case InputHandler::kNCmultipi:
    return os << "NCmultipi";
  case InputHandler::kNC1etaonn:
    return os << "NC1etaonn";
  case InputHandler::kNC1etaonp:
    return os << "NC1etaonp";
  case InputHandler::kNC1kaon0:
    return os << "NC1kaon0";
  case InputHandler::kNC1kaonp:
    return os << "NC1kaonp";
  case InputHandler::kNCDIS:
    return os << "NCDIS";
  case InputHandler::kNCELonp:
    return os << "NCELonp";
  case InputHandler::kNCELonn:
    return os << "NCELonn";
  case InputHandler::kNC2p2h:
    return os << "NC2p2h";
  default:
    return os << "UNKNOWN";
  }
  return os << "UNKNOWN";
}

#endif
