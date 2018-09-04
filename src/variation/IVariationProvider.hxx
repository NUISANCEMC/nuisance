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

#ifndef VARIATION_IVARIATIONPROVIDER_HXX_SEEN
#define VARIATION_IVARIATIONPROVIDER_HXX_SEEN

#include "plugins/traits.hxx"

#include "parameters/ParameterManager.hxx"

namespace fhicl {
class ParameterSet;
}

namespace nuis {
namespace event {
class MinimalEvent;
class FullEvent;
} // namespace event
} // namespace nuis

class IVariationProvider {
public:
  virtual void Initialize(fhicl::ParameterSet const &) = 0;

  virtual nuis::params::paramId_t GetParameterId(std::string const &) = 0;

  bool HandlesParameter(std::string const &param_name) {
    return (GetParameterId(param_name) != nuis::params::kParamUnhandled);
  }

  virtual void SetParameterValue(nuis::params::paramId_t, double) = 0;
  virtual bool ParametersVaried() = 0;
  virtual void Reconfigure() = 0;

  virtual nuis::event::FullEvent
  VaryFullEvent(nuis::event::FullEvent const &) = 0;

  virtual ~IVariationProvider() {}
};

DECLARE_PLUGIN_INTERFACE(IVariationProvider);

#endif
