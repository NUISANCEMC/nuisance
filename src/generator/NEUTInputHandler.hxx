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

#ifndef GENERATOR_NEUTINPUTHANDLER_HXX_SEEN
#define GENERATOR_NEUTINPUTHANDLER_HXX_SEEN

#include "core/IInputHandler.hxx"
#include "core/FullEvent.hxx"

#include <memory>

namespace fhicl {
class ParameterSet;
}

namespace nuis {
namespace core {
class MinimalEvent;
} // namespace core
namespace utility {
class TreeFile;
}
} // namespace nuis

class NEUTInputHandler : public IInputHandler {
  mutable std::unique_ptr<nuis::utility::TreeFile> fInputTree;
  mutable nuis::core::FullEvent fReaderEvent;

public:
  NEUTInputHandler();
  NEUTInputHandler(NEUTInputHandler const &) = delete;
  NEUTInputHandler(NEUTInputHandler &&);

  void Initialize(fhicl::ParameterSet const &);
  nuis::core::MinimalEvent const &GetMinimalEvent(ev_index_t idx) const;
  nuis::core::FullEvent const &GetFullEvent(ev_index_t idx) const;
  size_t GetNEvents() const;
};

#endif
