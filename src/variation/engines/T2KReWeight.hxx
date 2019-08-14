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

#include "T2KSyst.h"

#include <memory>
#include <vector>

namespace t2krew {
class T2KReWeight;
}

class T2KReWeightEngine : public IWeightProvider {

  struct T2KSystParam {
    nuis::params::paramId_t pid;
    t2krew::T2KSyst_t t2ksyst;
  };

  std::vector<T2KSystParam> fT2KSysts;
  std::unique_ptr<t2krew::T2KReWeight> fT2KRW;

public:
  NEW_NUIS_EXCEPT(invalid_T2K_syst_name);

  void Initialize(fhicl::ParameterSet const &);
  void Reconfigure();
  double GetEventWeight(nuis::event::MinimalEvent const &);
  std::string GetName();
  std::string GetDocumentation();
  fhicl::ParameterSet GetExampleConfiguration();
  nuis::GeneratorManager::Generator_id_t GetGeneratorId();
};

#endif
