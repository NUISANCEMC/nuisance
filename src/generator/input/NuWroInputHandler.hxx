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

#ifndef GENERATOR_INPUT_NUWROINPUTHANDLER_HXX_SEEN
#define GENERATOR_INPUT_NUWROINPUTHANDLER_HXX_SEEN

#include "event/FullEvent.hxx"

#include "input/IInputHandler.hxx"

#include "exception/exception.hxx"

#include <memory>

namespace fhicl {
class ParameterSet;
}

namespace nuis {
namespace utility {
class TreeFile;
}
} // namespace nuis

class NuWroInputHandler : public IInputHandler {
  mutable std::unique_ptr<nuis::utility::TreeFile> fInputTree;
  mutable nuis::event::FullEvent fReaderEvent;
  mutable std::vector<double> fWeightCache;

  bool fKeepIntermediates;

public:

  NEW_NUIS_EXCEPT(weight_cache_miss);

  NuWroInputHandler();
  NuWroInputHandler(NuWroInputHandler const &) = delete;
  NuWroInputHandler(NuWroInputHandler &&);

  void Initialize(fhicl::ParameterSet const &);
  nuis::event::MinimalEvent const &GetMinimalEvent(ev_index_t idx) const;
  nuis::event::FullEvent const &GetFullEvent(ev_index_t idx) const;
  double GetEventWeight(ev_index_t idx) const;
  size_t GetNEvents() const;
};

#endif
