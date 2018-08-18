#ifndef UTILITY_CHANNELUTILITY_HXX_SEEN
#define UTILITY_CHANNELUTILITY_HXX_SEEN

#include "core/types.hxx"

#include "exception/exception.hxx"

#include <iostream>

namespace nuis {

namespace utility {
NEW_NUIS_EXCEPT(invalid_channel_conversion);

#define X(A, B)                                                                \
  case B: {                                                                    \
    return nuis::core::Channel_t::A;                                           \
  }

inline core::Channel_t IntToChannel(int mode) {
  switch (mode) {
    NUIS_INTERACTION_CHANNEL_LIST
  default: {
    throw invalid_channel_conversion()
        << "[ERROR]: Failed to parse mode integer " << mode
        << " as a nuis::core::Channel_t.";
  }
  }
}

#undef X

#define X(A, B)                                                                \
  case core::Channel_t::A: {                                                   \
    return B;                                                                  \
  }

inline int ChannelToInt(core::Channel_t mode) {
  switch (mode) {
    NUIS_INTERACTION_CHANNEL_LIST
  default: {
    throw invalid_channel_conversion()
        << "[ERROR]: Attempting to convert "
           "undefined nuis::core::Channel_t to an "
           "integer.";
  }
  }
}

inline bool IsNC(core::Channel_t mode) { return abs(ChannelToInt(mode) > 30); }
inline bool IsCC(core::Channel_t mode) { return !IsNC(mode); }
inline bool IsNu(core::Channel_t mode) { return ChannelToInt(mode) > 0; }
inline bool IsNub(core::Channel_t mode) { return !IsNu(mode); }

} // namespace utility
} // namespace nuis

#endif
