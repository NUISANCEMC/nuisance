#include "utility/EventTopologyUtility.hxx"

#include "event/FullEvent.hxx"

#include "utility/FullEventUtility.hxx"

namespace nuis {
namespace utility {

bool IsCC0Pi(event::FullEvent const &ev) {

  event::Particle ISNu = GetHMISNeutralLepton(ev);
  if (!ISNu) {
    return false;
  }

  event::PDG_t expected_fslep_pdg = ISNu.pdg > 0 ? ISNu.pdg - 1 : ISNu.pdg + 1;


  event::Particle FSLep = GetHMFSParticle(ev, {expected_fslep_pdg});
  if (!FSLep) {
    return false;
  }

  if (GetFSPions(ev).size() || GetFSOthers(ev).size()) {
    return false;
  }

  return true;
}

} // namespace utility
} // namespace nuis
