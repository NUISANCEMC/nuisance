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

#ifndef EVENT_MINIMALEVENT_HXX_SEEN
#define EVENT_MINIMALEVENT_HXX_SEEN

#include "event/types.hxx"

namespace nuis {
namespace event {
///\brief The minimal event information needed to perform reweights.
///
/// Most often, event selections cannot be applied using this reduced format.
class MinimalEvent {
public:
  MinimalEvent();
  MinimalEvent(MinimalEvent const &) = delete;
  MinimalEvent(MinimalEvent &&);
  MinimalEvent &operator=(MinimalEvent &&);

  /// Make a clone of this MinimalEvent
  MinimalEvent Clone() const;

  /// True interaction mode
  Channel_t mode;
  /// True probe energy
  double probe_E;
  /// True probe particle code
  PDG_t probe_pdg;

  PDG_t target_pdg;

  /// Event-weight that can be used to scale to a cross-section prediction
  double XSecWeight;
  /// Event weight incurred from current reweight engine state.
  double RWWeight;
};
} // namespace core
} // namespace nuis
#endif