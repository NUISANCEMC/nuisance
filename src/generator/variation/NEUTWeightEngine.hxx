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

#ifndef GENERATOR_VARIATION_NEUTREWEIGHT_HXX_SEEN
#define GENERATOR_VARIATION_NEUTREWEIGHT_HXX_SEEN

#include "variation/IWeightProvider.hxx"

#include "exception/exception.hxx"

#include "parameters/ParameterManager.hxx"

#include "NSyst.h"

#include <memory>
#include <vector>

namespace neut {
namespace rew {
class NReWeight;
}
} // namespace neut

class NEUTWeightEngine : public IWeightProvider {

  struct NEUTSystParam {
    nuis::params::paramId_t pid;
    neut::rew::NSyst_t nsyst;
  };

  std::vector<NEUTSystParam> fNEUTSysts;
  std::unique_ptr<neut::rew::NReWeight> fNeutRW;

public:
  NEW_NUIS_EXCEPT(invalid_NEUT_syst_name);

  void Initialize(fhicl::ParameterSet const &);
  void Reconfigure();
  double GetEventWeight(nuis::event::MinimalEvent const &);
  std::string GetName();
  std::string GetDocumentation();
  fhicl::ParameterSet GetExampleConfiguration();
};

#endif
