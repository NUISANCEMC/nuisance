#include "utility/EventTopologyUtility.hxx"

#include "event/FullEvent.hxx"

#include "utility/FullEventUtility.hxx"

namespace nuis {
namespace utility {

bool IsCC0Pi(event::FullEvent const &ev) {
  if (!IsCCInc(ev)) {
    return false;
  }

  if (GetNFSPions(ev) || GetNFSOthers(ev)) {
    return false;
  }

  return true;
}

bool IsCC1Pi(event::FullEvent const &ev, std::vector<event::PDG_t> PionPDGs) {
  if (!IsCCInc(ev)) {
    return false;
  }

  size_t NChosenPi = GetNParticles(ev, PionPDGs);
  if ((NChosenPi != 1) || (NChosenPi != GetNFSPions(ev)) || GetNFSOthers(ev)) {
    return false;
  }

  return true;
}

bool IsCCInc(event::FullEvent const &ev) {
  event::Particle ISNu = GetHMISNeutralLepton(ev);
  if (!ISNu) {
    return false;
  }

  event::PDG_t expected_fslep_pdg = ISNu.pdg > 0 ? ISNu.pdg - 1 : ISNu.pdg + 1;

  if (GetNParticles(ev, {expected_fslep_pdg}) != GetNFSLeptons(ev)) {
    return false;
  }

  return true;
}

} // namespace utility
} // namespace nuis
