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

//#define DEBUG_OSC_WE

#include "OscWeightEngine.h"

#include <limits>

enum nuTypes {
  kNuebarType = -1,
  kNumubarType = -2,
  kNutaubarType = -3,
  kNueType = 1,
  kNumuType = 2,
  kNutauType = 3,
};

nuTypes GetNuType(int pdg) {
  switch (pdg) {
    case 16:
      return kNutauType;
    case 14:
      return kNumuType;
    case 12:
      return kNueType;
    case -16:
      return kNutaubarType;
    case -14:
      return kNumubarType;
    case -12:
      return kNuebarType;
    default: {
      NUIS_ABORT("Attempting to convert \"neutrino pdg\": " << pdg);
    }
  }
}

OscWeightEngine::OscWeightEngine()
    : bp(),
      theta12(0.825),
      theta13(0.10),
      theta23(1.0),
      dm12(7.9e-5),
      dm23(2.5e-3),
      dcp(0.0),
      LengthParam(0xdeadbeef),
      TargetNuType(0),
      ForceFromNuPDG(0) {
  Config();
}

void OscWeightEngine::Config() {
  std::vector<nuiskey> OscParam = Config::QueryKeys("OscParam");

  if (OscParam.size() < 1) {
    NUIS_ERR(WRN,
             "Oscillation parameters specified but no OscParam element "
             "configuring the experimental characteristics found.\nExpect at "
             "least <OscParam baseline_km=\"XXX\" />. Pausing for "
             "10...");
    sleep(10);
    return;
  }

  if (OscParam[0].Has("baseline_km")) {
    LengthParamIsZenith = false;
    LengthParam = OscParam[0].GetD("baseline_km");
    constant_density = OscParam[0].Has("matter_density")
                           ? OscParam[0].GetD("matter_density")
                           : 0xdeadbeef;

  } else if (OscParam[0].Has("detection_zenith_deg")) {
    LengthParamIsZenith = true;
    static const double deg2rad = asin(1) / 90.0;
    LengthParam = cos(OscParam[0].GetD("detection_zenith_deg") * deg2rad);
  } else {
    NUIS_ERR(WRN,
             "It appeared that you wanted to set up an oscillation weight "
             "branch, but it was not correctly configured. You need to specify "
             "either: detection_zenith_deg or baseline_km attributes on the "
             "OscParam element, and if baseline_km is specified, you can "
             "optionally also set matter_density for oscillations through a "
             "constant matter density. Pausing for 10...");
    sleep(10);
    return;
  }

  dm23 = OscParam[0].Has("dm23") ? OscParam[0].GetD("dm23") : dm23;
  theta23 = OscParam[0].Has("sinsq_theta23") ? OscParam[0].GetD("sinsq_theta23")
                                             : theta23;
  theta13 = OscParam[0].Has("sinsq_theta13") ? OscParam[0].GetD("sinsq_theta13")
                                             : theta13;
  dm12 = OscParam[0].Has("dm12") ? OscParam[0].GetD("dm12") : dm12;
  theta12 = OscParam[0].Has("sinsq_theta12") ? OscParam[0].GetD("sinsq_theta12")
                                             : theta12;
  dcp = OscParam[0].Has("dcp") ? OscParam[0].GetD("dcp") : dcp;
  TargetNuType = OscParam[0].Has("TargetNuPDG")
                     ? GetNuType(OscParam[0].GetI("TargetNuPDG"))
                     : 0;
  ForceFromNuPDG = OscParam[0].Has("ForceFromNuPDG")
                       ? GetNuType(OscParam[0].GetI("ForceFromNuPDG"))
                       : 0;

  NUIS_LOG(FIT, "Configured oscillation weighter:");

  if (LengthParamIsZenith) {
    NUIS_LOG(FIT, "Earth density profile with detection cos(zenith) = "
                      << LengthParam);
  } else {
    if (constant_density != 0xdeadbeef) {
      NUIS_LOG(FIT,
               "Constant density with experimental baseline = " << LengthParam);
    } else {
      NUIS_LOG(FIT, "Vacuum oscillations with experimental baseline = "
                        << LengthParam);
    }
  }

  params[0] = dm23;
  params[1] = theta23;
  params[2] = theta13;
  params[3] = dm12;
  params[4] = theta12;
  params[5] = dcp;

  NUIS_LOG(FIT, "\tdm23   : " << params[0]);
  NUIS_LOG(FIT, "\tsinsq_theta23: " << params[1]);
  NUIS_LOG(FIT, "\tsinsq_theta13: " << params[2]);
  NUIS_LOG(FIT, "\tdm12   : " << params[3]);
  NUIS_LOG(FIT, "\tsinsq_theta12: " << params[4]);
  NUIS_LOG(FIT, "\tdcp   : " << params[5]);
  if (TargetNuType) {
    NUIS_LOG(FIT, "\tTargetNuType: " << TargetNuType);
  }
  if (ForceFromNuPDG) {
    NUIS_LOG(FIT, "\tForceFromNuPDG: " << ForceFromNuPDG);
  }

  bp.SetMNS(params[theta12_idx], params[theta13_idx], params[theta23_idx],
            params[dm12_idx], params[dm23_idx], params[dcp_idx], 1, true, 2);
  bp.DefinePath(LengthParam, 0);

  if (LengthParamIsZenith) {
    NUIS_LOG(FIT, "\tBaseline   : " << (bp.GetBaseline() / 100.0) << " km.");
  }
}

void OscWeightEngine::IncludeDial(std::string name, double startval) {
#ifdef DEBUG_OSC_WE
  std::cout << "IncludeDial: " << name << " at " << startval << std::endl;
#endif
  int dial = SystEnumFromString(name);
  if (!dial) {
    NUIS_ABORT("OscWeightEngine passed dial: "
               << name << " that it does not understand.");
  }
  params[dial - 1] = startval;
}

void OscWeightEngine::SetDialValue(int nuisenum, double val) {
#ifdef DEBUG_OSC_WE
  std::cout << "SetDial: " << (nuisenum % NUIS_DIAL_OFFSET) << " at " << val
            << std::endl;
#endif
  fHasChanged = (params[(nuisenum % NUIS_DIAL_OFFSET) - 1] - val) >
                std::numeric_limits<double>::epsilon();
  params[(nuisenum % NUIS_DIAL_OFFSET) - 1] = val;
}
void OscWeightEngine::SetDialValue(std::string name, double val) {
#ifdef DEBUG_OSC_WE
  std::cout << "SetDial: " << name << " at " << val << std::endl;
#endif
  int dial = SystEnumFromString(name);
  if (!dial) {
    NUIS_ABORT("OscWeightEngine passed dial: "
               << name << " that it does not understand.");
  }

  fHasChanged =
      (params[dial - 1] - val) > std::numeric_limits<double>::epsilon();
  params[dial - 1] = val;
}

bool OscWeightEngine::IsDialIncluded(std::string name) {
  return SystEnumFromString(name);
}
bool OscWeightEngine::IsDialIncluded(int nuisenum) {
  return ((nuisenum % NUIS_DIAL_OFFSET) > 0) &&
         ((nuisenum % NUIS_DIAL_OFFSET) < 6);
}

double OscWeightEngine::GetDialValue(std::string name) {
  int dial = SystEnumFromString(name);
  if (!dial) {
    NUIS_ABORT("OscWeightEngine passed dial: "
               << name << " that it does not understand.");
  }
  return params[dial - 1];
}
double OscWeightEngine::GetDialValue(int nuisenum) {
  if (!(nuisenum % NUIS_DIAL_OFFSET) || (nuisenum % NUIS_DIAL_OFFSET) > 6) {
    NUIS_ABORT("OscWeightEngine passed dial enum: "
               << (nuisenum % NUIS_DIAL_OFFSET)
               << " that it does not understand, expected [1,6].");
  }
  return params[(nuisenum % NUIS_DIAL_OFFSET) - 1];
}

void OscWeightEngine::Reconfigure(bool silent) { fHasChanged = false; };

bool OscWeightEngine::NeedsEventReWeight() {
  if (fHasChanged) {
    return true;
  }
  return false;
}

double OscWeightEngine::CalcWeight(BaseFitEvt* evt) {
  static bool Warned = false;
  if (evt->probe_E == 0xdeadbeef) {
    if (!Warned) {
      NUIS_ERR(WRN,
               "Oscillation weights asked for but using 'litemode' or "
               "unsupported generator input. Pasuing for 10...");
      sleep(10);
      Warned = true;
    }
    return 1;
  }

  return CalcWeight(evt->probe_E * 1E-3, evt->probe_pdg);
}

double OscWeightEngine::CalcWeight(double ENu, int PDGNu, int TargetPDGNu) {
  if (LengthParam == 0xdeadbeef) {  // not configured.
    return 1;
  }
  int NuType = (ForceFromNuPDG != 0) ? ForceFromNuPDG : GetNuType(PDGNu);
  bp.SetMNS(params[theta12_idx], params[theta13_idx], params[theta23_idx],
            params[dm12_idx], params[dm23_idx], params[dcp_idx], ENu, true,
            NuType);

  int pmt = 0;
  double prob_weight = 1;
  TargetPDGNu = (TargetPDGNu == -1) ? (TargetNuType ? TargetNuType : NuType)
                                    : GetNuType(TargetPDGNu);

  if (LengthParamIsZenith) {  // Use earth density
    bp.DefinePath(LengthParam, 0);
    bp.propagate(NuType);
    pmt = 0;
    prob_weight = bp.GetProb(NuType, TargetPDGNu);
  } else {
    if (constant_density != 0xdeadbeef) {
      bp.propagateLinear(NuType, LengthParam, constant_density);
      pmt = 1;
      prob_weight = bp.GetProb(NuType, TargetPDGNu);
    } else {
      pmt = 2;
      prob_weight =
          bp.GetVacuumProb(NuType, TargetPDGNu, ENu * 1E-3, LengthParam);
    }
  }
#ifdef DEBUG_OSC_WE
  if (prob_weight != prob_weight) {
    NUIS_ABORT("Calculated bad prob weight: " << prob_weight << "(Osc Type: "
                                              << pmt << " -- " << NuType
                                              << " -> " << TargetPDGNu << ")");
  }
  if (prob_weight > 1) {
    NUIS_ABORT("Calculated bad prob weight: " << prob_weight << "(Osc Type: "
                                              << pmt << " -- " << NuType
                                              << " -> " << TargetPDGNu << ")");
  }

  std::cout << NuType << " -> " << TargetPDGNu << ": " << ENu << " = "
            << prob_weight << "%%." << std::endl;
#endif
  return prob_weight;
}

int OscWeightEngine::SystEnumFromString(std::string const& name) {
  if (name == "dm23") {
    return 1;
  } else if (name == "sinsq_theta23") {
    return 2;
  } else if (name == "sinsq_theta13") {
    return 3;
  } else if (name == "dm12") {
    return 4;
  } else if (name == "sinsq_theta12") {
    return 5;
  } else if (name == "dcp") {
    return 6;
  } else {
    return 0;
  }
}

void OscWeightEngine::Print() {
  std::cout << "OscWeightEngine: " << std::endl;

  std::cout << "\t theta12: " << params[theta12_idx] << std::endl;
  std::cout << "\t theta13: " << params[theta13_idx] << std::endl;
  std::cout << "\t theta23: " << params[theta23_idx] << std::endl;
  std::cout << "\t dm12: " << params[dm12_idx] << std::endl;
  std::cout << "\t dm23: " << params[dm23_idx] << std::endl;
  std::cout << "\t dcp: " << params[dcp_idx] << std::endl;
}
