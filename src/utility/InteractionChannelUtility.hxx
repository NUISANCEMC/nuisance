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
    return nuis::event::Channel_t::A;                                           \
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
  case event::Channel_t::A: {                                                   \
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

inline bool IsNC(event::Channel_t mode) { return abs(ChannelToInt(mode) > 30); }
inline bool IsCC(event::Channel_t mode) { return !IsNC(mode); }
inline bool IsNu(event::Channel_t mode) { return ChannelToInt(mode) > 0; }
inline bool IsNub(event::Channel_t mode) { return !IsNu(mode); }
inline bool IsCoh(event::Channel_t mode) {
  return ((mode == event::Channel_t::kCCCohPi) ||
          (mode == event::Channel_t::kNCCohPi) ||
          (mode == event::Channel_t::kCCCohPi_nub) ||
          (mode == event::Channel_t::kNCCohPi_nub));
}

} // namespace utility
} // namespace nuis

#endif
