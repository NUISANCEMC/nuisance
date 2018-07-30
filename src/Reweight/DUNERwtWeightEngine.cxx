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

#include "DUNERwtWeightEngine.h"

#include <limits>
#include <string>

DUNERwtWeightEngine::DUNERwtWeightEngine() { Config(); }

void DUNERwtWeightEngine::Config() {
  std::vector<nuiskey> DuneRwtParam = Config::QueryKeys("DUNERwt");

  if (DuneRwtParam.size() < 1) {
    ERROR(WRN, "Oscillation parameters specified but no OscParam element "
               "configuring the experimental characteristics found.\nExpect at "
               "least <OscParam baseline_km=\"XXX\" />. Pausing for "
               "10...");
    sleep(10);
    return;
  }

  std::string fhicl_name = DuneRwtParam.front().GetS("ConfigFHiCL");

  DUNErwt.LoadConfiguration(fhicl_name);
}

int DUNERwtWeightEngine::ConvDial(std::string name) {
  if (!DUNErwt.HaveHeader(name)) {
    THROW("DUNERwtWeightEngine passed dial: "
          << name << " that it does not understand.");
  }
  return DUNErwt.GetHeaderId(name);
}

void DUNERwtWeightEngine::IncludeDial(std::string name, double startval) {
  EnabledParams.push_back({systtools::paramId_t(ConvDial(name)), startval});
}

void DUNERwtWeightEngine::SetDialValue(int nuisenum, double val) {

  systtools::paramId_t DuneRwtEnum = (nuisenum % 1000);
  systtools::ParamValue &pval =
      GetParamElementFromContainer(EnabledParams, DuneRwtEnum);
  fHasChanged = (pval.val - val) > std::numeric_limits<double>::epsilon();
  pval.val = val;
}
void DUNERwtWeightEngine::SetDialValue(std::string name, double val) {
  if (!IsDialIncluded(name)) {
    THROW("DUNERwtWeightEngine passed dial: " << name
                                              << " that is not enabled.");
  }

  systtools::ParamValue &pval =
      GetParamElementFromContainer(EnabledParams, ConvDial(name));
  fHasChanged = (pval.val - val) > std::numeric_limits<double>::epsilon();
  pval.val = val;
}

bool DUNERwtWeightEngine::IsDialIncluded(std::string name) {
  return IsDialIncluded(ConvDial(name));
}
bool DUNERwtWeightEngine::IsDialIncluded(int nuisenum) {
  systtools::paramId_t DuneRwtEnum = (nuisenum % 1000);
  return systtools::ContainterHasParam(EnabledParams, DuneRwtEnum);
}

double DUNERwtWeightEngine::GetDialValue(std::string name) {
  if (!IsDialIncluded(name)) {
    THROW("DUNERwtWeightEngine passed dial: " << name
                                              << " that is not enabled.");
  }
  systtools::ParamValue &pval =
      GetParamElementFromContainer(EnabledParams, ConvDial(name));
  return pval.val;
}
double DUNERwtWeightEngine::GetDialValue(int nuisenum) {
  if (!IsDialIncluded(nuisenum)) {
    THROW("DUNERwtWeightEngine passed dial: " << nuisenum
                                              << " that is not enabled.");
  }
  systtools::paramId_t DuneRwtEnum = (nuisenum % 1000);
  systtools::ParamValue &pval =
      GetParamElementFromContainer(EnabledParams, DuneRwtEnum);
  return pval.val;
}

void DUNERwtWeightEngine::Reconfigure(bool silent) { fHasChanged = false; };

bool DUNERwtWeightEngine::NeedsEventReWeight() {
  if (fHasChanged) {
    return true;
  }
  return false;
}

double DUNERwtWeightEngine::CalcWeight(BaseFitEvt *evt) {

  double weight = 1;

  if (evt->HasDUNERwtPolyResponses) {
    for (size_t i = 0; i < EnabledParams.size(); ++i) {
      if (DUNErwt.IsSplineParam(EnabledParams[i].pid)) {
        if (!ContainterHasParam(evt->DUNERwtPolyResponses,
                                EnabledParams[i].pid)) {
          continue;
        }
        weight *= GetParamElementFromContainer(evt->DUNERwtPolyResponses,
                                               EnabledParams[i].pid)
                      .resp.eval(EnabledParams[i].val);

      } else {
        if (!evt->HasDUNERwtResponses) {
          evt->DUNERwtResponses =
              DUNErwt.GetEventResponses(*evt->genie_event->event);
          evt->HasDUNERwtResponses = true;
        }
        weight *= DUNErwt.GetDiscreteResponse(EnabledParams[i].pid,
                                              int(EnabledParams[i].val),
                                              evt->DUNERwtResponses);
      }
    }
  } else {
    if (!evt->HasDUNERwtResponses) {
      evt->DUNERwtResponses =
          DUNErwt.GetEventResponses(*evt->genie_event->event);
      evt->HasDUNERwtResponses = true;
    }

    for (size_t i = 0; i < EnabledParams.size(); ++i) {
      if (DUNErwt.IsSplineParam(EnabledParams[i].pid)) {
        weight *= DUNErwt.GetParameterResponse(
            EnabledParams[i].pid, EnabledParams[i].val, evt->DUNERwtResponses);

      } else {
        weight *= DUNErwt.GetDiscreteResponse(EnabledParams[i].pid,
                                              int(EnabledParams[i].val),
                                              evt->DUNERwtResponses);
      }
    }
  }

  return weight;
}

void DUNERwtWeightEngine::Print() {
  std::cout << "DUNERwtWeightEngine: " << std::endl;
}
