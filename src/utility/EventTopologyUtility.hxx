#pragma once

#include "utility/PDGCodeUtility.hxx"

namespace nuis {
namespace event {
class FullEvent;
} // namespace event
} // namespace nuis

namespace nuis {
namespace utility {

  bool IsCC0Pi(event::FullEvent const &);
  bool IsCC1Pi(event::FullEvent const &, std::vector<event::PDG_t> PionPDGs = pdgcodes::Pions);
  bool IsCCInc(event::FullEvent const &);

}
}
