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

#ifndef VARIATION_VARIATIONMANAGER_HXX_SEEN
#define VARIATION_VARIATIONMANAGER_HXX_SEEN

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
class VariationManager {
public:
  typedef size_t VarProv_id_t;

private:
  struct NamedWeightProvider {
    NamedWeightProvider(
        std::string const &,
        plugins::plugin_traits<IWeightProvider>::unique_ptr_t &&);
    std::string name;
    plugins::plugin_traits<IWeightProvider>::unique_ptr_t handler;
  };
  std::vector<NamedWeightProvider> VarProvs;

  VariationManager();

  static VariationManager *_global_inst;

public:

  static VariationManager &Get();

  paramId_t EnsureParameterHandled(fhicl::ParameterSet const &);
  void SetParameterValue(paramId_t, double);
  void GetParameterPull(paramId_t);

  double GetEventWeight(nuis::event::MinimalEvent const &);
};
} // namespace variation
} // namespace nuis

#endif
