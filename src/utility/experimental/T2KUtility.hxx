#pragma once

#include <utility>
#include <cstdlib> //for size_t

namespace nuis {
namespace event {
class FullEvent;
} // namespace event
} // namespace nuis

namespace nuis {
namespace utility {
namespace t2k {

std::pair<bool, size_t> IsCC0Pi_NumProtons(event::FullEvent const &ev);
bool IsCC0PiNp(event::FullEvent const &);
bool IsCC0Pi1p(event::FullEvent const &);
bool IsCC0Pi0p(event::FullEvent const &);
bool IsCC0Pi_STV(event::FullEvent const &);

/// \brief T2K CC1pi+ CH analysis (Raquel's thesis)
/// Has different phase space cuts depending on if using Michel tag or not
///
/// Essentially consists of two samples: one sample which has Michel e (which we
/// can't get pion direction from); this covers backwards angles quite well.
/// Measurements including this sample does not have include pion kinematics
/// cuts one sample which has PID in FGD and TPC and no Michel e. These are
/// mostly forward-going so require a pion kinematics cut
///
///  Essentially, cuts are:
///                          1 negative muon
///                          1 positive pion
///                          Any number of nucleons
///                          No other particles in the final state
bool IsCC1Pip_CH_MichTag(event::FullEvent const &);
bool IsCC1Pip_CH_RecPi(event::FullEvent const &);




} // namespace t2k
} // namespace utility
} // namespace nuis
