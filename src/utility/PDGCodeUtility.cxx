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
NEW_NUIS_EXCEPT(invalid_neutral_lepton_pdg);
NEW_NUIS_EXCEPT(invalid_charged_lepton_pdg);

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
PDG_t GetChargedLeptonPDG(PDG_t pdg) {
  if (!IsNeutralLepton(pdg)) {
    throw invalid_neutral_lepton_pdg()
        << std::to_string(pdg) << " is not a neutral lepton PDG code.";
  }
  return pdg + (IsMatter(pdg) ? -1 : 1);
}
PDG_t GetNeutralLeptonPDG(PDG_t pdg) {
  if (!IsNeutralLepton(pdg)) {
    throw invalid_charged_lepton_pdg()
        << std::to_string(pdg) << " is not a neutral lepton PDG code.";
  }
  return pdg + (IsMatter(pdg) ? 1 : -1);
}
bool IsProton(PDG_t pdg, MatterType type) {
  return IsInPDGList(pdg, Proton_matter, Proton_antimatter, type);
}
bool IsNeutron(PDG_t pdg) { return pdg == Neutron[0]; }
bool IsChargedPion(PDG_t pdg) {
  return std::count(ChargedPions.begin(), ChargedPions.end(), pdg);
}
bool IsNeutralPion(PDG_t pdg) { return (pdg == kPi0); }
bool IsPositivePion(PDG_t pdg) { return (pdg == kPiPlus); }
bool IsNegativePion(PDG_t pdg) { return (pdg == kPiMinus); }
bool IsPion(PDG_t pdg) { return std::count(Pions.begin(), Pions.end(), pdg); }
bool IsChargedKaon(PDG_t pdg) {
  return std::count(ChargedKaons.begin(), ChargedKaons.end(), pdg);
}
bool IsNeutralKaon(PDG_t pdg) {
  return std::count(NeutralKaons.begin(), NeutralKaons.end(), pdg);
}
bool IsPositiveKaon(PDG_t pdg) { return (pdg == kKPlus); }
bool IsNegativeKaon(PDG_t pdg) { return (pdg == kKMinus); }
bool IsKaon(PDG_t pdg) { return std::count(Kaons.begin(), Kaons.end(), pdg); }
bool IsGamma(PDG_t pdg) { return (pdg == kGamma); }
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

bool IsNuclearPDG(PDG_t pdg) { return (pdg > 1000000000); }

PDG_t MakeNuclearPDG(size_t A, size_t Z) {
  return (10000 * Z) + (10 * A) + 1000000000;
}

size_t GetA(PDG_t pdg) { return ((pdg / 10) % 1000); }
size_t GetZ(PDG_t pdg) { return ((pdg / 10000) % 1000); }

} // namespace utility
} // namespace nuis