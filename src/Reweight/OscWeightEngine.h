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

#include "FitLogger.h"

#include "FitEvent.h"

#include "PhysConst.h"

#include "BargerPropagator.h"

#include <cmath>

class OscWeightEngine {
  BargerPropagator bp;

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
      default: { THROW("Attempting to convert \"neutrino pdg\": " << pdg); }
    }
  }

  bool DoOsc;

  // Osc params.
  double dm23;
  double theta23;
  double theta13;
  double dm12;
  double theta12;
  double dcp;
  double constant_density;

  /// Whether LengthParam corresponds to a Zenith or a baseline.
  ///
  /// If we just want to calculate the osc. prob. with a constant matter density
  /// then this should be false and constant_density should be set
  /// (or 0 for vacuum prob).
  bool LengthParamIsZenith;

  /// Either a path length or a post oscillation zenith angle
  ///
  /// N.B. For a beamline that has a dip angle of X degrees, the post
  /// oscillation zenith angle will be 90+X degrees.
  double LengthParam;

 public:
  OscWeightEngine()
      : bp(),
        DoOsc(false),
        dm23(2.5e-3),
        theta23(1.0),
        theta13(0.10),
        dm12(7.9e-5),
        theta12(0.825),
        dcp(0.0) {}

  /// Configures oscillation parameters from input xml file.
  ///
  /// Osc parameters configured from OscParam XML element as:
  /// <nuisance>
  /// <OscParam dm23="XX" dm12="XX" theta23="XX" theta12="XX" theta13="XX"
  /// dcp="XX" matter_density="XX" baseline="XX" detection_zenith_deg="XX" />
  /// </nuisance>
  /// If matter_density and baseline are present, then oscillation probability
  /// is calculated for a constant matter density.
  /// If detection_zenith_deg is present, then the baseline and density are
  /// calculated from the density profile and radius of the earth.
  /// If none are present, a vacuum oscillation is calculated.
  void Config() {
    std::vector<nuiskey> OscParam = Config::QueryKeys("OscParam");

    if (OscParam.size() < 1) {
      return;
    }

    if (OscParam[0].Has("baseline") && OscParam[0].Has("matter_density")) {
      LengthParamIsZenith = false;
      LengthParam = OscParam[0].GetD("baseline");
      constant_density = OscParam[0].GetD("matter_density");
    } else if (OscParam[0].Has("detection_zenith_deg")) {
      LengthParamIsZenith = true;
      static const double deg2rad = asin(1) / 90.0;
      LengthParam = cos(OscParam[0].GetD("detection_zenith_deg") * deg2rad);
    } else {
      constant_density = 0xdeadbeef;
    }
    DoOsc = true;

    dm23 = OscParam[0].Has("dm23") ? OscParam[0].GetD("dm23") : dm23;
    theta23 =
        OscParam[0].Has("theta23") ? OscParam[0].GetD("theta23") : theta23;
    theta13 =
        OscParam[0].Has("theta13") ? OscParam[0].GetD("theta13") : theta13;
    dm12 = OscParam[0].Has("dm12") ? OscParam[0].GetD("dm12") : dm12;
    theta12 =
        OscParam[0].Has("theta12") ? OscParam[0].GetD("theta12") : theta12;
    dcp = OscParam[0].Has("dcp") ? OscParam[0].GetD("dcp") : dcp;

    QLOG(FIT, "Configured oscillation weighter:");

    QLOG(FIT, "\tdm23   : " << dm23);
    QLOG(FIT, "\ttheta23: " << theta23);
    QLOG(FIT, "\ttheta13: " << theta13);
    QLOG(FIT, "\tdm12   : " << dm12);
    QLOG(FIT, "\ttheta12: " << theta12);
    QLOG(FIT, "\tdcp   : " << dcp);

    if (LengthParamIsZenith) {
      QLOG(FIT, "Earth density profile with detection cos(zenith) = "
                    << LengthParam);
    } else {
      if (constant_density != 0xdeadbeef) {
        QLOG(FIT,
             "Constant density with experimental baseline = " << LengthParam);
      } else {
        QLOG(FIT, "Vacuum oscillations with experimental baseline = "
                      << LengthParam);
      }
    }
  }

  double CalcWeight(FitEvent* evt) {
    if (!DoOsc) {
      return 1;
    }

    FitParticle* ISNeutralLepton =
        evt->GetHMISParticle(PhysConst::pdg_neutrinos);
    if (!ISNeutralLepton) {
      return 1;
    }

    int NuType = GetNuType(ISNeutralLepton->PDG());
    bp.SetMNS(theta12, theta13, theta23, dm12, dm23, dcp,
              ISNeutralLepton->E() * 1E-3, false, NuType);

    if (LengthParamIsZenith) {  // Use earth density
      bp.DefinePath(LengthParam, 0);
      bp.propagate(NuType);
      return bp.GetProb(NuType, NuType);
    } else {
      if (constant_density != 0xdeadbeef) {
        bp.propagateLinear(NuType, LengthParam, constant_density);
        return bp.GetProb(NuType, NuType);
      } else {
        return bp.GetVacuumProb(NuType, NuType, ISNeutralLepton->E() * 1E-3,
                                LengthParam);
      }
    }
  };
};
