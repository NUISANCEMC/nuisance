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

#ifndef SAMPLES_ISAMPLE_HXX_SEEN
#define SAMPLES_ISAMPLE_HXX_SEEN

#include "plugins/traits.hxx"

#include "exception/exception.hxx"

namespace fhicl {
class ParameterSet;
}

namespace nuis {
namespace core {
class FullEvent;
class MinimalEvent;
} // namespace core
} // namespace nuis

class ISample {
public:
  NEW_NUIS_EXCEPT(uninitialized_ISample);

  virtual void Initialize(fhicl::ParameterSet const &) = 0;

  virtual void ProcessSample() = 0;

  virtual void Write() = 0;

  virtual ~ISample(){}
};

DECLARE_PLUGIN_INTERFACE(ISample);

#endif
