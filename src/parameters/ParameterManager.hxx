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

#ifndef PARAMETERS_PARAMETERMANAGER_HXX_SEEN
#define PARAMETERS_PARAMETERMANAGER_HXX_SEEN

#include "exception/exception.hxx"

#include <limits>
#include <string>
#include <vector>

namespace fhicl {
class ParameterSet;
}

namespace nuis {
namespace params {

typedef size_t paramId_t;
static paramId_t const kParamUnhandled = std::numeric_limits<paramId_t>::max();
static double const kDefaultLimit = 0xdeadbeef;

class ParameterManager {
  struct NamedParameter {
    std::string name;
    std::string type;
    double value;
    double start;
    double min;
    double max;
    double step;
  };
  std::vector<NamedParameter> Parameters;
  // TMatrixD describing parameter covariance.

  ParameterManager();

  static ParameterManager *_global_inst;

  void ValidateParamId(paramId_t);

  bool locked;

public:
  static ParameterManager &Get();

  NEW_NUIS_EXCEPT(invalid_parameter_id);
  NEW_NUIS_EXCEPT(param_value_out_of_bounds);
  NEW_NUIS_EXCEPT(ambiguous_parameter_specified);
  NEW_NUIS_EXCEPT(parameter_list_is_locked);

  ///\brief Lock the parameter list so that subsequent calls to
  ///EnsureParameterRegistered cause an exception to be thrown.
  ///
  /// Useful for ensuring that plugins do not attempt to add parameters mid-fit.
  void LockParameterList();
  void UnlockParameterList();

  paramId_t EnsureParameterRegistered(fhicl::ParameterSet const &);
  paramId_t GetParameterId(std::string const &, std::string const &type = "");
  void SetParameterValue(paramId_t, double);
  double GetParameterValue(paramId_t);
  double GetParameterStep(paramId_t);
  double GetParameterStart(paramId_t);
  double GetParameterMin(paramId_t);
  double GetParameterMax(paramId_t);
  bool IsValidParameterValue(paramId_t, double);

  std::string StateString();
};
} // namespace params
} // namespace nuis

#endif
