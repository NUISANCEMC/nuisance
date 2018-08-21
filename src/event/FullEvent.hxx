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

#ifndef EVENT_FULLEVENT_HXX_SEEN
#define EVENT_FULLEVENT_HXX_SEEN

#include "event/MinimalEvent.hxx"
#include "event/Particle.hxx"

#include "string_parsers/to_string.hxx"

#include <sstream>

namespace nuis {
namespace event {

///\brief The full, internal event format.
class FullEvent : public MinimalEvent {
public:
  struct StatusParticles {
    Particle::Status_t status;
    std::vector<Particle> particles;
  };

  FullEvent();
  FullEvent(FullEvent const &) = delete;
  FullEvent(FullEvent &&);
  std::vector<StatusParticles> ParticleStack;

  void ClearParticleStack();

  std::string to_string() const {
    std::stringstream ss("");
    ss << "Event: Interaction mode = " << mode
       << ", probe: { PDG: " << probe_pdg << ", Energy: " << probe_E
       << " MeV }." << std::endl;
    for (auto &status_stack : ParticleStack) {
      ss << "\t[" << status_stack.status << "]" << std::endl;

      for (Particle const &part : status_stack.particles) {
        ss << "\t\t{ PDG: " << part.pdg << ", P3: [ " << part.P4[0] << ", "
           << part.P4[1] << ", " << part.P4[2] << "], E: " << part.P4[3]
           << ", M: " << part.P4.M() << " }" << std::endl;
      }
    }
    ss << std::endl;
    return ss.str();
  }
};

} // namespace event
} // namespace nuis

#endif
