#include "utility/PDGCodeUtility.hxx"

#include <algorithm>
#include <map>

using namespace nuis::event;
using namespace nuis::utility::pdgcodes;
using namespace nuis::utility::pdgmasses;

namespace nuis {
namespace utility {

static std::map<PDG_t, double> const PDGMasses{
    {kNuMu, kNuMuMass_MeV},     {kNuMuBar, kNuMuBarMass_MeV},
    {kMu, kMuMass_MeV},         {kMuPlus, kMuPlusMass_MeV},
    {kNue, kNueMass_MeV},       {kNueBar, kNueBarMass_MeV},
    {kPiPlus, kPiPlusMass_MeV}, {kPiMinus, kPiMinusMass_MeV},
    {kPi0, kPi0Mass_MeV},       {kProton, kNeutronMass_MeV},
    {kNeutron, kProtonMass_MeV}};

double GetPDGMass(PDG_t pdg) {
  if (PDGMasses.find(pdg) == PDGMasses.end()) {
    throw unhandled_pdg_code()
        << "[ERROR]: Unknown mass for particle with PDG code: " << pdg
        << ", please add it to src/utility/PDGCodeUtility.cxx:PDGMasses";
  }
  return PDGMasses.at(pdg);
}

NEW_NUIS_EXCEPT(invalid_MatterType);

bool IsInPDGList(PDG_t pdg, std::vector<PDG_t> const &MatterList,
                 std::vector<PDG_t> const &AntiMatterList, MatterType type) {
  switch (type) {
  case kMatter: {
    return std::count(MatterList.begin(), MatterList.end(), pdg);
  }
  case kMatterAntimatter: {
    return std::count(MatterList.begin(), MatterList.end(), pdg) ||
           std::count(AntiMatterList.begin(), AntiMatterList.end(), pdg);
  }
  case kAntimatter: {
    return std::count(AntiMatterList.begin(), AntiMatterList.end(), pdg);
  }
  default: { throw invalid_MatterType(); }
  }
}
bool IsNeutralLepton(PDG_t pdg, MatterType type) {
  return IsInPDGList(pdg, NeutralLeptons_matter, NeutralLeptons_antimatter,
                     type);
}
bool IsChargedLepton(PDG_t pdg, MatterType type) {
  return IsInPDGList(pdg, ChargedLeptons_matter, ChargedLeptons_antimatter,
                     type);
}
bool IsProton(PDG_t pdg, MatterType type) {
  return IsInPDGList(pdg, Proton_matter, Proton_antimatter, type);
}
bool IsNeutron(PDG_t pdg) { return pdg == Neutron[0]; }
bool IsChargedPion(PDG_t pdg) {
  return std::count(ChargedPions.begin(), ChargedPions.end(), pdg);
}
bool IsNeutralPion(PDG_t pdg) {
  return std::count(NeutralPions.begin(), NeutralPions.end(), pdg);
}
bool IsPion(PDG_t pdg) { return std::count(Pions.begin(), Pions.end(), pdg); }
bool IsOther(PDG_t pdg) {
  return !std::count(CommonParticles.begin(), CommonParticles.end(), pdg);
}

bool IsMatter(PDG_t pdg) {
  // Special cases
  switch (pdg) {
  case kPiPlus: {
    return true;
  }
  case kPiMinus: {
    return true;
  }
  case kPi0: {
    return true;
  }
  case kNeutron: {
    return true;
  }
  }
  return (pdg > 0);
}
bool IsAntiMatter(PDG_t pdg) {
  // Special cases
  switch (pdg) {
  case kPiPlus: {
    return true;
  }
  case kPiMinus: {
    return true;
  }
  case kPi0: {
    return true;
  }
  case kNeutron: {
    return true;
  }
  }
  return (pdg < 0);
}

bool IsNuclearPDG(event::PDG_t pdg) { return (pdg > 1000000000); }

PDG_t MakeNuclearPDG(size_t A, size_t Z) {
  return 1000 * Z + 10 * A + 1000000000;
}

size_t GetA(PDG_t pdg) { return ((pdg / 10) % 1000); }
size_t GetZ(PDG_t pdg) { return ((pdg / 1000) % 1000); }

} // namespace utility
} // namespace nuis
