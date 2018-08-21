#include "generator/utility/NuWroUtility.hxx"

#include "exception/exception.hxx"

using namespace nuis::event;

namespace nuis {
namespace nuwrotools {

NEW_NUIS_EXCEPT(invalid_channel_to_NuWro);
NEW_NUIS_EXCEPT(invalid_NuWro_dyn_found);

std::pair<NuWroFlags, int> GetFlagsDynEquivalent(Channel_t chan) {

  std::pair<NuWroFlags, int> NuMode;

  NuMode.first.qel = false;
  NuMode.first.res = false;
  NuMode.first.dis = false;
  NuMode.first.coh = false;
  NuMode.first.mec = false;
  NuMode.first.hip = false;
  NuMode.first.nc = false;
  NuMode.first.cc = false;
  NuMode.first.anty = false;
  NuMode.first.res_delta = false;

  switch (chan) {
  case Channel_t::kCCQE: {
    NuMode.first.qel = true;
    NuMode.first.cc = true;
    break;
  }
  case Channel_t::kCC2p2h: {
    NuMode.first.mec = true;
    NuMode.first.cc = true;
    break;
  }
  case Channel_t::kCCSPP_PPip: {
    NuMode.first.res = true;
    NuMode.first.res_delta = true;
    NuMode.first.cc = true;
    break;
  }
  case Channel_t::kCCSPP_PPi0: {
    NuMode.first.res = true;
    NuMode.first.res_delta = true;
    NuMode.first.cc = true;
    break;
  }
  case Channel_t::kCCSPP_NPip: {
    NuMode.first.res = true;
    NuMode.first.res_delta = true;
    NuMode.first.cc = true;
    break;
  }
  case Channel_t::kCCCohPi: {
    NuMode.first.coh = true;
    NuMode.first.cc = true;
    break;
  }
  case Channel_t::kCCResGamma: {
    NuMode.first.res = true;
    NuMode.first.res_delta = true;
    NuMode.first.cc = true;
    break;
  }
  case Channel_t::kCCTransitionMPi: {
    NuMode.first.res = true;
    NuMode.first.cc = true;
    break;
  }
  case Channel_t::kCCResEta0: {
    NuMode.first.res = true;
    NuMode.first.cc = true;
    break;
  }
  case Channel_t::kCCResK: {
    NuMode.first.res = true;
    NuMode.first.cc = true;
    break;
  }
  case Channel_t::kCCDIS: {
    NuMode.first.dis = true;
    NuMode.first.cc = true;
    break;
  }
  case Channel_t::kNCSPP_NPi0: {
    NuMode.first.res = true;
    NuMode.first.res_delta = true;
    NuMode.first.nc = true;
    break;
  }
  case Channel_t::kNCSPP_PPi0: {
    NuMode.first.res = true;
    NuMode.first.res_delta = true;
    NuMode.first.nc = true;
    break;
  }
  case Channel_t::kNCSPP_PPim: {
    NuMode.first.res = true;
    NuMode.first.res_delta = true;
    NuMode.first.nc = true;
    break;
  }
  case Channel_t::kNCSPP_NPip: {
    NuMode.first.res = true;
    NuMode.first.res_delta = true;
    NuMode.first.nc = true;
    break;
  }
  case Channel_t::kNCCohPi: {
    NuMode.first.coh = true;
    NuMode.first.nc = true;
    break;
  }
  case Channel_t::kNCResNGamma: {
    NuMode.first.res = true;
    NuMode.first.res_delta = true;
    NuMode.first.nc = true;
    break;
  }
  case Channel_t::kNCResPGamma: {
    NuMode.first.res = true;
    NuMode.first.res_delta = true;
    NuMode.first.nc = true;
    break;
  }
  case Channel_t::kNCTransitionMPi: {
    NuMode.first.res = true;
    NuMode.first.nc = true;
    break;
  }
  case Channel_t::kNCResNEta0: {
    NuMode.first.res = true;
    NuMode.first.nc = true;
    break;
  }
  case Channel_t::kNCResPEta0: {
    NuMode.first.res = true;
    NuMode.first.nc = true;
    break;
  }
  case Channel_t::kNCResK0: {
    NuMode.first.res = true;
    NuMode.first.nc = true;
    break;
  }
  case Channel_t::kNCResKp: {
    NuMode.first.res = true;
    NuMode.first.nc = true;
    break;
  }
  case Channel_t::kNCDIS: {
    NuMode.first.dis = true;
    NuMode.first.nc = true;
    break;
  }
  case Channel_t::kNCELP: {
    NuMode.first.qel = true;
    NuMode.first.nc = true;
    break;
  }
  case Channel_t::kNCELN: {
    NuMode.first.qel = true;
    NuMode.first.nc = true;
    break;
  }
  case Channel_t::kCCQE_nub: {
    NuMode.first.qel = true;
    NuMode.first.cc = true;
    NuMode.first.anty = true;
    break;
  }
  case Channel_t::kCC2p2h_nub: {
    NuMode.first.mec = true;
    NuMode.first.cc = true;
    NuMode.first.anty = true;
    break;
  }
  case Channel_t::kCCSPP_NPim_nub: {
    NuMode.first.res = true;
    NuMode.first.res_delta = true;
    NuMode.first.cc = true;
    NuMode.first.anty = true;
    break;
  }
  case Channel_t::kCCSPP_NPi0_nub: {
    NuMode.first.res = true;
    NuMode.first.res_delta = true;
    NuMode.first.cc = true;
    NuMode.first.anty = true;
    break;
  }
  case Channel_t::kCCSPP_PPim_nub: {
    NuMode.first.res = true;
    NuMode.first.res_delta = true;
    NuMode.first.cc = true;
    NuMode.first.anty = true;
    break;
  }
  case Channel_t::kCCCohPi_nub: {
    NuMode.first.coh = true;
    NuMode.first.cc = true;
    NuMode.first.anty = true;
    break;
  }
  case Channel_t::kCCResGamma_nub: {
    NuMode.first.res = true;
    NuMode.first.res_delta = true;
    NuMode.first.cc = true;
    NuMode.first.anty = true;
    break;
  }
  case Channel_t::kCCTransitionMPi_nub: {
    NuMode.first.res = true;
    NuMode.first.cc = true;
    NuMode.first.anty = true;
    break;
  }
  case Channel_t::kCCResEta0_nub: {
    NuMode.first.res = true;
    NuMode.first.res_delta = true;
    NuMode.first.cc = true;
    NuMode.first.anty = true;
    break;
  }
  case Channel_t::kCCResK_nub: {
    NuMode.first.res = true;
    NuMode.first.res_delta = true;
    NuMode.first.cc = true;
    NuMode.first.anty = true;
    break;
  }
  case Channel_t::kCCDIS_nub: {
    NuMode.first.dis = true;
    NuMode.first.anty = true;
    break;
  }
  case Channel_t::kNCSPP_NPi0_nub: {
    NuMode.first.res = true;
    NuMode.first.res_delta = true;
    NuMode.first.nc = true;
    NuMode.first.anty = true;
    break;
  }
  case Channel_t::kNCSPP_PPi0_nub: {
    NuMode.first.res = true;
    NuMode.first.res_delta = true;
    NuMode.first.nc = true;
    NuMode.first.anty = true;
    break;
  }
  case Channel_t::kNCSPP_PPim_nub: {
    NuMode.first.res = true;
    NuMode.first.res_delta = true;
    NuMode.first.nc = true;
    NuMode.first.anty = true;
    break;
  }
  case Channel_t::kNCSPP_NPip_nub: {
    NuMode.first.res = true;
    NuMode.first.res_delta = true;
    NuMode.first.nc = true;
    NuMode.first.anty = true;
    break;
  }
  case Channel_t::kNCCohPi_nub: {
    NuMode.first.coh = true;
    NuMode.first.nc = true;
    NuMode.first.anty = true;
    break;
  }
  case Channel_t::kNCResNGamma_nub: {
    NuMode.first.res = true;
    NuMode.first.res_delta = true;
    NuMode.first.nc = true;
    NuMode.first.anty = true;
    break;
  }
  case Channel_t::kNCResPGamma_nub: {
    NuMode.first.res = true;
    NuMode.first.res_delta = true;
    NuMode.first.nc = true;
    NuMode.first.anty = true;
    break;
  }
  case Channel_t::kNCTransitionMPi_nub: {
    NuMode.first.res = true;
    NuMode.first.nc = true;
    NuMode.first.anty = true;
    break;
  }
  case Channel_t::kNCResNEta0_nub: {
    NuMode.first.res = true;
    NuMode.first.nc = true;
    NuMode.first.anty = true;
    break;
  }
  case Channel_t::kNCResPEta0_nub: {
    NuMode.first.res = true;
    NuMode.first.nc = true;
    NuMode.first.anty = true;
    break;
  }
  case Channel_t::kNCResK0_nub: {
    NuMode.first.res = true;
    NuMode.first.nc = true;
    NuMode.first.anty = true;
    break;
  }
  case Channel_t::kNCResKp_nub: {
    NuMode.first.res = true;
    NuMode.first.nc = true;
    NuMode.first.anty = true;
    break;
  }
  case Channel_t::kNCDIS_nub: {
    NuMode.first.dis = true;
    NuMode.first.nc = true;
    NuMode.first.anty = true;
    break;
  }
  case Channel_t::kNCELP_nub: {
    NuMode.first.qel = true;
    NuMode.first.nc = true;
    NuMode.first.anty = true;
    break;
  }
  case Channel_t::kNCELN_nub: {
    NuMode.first.qel = true;
    NuMode.first.nc = true;
    NuMode.first.anty = true;
    break;
  }

  case Channel_t::kUndefined:
  default: {
    throw invalid_channel_to_NuWro()
        << "[ERROR]: Encountered unhandled Channel_t: " << chan
        << " when build Nuwro-equivalent dyn and flags.";
  }
  }

  NuMode.second = 0;
  if (NuMode.first.qel) {
    NuMode.second = NuMode.first.cc ? 0 : 1;
  }
  if (NuMode.first.res) {
    NuMode.second = NuMode.first.cc ? 2 : 3;
  }
  if (NuMode.first.dis) {
    NuMode.second = NuMode.first.cc ? 4 : 5;
  }
  if (NuMode.first.coh) {
    NuMode.second = NuMode.first.cc ? 6 : 7;
  }
  if (NuMode.first.mec) {
    NuMode.second = NuMode.first.cc ? 8 : 9;
  }

  return NuMode;
}

Channel_t NuWroEventChannel(NuWroEvent const &ev) {
  switch (ev.dyn) {
  case 0: {
    return ev.flag.anty ? Channel_t::kCCQE_nub : Channel_t::kCCQE;
  }
  case 1: {
    return ev.flag.anty ? Channel_t::kNCELN_nub : Channel_t::kNCELN;
  }
  case 2: {
    return ev.flag.anty ? Channel_t::kCCSPP_NPi0_nub : Channel_t::kCCSPP_NPip;
  }
  case 3: {
    return ev.flag.anty ? Channel_t::kNCSPP_NPi0_nub : Channel_t::kNCSPP_NPi0;
  }
  case 4: {
    return ev.flag.anty ? Channel_t::kCCDIS_nub : Channel_t::kCCDIS;
  }
  case 5: {
    return ev.flag.anty ? Channel_t::kNCDIS_nub : Channel_t::kNCDIS;
  }
  case 6: {
    return ev.flag.anty ? Channel_t::kCCCohPi_nub : Channel_t::kCCCohPi;
  }
  case 7: {
    return ev.flag.anty ? Channel_t::kNCCohPi_nub : Channel_t::kNCCohPi;
  }
  case 8: {
    return ev.flag.anty ? Channel_t::kCC2p2h_nub : Channel_t::kCC2p2h;
  }
  default:
    throw invalid_NuWro_dyn_found() << "[ERROR]: Found NuWro dyn: " << ev.dyn;
  }
}

} // namespace nuwrotools
} // namespace nuis
