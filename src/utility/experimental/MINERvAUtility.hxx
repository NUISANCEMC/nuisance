#pragma once

#include "utility/PhaseSpaceRestriction.hxx"

#include <cstdlib> //for size_t
#include <utility>

namespace nuis {
namespace event {
class FullEvent;
} // namespace event
} // namespace nuis

namespace nuis {
namespace utility {
namespace mnv {

std::pair<bool, size_t> IsCC0Pi_NumProtons(event::FullEvent const &);

bool IsCCIncLowRecoil(event::FullEvent const &);
bool IsCC0PiNp(event::FullEvent const &);
bool IsCC0PiNp_STV(event::FullEvent const &);

bool IsCC1Pi0_2016(event::FullEvent const &);
bool IsCC1CPi_2017(event::FullEvent const &);

TVector3 GetDeltaPT_CC0PiN_mnv(event::FullEvent const &fev);
double GetDeltaPhiT_CC0PiN_mnv(event::FullEvent const &fev);
double GetDeltaAlphaT_CC0PiN_mnv(event::FullEvent const &fev);

double GetNeutronMomentumReco_CC0PiN_mnv(event::FullEvent const &fev);

extern SimpleParticlePhaseSpaceRestriction CC0PiNProt_ProtonPS;

} // namespace mnv
} // namespace utility
} // namespace nuis
