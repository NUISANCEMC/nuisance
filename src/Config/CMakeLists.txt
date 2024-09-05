# Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

################################################################################
#    This file is part of NUISANCE.
#
#    NUISANCE is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    NUISANCE is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with NUISANCE.  If not, see <http://www.gnu.org/licenses/>.
################################################################################

set(Config_Impl_Files
  NuisConfig.cxx
  NuisKey.cxx
)

set(Config_Hdr_Files
  NuisConfig.h
  NuisKey.h
)

add_library(Config SHARED ${Config_Impl_Files})
target_link_libraries(Config CoreIncludes ROOT::ROOT)

install(TARGETS Config DESTINATION lib)
set_target_properties(Config PROPERTIES PUBLIC_HEADER "${Config_Hdr_Files}")

install(TARGETS Config
    EXPORT nuisance-targets
    LIBRARY DESTINATION lib/
    PUBLIC_HEADER DESTINATION include)

add_library(NUIS::Config ALIAS Config)