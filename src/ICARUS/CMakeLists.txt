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

set(ICARUS_Impl_Files

  ICARUS_NuMI_CCQELike_numu_2026.cxx

  ICARUSUtils.cxx
  ICARUS_SignalDef.cxx
)

add_library(ICARUS SHARED ${ICARUS_Impl_Files})
target_link_libraries(ICARUS FrameworkLibraries CoreIncludes)

set_target_properties(ICARUS PROPERTIES PUBLIC_HEADER "ICARUS_SignalDef.h")

install(TARGETS ICARUS
    EXPORT nuisance-targets
    LIBRARY DESTINATION lib/
    PUBLIC_HEADER DESTINATION include)

add_library(NUIS::ICARUS ALIAS ICARUS)

