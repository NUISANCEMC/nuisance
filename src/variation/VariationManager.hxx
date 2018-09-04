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
namespace input {
class VariationManager {
  struct NamedVariationProvider {
    NamedVariationProvider(
        std::string const &,
        plugins::plugin_traits<IWeightProvider>::unique_ptr_t &&);
    std::string name;
    plugins::plugin_traits<IWeightProvider>::unique_ptr_t handler;
  };
  std::vector<NamedVariationProvider> VarProvs;

  VariationManager();

  static VariationManager *_global_inst;

public:
  typedef size_t VarProv_id_t;
  typedef size_t paramId_t;

  struct VariationProviderParameter {
    std::string name;
    VarProv_id_t providerId;
    IVariationProvider::paramId_t providerParameterId;
  };

  std::vector<VariationProviderParameter> VarParams;


  static VariationManager &Get();

  paramId_t EnsureParameterHandled(fhicl::ParameterSet const &);
  void SetParameterValue(paramId_t, double);
  void GetParameterPull(paramId_t);

  double GetEventWeight();
};
} // namespace input
} // namespace nuis

#endif
