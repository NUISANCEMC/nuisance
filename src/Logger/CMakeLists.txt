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

set(Logger_Impl_Files
  FitLogger.cxx
  Initialiser.cxx
)

set(Logger_Hdr_Files
  FitLogger.h
  Initialiser.h
)

add_library(PythiaQuiet SHARED PythiaQuiet.f)

add_library(Logger SHARED ${Logger_Impl_Files})
target_link_libraries(Logger CoreIncludes ROOT::ROOT PythiaQuiet)

install(TARGETS Logger DESTINATION lib)
set_target_properties(Logger PROPERTIES PUBLIC_HEADER "${Logger_Hdr_Files}")

install(TARGETS Logger PythiaQuiet
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include)
