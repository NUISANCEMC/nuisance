#ifndef UTILITY_CHANNELUTILITY_HXX_SEEN
#define UTILITY_CHANNELUTILITY_HXX_SEEN

#include "event/types.hxx"

#include "exception/exception.hxx"

#include <iostream>

namespace nuis {

namespace utility {
NEW_NUIS_EXCEPT(invalid_channel_conversion);

#define X(A, B)                                                                \
  case B: {                                                                    \
    return nuis::event::Channel_t::A;                                          \
  }

inline event::Channel_t IntToChannel(int mode) {
  switch (mode) {
    NUIS_INTERACTION_CHANNEL_LIST
  default: {
    throw invalid_channel_conversion()
        << "[ERROR]: Failed to parse mode integer " << mode
        << " as a nuis::event::Channel_t.";
  }
  }
}

#undef X

#define X(A, B)                                                                \
  case event::Channel_t::A: {                                                  \
    return B;                                                                  \
  }

inline int ChannelToInt(event::Channel_t mode) {
  switch (mode) {
    NUIS_INTERACTION_CHANNEL_LIST
  default: {
    throw invalid_channel_conversion()
        << "[ERROR]: Attempting to convert "
           "undefined nuis::event::Channel_t to an "
           "integer.";
  }
  }
}

#undef X

inline bool IsNC(event::Channel_t mode) { return abs(ChannelToInt(mode)) > 30; }
inline bool IsCC(event::Channel_t mode) { return !IsNC(mode); }
inline bool IsNu(event::Channel_t mode) { return ChannelToInt(mode) > 0; }
inline bool IsNub(event::Channel_t mode) { return !IsNu(mode); }
inline bool IsQE(event::Channel_t mode) {
  return ((mode == event::Channel_t::kCCQE) ||
          (mode == event::Channel_t::kNCELP) ||
          (mode == event::Channel_t::kNCELN) ||
          (mode == event::Channel_t::kCCQE_nub) ||
          (mode == event::Channel_t::kNCELP_nub) ||
          (mode == event::Channel_t::kNCELN_nub));
}
inline bool Is2p2h(event::Channel_t mode) {
  return ((mode == event::Channel_t::kCC2p2h) ||
          (mode == event::Channel_t::kNC2p2h) ||
          (mode == event::Channel_t::kCC2p2h_nub) ||
          (mode == event::Channel_t::kNC2p2h_nub));
}
inline bool IsQEL(event::Channel_t mode) {
  return (IsQE(mode) || Is2p2h(mode));
}
inline bool IsNucleonSPP(event::Channel_t mode) {
  return ((mode == event::Channel_t::kCCSPP_PPip) ||
          (mode == event::Channel_t::kCCSPP_PPi0) ||
          (mode == event::Channel_t::kCCSPP_NPip) ||
          (mode == event::Channel_t::kNCSPP_NPi0) ||
          (mode == event::Channel_t::kNCSPP_PPi0) ||
          (mode == event::Channel_t::kNCSPP_PPim) ||
          (mode == event::Channel_t::kCCSPP_NPim_nub) ||
          (mode == event::Channel_t::kCCSPP_NPi0_nub) ||
          (mode == event::Channel_t::kCCSPP_PPim_nub) ||
          (mode == event::Channel_t::kNCSPP_NPi0_nub) ||
          (mode == event::Channel_t::kNCSPP_PPi0_nub) ||
          (mode == event::Channel_t::kNCSPP_PPim_nub) ||
          (mode == event::Channel_t::kNCSPP_NPip_nub));
}
inline bool IsMultiPi(event::Channel_t mode) {
  return ((mode == event::Channel_t::kCCTransitionMPi) ||
          (mode == event::Channel_t::kNCTransitionMPi) ||
          (mode == event::Channel_t::kCCTransitionMPi_nub) ||
          (mode == event::Channel_t::kNCTransitionMPi_nub));
}
inline bool IsDIS(event::Channel_t mode) {
  return ((mode == event::Channel_t::kCCDIS) ||
          (mode == event::Channel_t::kNCDIS) ||
          (mode == event::Channel_t::kCCDIS_nub) ||
          (mode == event::Channel_t::kNCDIS_nub));
}
inline bool IsCoh(event::Channel_t mode) {
  return ((mode == event::Channel_t::kCCCohPi) ||
          (mode == event::Channel_t::kNCCohPi) ||
          (mode == event::Channel_t::kCCCohPi_nub) ||
          (mode == event::Channel_t::kNCCohPi_nub));
}
} // namespace utility
} // namespace nuis

#endif
