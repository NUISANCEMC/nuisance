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

#ifndef CORE_IINPUTHANDLER_HXX_SEEN
#define CORE_IINPUTHANDLER_HXX_SEEN

#include "plugins/traits.hxx"

#include "exception/exception.hxx"

namespace fhicl {
class ParameterSet;
}

namespace nuis {
namespace core {
class MinimalEvent;
class FullEvent;
} // namespace core
} // namespace nuis

class IInputHandler {
public:

  NEW_NUIS_EXCEPT(invalid_input_file);
  NEW_NUIS_EXCEPT(invalid_entry);

  typedef size_t ev_index_t;

  virtual void Initialize(fhicl::ParameterSet const &) = 0;
  virtual nuis::core::MinimalEvent const &GetMinimalEvent(ev_index_t idx) = 0;
  virtual nuis::core::FullEvent const &GetFullEvent(ev_index_t idx) = 0;

  virtual size_t GetNEvents() = 0;

  virtual ~IInputHandler(){}
};

DECLARE_PLUGIN_INTERFACE(IInputHandler);

#endif
