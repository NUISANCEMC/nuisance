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

#include "WeightEngineBase.h"

#ifdef __PROB3PP_ENABLED__
#include "BargerPropagator.h"
#endif

#include <cmath>

class OscWeightEngine : public WeightEngineBase {
  enum params {

    dm23_idx = 0,
    theta23_idx,
    theta13_idx,
    dm12_idx,
    theta12_idx,
    dcp_idx,

  };

#ifdef __PROB3PP_ENABLED__
  BargerPropagator bp;
#endif

  //******************************* Osc params ******************************
  double theta12;
  double theta13;
  double theta23;
  /// The 1-2 mass squared splitting (small) [eV]
  double dm12;
  /// The 2-3 mass squared splitting (large) [eV]
  double dm23;
  /// The PMNS CP-violating phase
  double dcp;

  ///\brief The constant matter density used for simple given baseline
  /// oscillation [g/cm^3]
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

  /// Holds current value of oscillation parameters.
  double params[6];

  /// The oscillation target type
  ///
  /// If unspecified in the <OscParam /> element, it will default to
  /// disappearance probability.
  int TargetNuType;

 public:
  OscWeightEngine();

  /// Configures oscillation parameters from input xml file.
  ///
  /// Osc parameters configured from OscParam XML element as:
  /// <nuisance>
  /// <OscParam dm23="XX" dm12="XX" theta23="XX" theta12="XX" theta13="XX"
  /// dcp="XX" matter_density="XX" baseline="XX" detection_zenith_deg="XX"
  /// TargetNuPDG="[12,14,16,-12,-14,-16]"/>
  /// </nuisance>
  /// If matter_density and baseline are present, then oscillation probability
  /// is calculated for a constant matter density.
  /// If detection_zenith_deg is present, then the baseline and density are
  /// calculated from the density profile and radius of the earth.
  /// If none are present, a vacuum oscillation is calculated.
  /// If TargetNuPDG is unspecified, oscillation will default to
  /// disappearance probability.
  void Config();

  // Functions requiring Override
  void IncludeDial(std::string name, double startval);

  void SetDialValue(int nuisenum, double val);
  void SetDialValue(std::string name, double val);

  bool IsDialIncluded(std::string name);
  bool IsDialIncluded(int nuisenum);

  double GetDialValue(std::string name);
  double GetDialValue(int nuisenum);

  void Reconfigure(bool silent);

  bool NeedsEventReWeight();

  double CalcWeight(BaseFitEvt* evt);

  static int SystEnumFromString(std::string const& name);
};
