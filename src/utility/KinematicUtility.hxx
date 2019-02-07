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

#ifndef UTILITY_KINEMATICUTILITY_HXX_SEEN
#define UTILITY_KINEMATICUTILITY_HXX_SEEN

#include <limits>
#include <utility>

namespace nuis {
namespace event {
class FullEvent;
}
} // namespace nuis

namespace nuis {
namespace utility {

double GetNeutrinoEQERec(event::FullEvent const &fev,
                         double SeparationEnergy_MeV);
double GetNeutrinoQ2QERec(event::FullEvent const &fev,
                          double SeparationEnergy_MeV);

struct ENuRange : public std::pair<double, double> {
  using std::pair<double, double>::pair;
  using std::pair<double, double>::operator=;

  ENuRange()
      : std::pair<double, double>(0, std::numeric_limits<double>::max()) {}

  bool IsInRange(double enu) { return (enu > second) || (enu < first); }
};

} // namespace utility
} // namespace nuis

#endif
