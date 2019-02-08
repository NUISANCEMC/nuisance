# Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

include_directories(${MINIMUM_INCLUDE_DIRECTORIES})
include_directories(${CMAKE_SOURCE_DIR}/src/Routines)
include_directories(${CMAKE_SOURCE_DIR}/src/FCN)
include_directories(${CMAKE_SOURCE_DIR}/src/MCStudies)
include_directories(${CMAKE_SOURCE_DIR}/src/Smearceptance)
include_directories(${EXP_INCLUDE_DIRECTORIES})

SET(TESTAPPS SignalDefTests ParserTests SmearceptanceTests)

if(USE_MINIMIZER)
  # LIST(APPEND TESTAPPS FitMechanicsTests)
endif()

foreach(appimpl ${TESTAPPS})
  add_executable(${appimpl} ${appimpl}.cxx)
  set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};${appimpl})
  target_link_libraries(${appimpl} ${MODULETargets})
  target_link_libraries(${appimpl} ${NUISANCE_DEPEND_LIBS})
  target_link_libraries(${appimpl} ${ROOT_LIBS})
  if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
    set_target_properties(${appimpl} PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
  endif()
  install(TARGETS ${appimpl} DESTINATION tests)
  add_test(${appimpl} ${appimpl} 1)
endforeach()

list (FIND TESTAPPS FitMechanicsTests _index)
if (${_index} GREATER -1)
  add_library(DummySample SHARED DummySample.cxx)
  target_link_libraries(DummySample ${MODULETargets})
  target_link_libraries(DummySample ${NUISANCE_DEPEND_LIBS})
  target_link_libraries(DummySample ${ROOT_LIBS})

  if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
    set_target_properties(DummySample PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
  endif()

  install(TARGETS DummySample DESTINATION tests)
endif()
