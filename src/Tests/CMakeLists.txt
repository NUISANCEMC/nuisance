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

include_directories(${RWENGINE_INCLUDE_DIRECTORIES})
include_directories(${CMAKE_SOURCE_DIR}/src/Routines)
include_directories(${CMAKE_SOURCE_DIR}/src/FitBase)
include_directories(${CMAKE_SOURCE_DIR}/src/InputHandler)
include_directories(${CMAKE_SOURCE_DIR}/src/Genie)
include_directories(${CMAKE_SOURCE_DIR}/src/Utils)
include_directories(${CMAKE_SOURCE_DIR}/src/Reweight)
include_directories(${CMAKE_SOURCE_DIR}/src/Splines)
include_directories(${CMAKE_SOURCE_DIR}/src/FCN)
include_directories(${CMAKE_SOURCE_DIR}/src/MCStudies)
include_directories(${EXP_INCLUDE_DIRECTORIES})

SET(TESTAPPS SignalDefTests ParserTests)

foreach(appimpl ${TESTAPPS})
  add_executable(${appimpl} ${appimpl}.cxx)
  set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};${appimpl})
  target_link_libraries(${appimpl} ${MODULETargets})
  target_link_libraries(${appimpl} ${CMAKE_DEPENDLIB_FLAGS})
  target_link_libraries(${appimpl} ${ROOT_LIBS})
  if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
    set_target_properties(${appimpl} PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
  endif()
  install(TARGETS ${appimpl} DESTINATION tests)
  add_test(${appimpl} ${appimpl} 1)
endforeach()
