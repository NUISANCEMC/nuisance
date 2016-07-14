# Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

################################################################################
#    This file is part of NuFiX.
#
#    NuFiX is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    NuFiX is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
################################################################################
add_executable(ExtFit_minimizer ExtFit_minimizer.cxx)

target_link_libraries(ExtFit_minimizer ${MODULETargets})

target_link_libraries(ExtFit_minimizer ${CMAKE_DEPENDLIB_FLAGS})

if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
  set_target_properties(ExtFit_minimizer PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
endif()

include_directories(${RWENGINE_INCLUDE_DIRECTORIES})

include_directories(${CMAKE_SOURCE_DIR}/src/Minimizer)
include_directories(${CMAKE_SOURCE_DIR}/src/FitBase)
include_directories(${CMAKE_SOURCE_DIR}/src/Utils)
include_directories(${CMAKE_SOURCE_DIR}/src/FCN)
include_directories(${CMAKE_SOURCE_DIR}/src/MCStudies)
include_directories(${EXP_INCLUDE_DIRECTORIES})


install(TARGETS ExtFit_minimizer DESTINATION bin)
