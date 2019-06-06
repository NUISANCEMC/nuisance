// Copyright 2018 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

#include "utility/KinematicUtility.hxx"

#include <limits>
#include <utility>

#include <cmath>

#pragma once
namespace nuis {
namespace event {
class Particle;
} // namespace event
} // namespace nuis

namespace nuis {
namespace utility {

class IParticlePhaseSpaceRestriction {
public:
  virtual bool Inside(nuis::event::Particle const &) const = 0;
};

class FullPhaseSpace : public IParticlePhaseSpaceRestriction {
public:
  bool Inside(nuis::event::Particle const &) const { return true; }
};

class SimpleParticlePhaseSpaceRestriction
    : public IParticlePhaseSpaceRestriction {
  KinematicRange EMomRange;
  KinematicRange CosThetaRange;
  bool IsERange;

public:
  ///\brief Simple E/P and angle phase space restriction constructor
  ///
  ///\detail Best to use one of the named static methods to avoid E/P or
  /// theta/costheta confusion.
  SimpleParticlePhaseSpaceRestriction(
      std::pair<double, double> eprange = {0,
                                           std::numeric_limits<double>::max()},
      bool iserange = true, std::pair<double, double> costhetarange = {0, M_PI})
      : EMomRange(eprange), CosThetaRange(costhetarange), IsERange(iserange) {}

  bool Inside(nuis::event::Particle const &) const;

  static SimpleParticlePhaseSpaceRestriction
  Energy(double min = 0, double max = std::numeric_limits<double>::max()) {
    return SimpleParticlePhaseSpaceRestriction({min, max});
  }

  static SimpleParticlePhaseSpaceRestriction Theta_deg(double min = 0,
                                                       double max = 180) {
    return SimpleParticlePhaseSpaceRestriction(
        {0, std::numeric_limits<double>::max()}, true,
        {cos(min * (M_PI / 180.0)), cos(max * (M_PI / 180.0))});
  }

  static SimpleParticlePhaseSpaceRestriction Theta_rad(double min = 0,
                                                       double max = M_PI) {
    return SimpleParticlePhaseSpaceRestriction(
        {0, std::numeric_limits<double>::max()}, true, {cos(min), cos(max)});
  }

  static SimpleParticlePhaseSpaceRestriction CosTheta(double min = -1,
                                                      double max = 1) {
    return SimpleParticlePhaseSpaceRestriction(
        {0, std::numeric_limits<double>::max()}, true, {min, max});
  }

  static SimpleParticlePhaseSpaceRestriction
  Momentum(double min = 0, double max = std::numeric_limits<double>::max()) {
    return SimpleParticlePhaseSpaceRestriction({min, max}, false);
  }

  static SimpleParticlePhaseSpaceRestriction EnergyTheta_deg(
      std::pair<double, double> erange = {0,
                                          std::numeric_limits<double>::max()},
      std::pair<double, double> thetarange = {0, 180}) {
    return SimpleParticlePhaseSpaceRestriction(
        erange, true,
        {cos(thetarange.first * (M_PI / 180.0)),
         cos(thetarange.second * (M_PI / 180.0))});
  }

  static SimpleParticlePhaseSpaceRestriction EnergyTheta_rad(
      std::pair<double, double> erange = {0,
                                          std::numeric_limits<double>::max()},
      std::pair<double, double> thetarange = {0, M_PI}) {
    return SimpleParticlePhaseSpaceRestriction(
        erange, true, {cos(thetarange.first), cos(thetarange.second)});
  }

  static SimpleParticlePhaseSpaceRestriction EnergyCosTheta(
      std::pair<double, double> erange = {0,
                                          std::numeric_limits<double>::max()},
      std::pair<double, double> costhetarange = {-1, 1}) {
    return SimpleParticlePhaseSpaceRestriction(erange, true, costhetarange);
  }

  static SimpleParticlePhaseSpaceRestriction MomentumTheta_deg(
      std::pair<double, double> prange = {0,
                                          std::numeric_limits<double>::max()},
      std::pair<double, double> thetarange = {0, 180}) {
    return SimpleParticlePhaseSpaceRestriction(
        prange, false,
        {cos(thetarange.first * (M_PI / 180.0)),
         cos(thetarange.second * (M_PI / 180.0))});
  }

  static SimpleParticlePhaseSpaceRestriction MomentumTheta_rad(
      std::pair<double, double> prange = {0,
                                          std::numeric_limits<double>::max()},
      std::pair<double, double> thetarange = {0, M_PI}) {
    return SimpleParticlePhaseSpaceRestriction(
        prange, false, {cos(thetarange.first), cos(thetarange.second)});
  }

  static SimpleParticlePhaseSpaceRestriction MomentumCosTheta(
      std::pair<double, double> prange = {0,
                                          std::numeric_limits<double>::max()},
      std::pair<double, double> costhetarange = {-1, 1}) {
    return SimpleParticlePhaseSpaceRestriction(prange, false, costhetarange);
  }
};

} // namespace utility
} // namespace nuis
