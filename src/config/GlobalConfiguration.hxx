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

#ifndef CONFIG_GLOBALCONFIGURATION_HXX_SEEN
#define CONFIG_GLOBALCONFIGURATION_HXX_SEEN

#include <string>

namespace fhicl {
class ParameterSet;
}

namespace nuis {
namespace config {
fhicl::ParameterSet const &GetDocument(std::string const &name = "global");
bool EnsureConfigurationRead(std::string const &fhicl_file,
                             std::string const &name = "global");

} // namespace config
} // namespace nuis

#endif
