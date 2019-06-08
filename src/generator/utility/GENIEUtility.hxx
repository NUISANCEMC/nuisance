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

#pragma once

#include "event/Particle.hxx"

#include "event/types.hxx"

#include <set>

class TGraph;

namespace genie {
class GHepRecord;
class GHepParticle;
} // namespace genie

namespace fhicl {
  class ParameterSet;
}

namespace nuis {
namespace genietools {

NEW_NUIS_EXCEPT(invalid_GENIE_event);

event::Channel_t GetEventChannel(genie::GHepRecord const &);

event::Particle::Status_t GetParticleStatus(genie::GHepParticle const &p,
                                            event::Channel_t chan);

double GetFileWeight(fhicl::ParameterSet const &xsecinfo,
                     std::set<std::string> const &spline_list = {});

} // namespace genietools
} // namespace nuis
