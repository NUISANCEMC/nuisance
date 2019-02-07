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

#include "variation/IWeightProvider.hxx"

#include "plugins/traits.hxx"

#include "exception/exception.hxx"

#include <string>
#include <vector>

namespace fhicl {
class ParameterSet;
}

namespace nuis {
namespace variation {

///\brief Provides event-weight responses for reweighting tools
class WeightManager {
public:
  typedef size_t WeightProv_id_t;

private:
  struct NamedWeightProvider {
    NamedWeightProvider(
        std::string const &,
        plugins::plugin_traits<IWeightProvider>::unique_ptr_t &&);
    NamedWeightProvider(NamedWeightProvider const &) = delete;
    NamedWeightProvider(NamedWeightProvider &&);
    std::string name;
    plugins::plugin_traits<IWeightProvider>::unique_ptr_t handler;
  };
  std::vector<NamedWeightProvider> WeightEngines;

  WeightManager();

  static WeightManager *_global_inst;

public:
  static WeightManager &Get();

  WeightProv_id_t EnsureWeightProviderLoaded(fhicl::ParameterSet const &);
  double GetEventWeight(nuis::event::MinimalEvent const &);

  void ReconfigureWeightEngines();
};
} // namespace variation
} // namespace nuis
