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

set(TARGETS_TO_BUILD)

if(USE_MINIMIZER)
  add_executable(nuismin nuismin.cxx)

  set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};nuismin)

  target_link_libraries(nuismin ${MODULETargets})

  target_link_libraries(nuismin ${CMAKE_DEPENDLIB_FLAGS})
  target_link_libraries(nuismin ${ROOT_LIBS})

  if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
    set_target_properties(nuismin PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
  endif()
endif()

include_directories(${RWENGINE_INCLUDE_DIRECTORIES})

include_directories(${CMAKE_SOURCE_DIR}/src/Routines)
include_directories(${CMAKE_SOURCE_DIR}/src/FitBase)
include_directories(${CMAKE_SOURCE_DIR}/src/Utils)
include_directories(${CMAKE_SOURCE_DIR}/src/FCN)
include_directories(${CMAKE_SOURCE_DIR}/src/MCStudies)
include_directories(${EXP_INCLUDE_DIRECTORIES})

add_executable(DumpGiBUUEvents DumpGiBUUEvents.cxx)
set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};DumpGiBUUEvents)
target_link_libraries(DumpGiBUUEvents ${MODULETargets})
target_link_libraries(DumpGiBUUEvents ${CMAKE_DEPENDLIB_FLAGS})
target_link_libraries(DumpGiBUUEvents ${ROOT_LIBS})
include_directories(${CMAKE_SOURCE_DIR}/src/FitBase)
if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
  set_target_properties(DumpGiBUUEvents PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
endif()


add_executable(nuiscomp nuiscomp.cxx)
set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};nuiscomp)
target_link_libraries(nuiscomp ${MODULETargets})
target_link_libraries(nuiscomp ${CMAKE_DEPENDLIB_FLAGS})
target_link_libraries(nuiscomp ${ROOT_LIBS})
if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
  set_target_properties(nuiscomp PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
endif()

# PREPARE NUWRO
# Commented out for the time being until it is finished..
#if(USE_NuWro)
#  add_executable(PrepareNuwro PrepareNuwro.cxx)
#  set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};PrepareNuwro)
#  target_link_libraries(PrepareNuwro ${MODULETargets})
#  target_link_libraries(PrepareNuwro ${CMAKE_DEPENDLIB_FLAGS})
#  target_link_libraries(PrepareNuwro ${ROOT_LIBS})
#  if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
#    set_target_properties(PrepareNuwro PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
#  endif()
#endif()

install(TARGETS ${TARGETS_TO_BUILD} DESTINATION bin)
