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
#ifndef EVENT_TYPES_HXX_SEEN
#define EVENT_TYPES_HXX_SEEN

#include "exception/exception.hxx"

namespace nuis {
namespace event {

// Modelled off the NEUT interaction codes.
#define NUIS_INTERACTION_CHANNEL_LIST                                          \
  X(kCCQE, 1)                                                                  \
  X(kCC2p2h, 2)                                                                \
  X(kCCSPP_PPip, 11)                                                           \
  X(kCCSPP_PPi0, 12)                                                           \
  X(kCCSPP_NPip, 13)                                                           \
  X(kCCDiffPP, 15)                                                             \
  X(kCCCohPi, 16)                                                              \
  X(kCCResGamma, 17)                                                           \
  X(kCCTransitionMPi, 21)                                                      \
  X(kCCResEta0, 22)                                                            \
  X(kCCResK, 23)                                                               \
  X(kCCDIS, 26)                                                                \
                                                                               \
  X(kNCSPP_NPi0, 31)                                                           \
  X(kNCSPP_PPi0, 32)                                                           \
  X(kNCSPP_PPim, 33)                                                           \
  X(kNCSPP_NPip, 34)                                                           \
  X(kNCDiffPP, 35)                                                             \
  X(kNCCohPi, 36)                                                              \
  X(kNCResNGamma, 38)                                                          \
  X(kNCResPGamma, 39)                                                          \
  X(kNCTransitionMPi, 41)                                                      \
  X(kNCResNEta0, 42)                                                           \
  X(kNCResPEta0, 43)                                                           \
  X(kNCResK0, 44)                                                              \
  X(kNCResKp, 45)                                                              \
  X(kNCDIS, 46)                                                                \
  X(kNCELP, 51)                                                                \
  X(kNCELN, 52)                                                                \
  X(kNC2p2h, 53)                                                               \
                                                                               \
  X(kCCQE_nub, -1)                                                             \
  X(kCC2p2h_nub, -2)                                                           \
  X(kCCSPP_NPim_nub, -11)                                                      \
  X(kCCSPP_NPi0_nub, -12)                                                      \
  X(kCCSPP_PPim_nub, -13)                                                      \
  X(kCCDiffPP_nub, -15)                                                        \
  X(kCCCohPi_nub, -16)                                                         \
  X(kCCResGamma_nub, -17)                                                      \
  X(kCCTransitionMPi_nub, -21)                                                 \
  X(kCCResEta0_nub, -22)                                                       \
  X(kCCResK_nub, -23)                                                          \
  X(kCCDIS_nub, -26)                                                           \
                                                                               \
  X(kNCSPP_NPi0_nub, -31)                                                      \
  X(kNCSPP_PPi0_nub, -32)                                                      \
  X(kNCSPP_PPim_nub, -33)                                                      \
  X(kNCSPP_NPip_nub, -34)                                                      \
  X(kNCDiffPP_nub, -35)                                                        \
  X(kNCCohPi_nub, -36)                                                         \
  X(kNCResNGamma_nub, -38)                                                     \
  X(kNCResPGamma_nub, -39)                                                     \
  X(kNCTransitionMPi_nub, -41)                                                 \
  X(kNCResNEta0_nub, -42)                                                      \
  X(kNCResPEta0_nub, -43)                                                      \
  X(kNCResK0_nub, -44)                                                         \
  X(kNCResKp_nub, -45)                                                         \
  X(kNCDIS_nub, -46)                                                           \
  X(kNCELP_nub, -51)                                                           \
  X(kNCELN_nub, -52)                                                           \
  X(kNC2p2h_nub, -53)                                                          \
                                                                               \
  X(kUndefined, 0)

#define X(A, B) A = B,
enum class Channel_t { NUIS_INTERACTION_CHANNEL_LIST };
#undef X

#define X(A, B)                                                                \
  case B: {                                                                    \
    return Channel_t::A;                                                       \
  }
inline Channel_t FromNEUTCode(int nc) {
  switch (nc) {
    NUIS_INTERACTION_CHANNEL_LIST
  default: { return Channel_t::kUndefined; }
  }
}
#undef X

using PDG_t = long;

} // namespace event
} // namespace nuis

#define X(A, B)                                                                \
  case nuis::event::Channel_t::A: {                                            \
    return os << #A;                                                           \
  }

inline std::ostream &operator<<(std::ostream &os, nuis::event::Channel_t te) {
  switch (te) { NUIS_INTERACTION_CHANNEL_LIST }
  return os;
}
#undef X
#endif
