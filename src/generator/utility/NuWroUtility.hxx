#ifndef GENERATOR_UTILITY_NUWROUTILITY_HXX_SEEN
#define GENERATOR_UTILITY_NUWROUTILITY_HXX_SEEN

#include "event/MinimalEvent.hxx"
#include "event/types.hxx"

#include "event1.h"
using NuWroFlags = ::flags;
using NuWroEvent = ::event;

#include <utility>

namespace nuis {
namespace nuwrotools {
std::pair<NuWroFlags, int> GetFlagsDynEquivalent(nuis::event::Channel_t);
nuis::event::Channel_t NuWroEventChannel(NuWroEvent const &);
} // namespace nuwrotools
} // namespace nuis

#endif
