// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret
/*******************************************************************************
 *    This file is part of NUISANCE.
 *
 *    NUISANCE is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    NUISANCE is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with NUISANCE.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/

#include "nusystematicsWeightEngine.h"

#include <limits>
#include <string>

nusystematicsWeightEngine::nusystematicsWeightEngine() {
  fUseCV = false;

  // This is a bit hacky...
  if (Config::HasPar("GENIETune")) {
    setenv("GENIE_XSEC_TUNE", Config::GetParS("GENIETune").c_str(), true);
    NUIS_LOG(DEB, "Set GENIE_XSEC_TUNE=" << Config::GetParS("GENIETune"));
  }

  Config();
}

void nusystematicsWeightEngine::Config() {
  std::vector<nuiskey> DuneRwtParam = Config::QueryKeys("DUNERwt");

  if (DuneRwtParam.size() < 1) {
    NUIS_ABORT(
        "Instantiaged nusystematicsWeightEngine but without specifying a "
        "DUNERwt element that leads the way to the configuration.");
  }

  std::string fhicl_name = DuneRwtParam.front().GetS("ConfigFHiCL");

  DUNErwt.LoadConfiguration(fhicl_name);
}

systtools::paramId_t const kNuSystCVResponse = 999;

int nusystematicsWeightEngine::ConvDial(std::string name) {

  std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine::ConvDial(std::string name)] called, name = " << name << std::endl;

  if (name == "NuSystCVResponse") {
    return kNuSystCVResponse;
  }
  if (!DUNErwt.HaveHeader(name)) {
    NUIS_ABORT("nusystematicsWeightEngine passed dial: "
               << name << " that it does not understand.");
  }
  NUIS_LOG(FIT, "Added NuSyst param, " << name << " with ID: " << DUNErwt.GetHeaderId(name));
  return DUNErwt.GetHeaderId(name);
}

void nusystematicsWeightEngine::IncludeDial(std::string name, double startval) {

  std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine::IncludeDial(std::string name, double startval)] called, name = " << name << ", startval = " << startval << std::endl;

  systtools::paramId_t DuneRwtEnum(ConvDial(name));

  if (DuneRwtEnum == kNuSystCVResponse) {
    fUseCV = true;
  }

  EnabledParams.push_back({DuneRwtEnum, startval});
}

void nusystematicsWeightEngine::SetDialValue(int nuisenum, double val) {

  std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine::SetDialValue(int nuisenum, double val)] called, nuisenum = " << nuisenum << ", val = " << val << std::endl;

  systtools::paramId_t DuneRwtEnum = (nuisenum % NUIS_DIAL_OFFSET);

  if (DuneRwtEnum == kNuSystCVResponse) {
    return;
  }

  systtools::ParamValue &pval =
      GetParamElementFromContainer(EnabledParams, DuneRwtEnum);
  fHasChanged = (pval.val - val) > std::numeric_limits<double>::epsilon();
  pval.val = val;
}
void nusystematicsWeightEngine::SetDialValue(std::string name, double val) {

  std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine::SetDialValue(std::string name, double val)] called, name = " << name << ", val = " << val << std::endl;

  if (!IsDialIncluded(name)) {
    NUIS_ABORT("nusystematicsWeightEngine passed dial: "
               << name << " that is not enabled.");
  }

  systtools::paramId_t DuneRwtEnum(ConvDial(name));

  if (DuneRwtEnum == kNuSystCVResponse) {
    return;
  }

  systtools::ParamValue &pval =
      GetParamElementFromContainer(EnabledParams, DuneRwtEnum);
  fHasChanged = (pval.val - val) > std::numeric_limits<double>::epsilon();
  pval.val = val;
}

bool nusystematicsWeightEngine::IsDialIncluded(std::string name) {

  std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine::IsDialIncluded(std::string name)] called, name = " << name << std::endl;

  return IsDialIncluded(ConvDial(name));
}
bool nusystematicsWeightEngine::IsDialIncluded(int nuisenum) {
  systtools::paramId_t DuneRwtEnum = (nuisenum % NUIS_DIAL_OFFSET);
  if (DuneRwtEnum == kNuSystCVResponse) {
    return fUseCV;
  }
  return systtools::ContainterHasParam(EnabledParams, DuneRwtEnum);
}

double nusystematicsWeightEngine::GetDialValue(std::string name) {
  if (!IsDialIncluded(name)) {
    NUIS_ABORT("nusystematicsWeightEngine passed dial: "
               << name << " that is not enabled.");
  }
  systtools::ParamValue &pval =
      GetParamElementFromContainer(EnabledParams, ConvDial(name));
  return pval.val;
}
double nusystematicsWeightEngine::GetDialValue(int nuisenum) {
  if (!IsDialIncluded(nuisenum)) {
    NUIS_ABORT("nusystematicsWeightEngine passed dial: "
               << nuisenum << " that is not enabled.");
  }

  systtools::paramId_t DuneRwtEnum = (nuisenum % NUIS_DIAL_OFFSET);

  if (DuneRwtEnum == kNuSystCVResponse) {
    return 1;
  }

  systtools::ParamValue &pval =
      GetParamElementFromContainer(EnabledParams, DuneRwtEnum);
  return pval.val;
}

void nusystematicsWeightEngine::Reconfigure(bool silent) {
  fHasChanged = false;
};

bool nusystematicsWeightEngine::NeedsEventReWeight() {
  if (fHasChanged) {
    return true;
  }
  return false;
}

double nusystematicsWeightEngine::CalcWeight(BaseFitEvt *evt) {

  std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine.cxx][CalcWeight] Called" << std::endl;

  // note 1) typedef std::vector<VarAndCVResponse> event_unit_response_w_cv_t
  // note 2) struct VarAndCVResponse {
  // systtools::paramId_t pid;
  // double CV_response;
  // std::vector<double> responses;
  // };
  
  systtools::event_unit_response_w_cv_t responses =
      DUNErwt.GetEventVariationAndCVResponse(*evt->genie_event->event);

  std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine.cxx][CalcWeight] GetEventVariationAndCVResponse finished" << std::endl;

  double weight = 1;
  std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine.cxx][CalcWeight] responses.size() = " << responses.size() << std::endl;
  for (auto const &resp : responses) {

    std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine.cxx][CalcWeight] - reps looped" << std::endl;
    std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine.cxx][CalcWeight]   - pid = " << resp.pid << std::endl;

    auto const& sph = DUNErwt.GetHeader(resp.pid);
    std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine.cxx][CalcWeight]   - prettyName = " << sph.prettyName << std::endl;
    std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine.cxx][CalcWeight]   - isSplineable = " << sph.isSplineable << std::endl;
    std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine.cxx][CalcWeight]   - paramVariations.size() = " << sph.paramVariations.size() << ":" << std::endl;
    for(auto const &v: sph.paramVariations) std::cout << v << ", ";
    std::cout << std::endl;

    std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine.cxx][CalcWeight]   - CV_response = " << resp.CV_response << std::endl;
    std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine.cxx][CalcWeight]   - responses.size() = " << resp.responses.size() << ":" << std::endl;
    for(auto const &v: resp.responses) std::cout << v << ", ";
    std::cout << std::endl;


    if (!DUNErwt.IsWeightResponse(resp.pid)) {
      std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine.cxx][CalcWeight]   -> NOT IsWeightResponse, continue" << std::endl;
      continue;
    }
    if (fUseCV) {
      std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine.cxx][CalcWeight]   - UseCV: true" << std::endl;
      weight *= resp.CV_response;
    } else { // This is very inefficient for fitting, as it recalculates the
             // spline every time.

      std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine.cxx][CalcWeight]   - UseCV: false" << std::endl;
      //this is a completely backwards way of doing this loop, but the whole thing is broken anyway.
      auto pid = resp.pid;
      size_t index = GetParamContainerIndex(EnabledParams, pid);
      if(index == systtools::kParamUnhandled<size_t>){
        std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine.cxx][CalcWeight]   -> Not in EnabledParams, skipping.." << std::endl;
      }
      else{
        std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine.cxx][CalcWeight]   - index = " << index << std::endl;
        std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine.cxx][CalcWeight]   - EnabledParams[index].val = " << EnabledParams[index].val << std::endl;
        std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine.cxx][CalcWeight]   - Running GetParameterResponse" << std::endl;

        bool DialAlreadyFound = false;
        unsigned int counter = -1;
        for(auto const &v: sph.paramVariations){
          counter++;
          if( std::abs( v - EnabledParams[index].val ) < std::numeric_limits<double>::epsilon() ){
            DialAlreadyFound = true;
            break;
          }
        }

        if(DialAlreadyFound){
          std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine.cxx][CalcWeight]   - Dial found from paramVariations" << std::endl;
          std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine.cxx][CalcWeight]   - sph.paramVariations[counter] = " << sph.paramVariations[counter] << std::endl;
          const double this_rw = (resp.CV_response * resp.responses[counter]);
          std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine.cxx][CalcWeight]   -> Found weight = " << this_rw << std::endl;
          weight *= this_rw;
        }
        else{

          std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine.cxx][CalcWeight]   - Dial NOT found from paramVariations, constructing a spline" << std::endl;

          const double this_rw = (resp.CV_response *
                     DUNErwt.GetParameterResponse(resp.pid, EnabledParams[index].val,
                                                  systtools::event_unit_response_t{
                                                      {resp.pid, resp.responses}}));
          std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine.cxx][CalcWeight]   -> this_rw = " << this_rw << std::endl;
/*
          weight *= (resp.CV_response *
                     DUNErwt.GetParameterResponse(resp.pid, EnabledParams[index].val,
                                                  systtools::event_unit_response_t{
                                                      {resp.pid, resp.responses}}));
*/
          weight *= this_rw;
        }
      }
    }

    std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine.cxx][CalcWeight]   -> Going to next resp" << std::endl;

  }

  std::cout << "[JSKIMDEBUG][nusystematicsWeightEngine.cxx][CalcWeight] ---> Final weight = " << weight << std::endl;

  return weight;
}

void nusystematicsWeightEngine::Print() {
  std::cout << "nusystematicsWeightEngine: " << std::endl;
}
