// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret
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
  if (name == "NuSystCVResponse") {
    return kNuSystCVResponse;
  }
  if (!DUNErwt.HaveHeader(name)) {
    NUIS_ABORT("nusystematicsWeightEngine passed dial: "
               << name << " that it does not understand.");
  }
  return DUNErwt.GetHeaderId(name);
}

void nusystematicsWeightEngine::IncludeDial(std::string name, double startval) {

  systtools::paramId_t DuneRwtEnum(ConvDial(name));

  if (DuneRwtEnum == kNuSystCVResponse) {
    fUseCV = true;
  }

  EnabledParams.push_back({DuneRwtEnum, startval});
}

void nusystematicsWeightEngine::SetDialValue(int nuisenum, double val) {

  systtools::paramId_t DuneRwtEnum = (nuisenum % 1000);

  if (DuneRwtEnum == kNuSystCVResponse) {
    return;
  }

  systtools::ParamValue &pval =
      GetParamElementFromContainer(EnabledParams, DuneRwtEnum);
  fHasChanged = (pval.val - val) > std::numeric_limits<double>::epsilon();
  pval.val = val;
}
void nusystematicsWeightEngine::SetDialValue(std::string name, double val) {
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
  return IsDialIncluded(ConvDial(name));
}
bool nusystematicsWeightEngine::IsDialIncluded(int nuisenum) {
  systtools::paramId_t DuneRwtEnum = (nuisenum % 1000);
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

  systtools::paramId_t DuneRwtEnum = (nuisenum % 1000);

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
  systtools::event_unit_response_w_cv_t responses =
      DUNErwt.GetEventVariationAndCVResponse(*evt->genie_event->event);

  double weight = 1;
  for (auto const &resp : responses) {
    if (!DUNErwt.IsWeightResponse(resp.pid)) {
      continue;
    }
    if (fUseCV) {
      weight *= resp.CV_response;
    } else { // This is very inefficient for fitting, as it recalculates the
             // spline every time.
      systtools::ParamValue const &pval =
          GetParamElementFromContainer(EnabledParams, resp.pid);
      weight *= (resp.CV_response *
                 DUNErwt.GetParameterResponse(resp.pid, pval.val,
                                              systtools::event_unit_response_t{
                                                  {resp.pid, resp.responses}}));
    }
  }

  return weight;
}

void nusystematicsWeightEngine::Print() {
  std::cout << "nusystematicsWeightEngine: " << std::endl;
}
