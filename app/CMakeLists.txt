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

  add_executable(nuissplines nuissplines.cxx)
  set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};nuissplines)

endif()

include_directories(${RWENGINE_INCLUDE_DIRECTORIES})
include_directories(${CMAKE_SOURCE_DIR}/src/Routines)
include_directories(${CMAKE_SOURCE_DIR}/src/InputHandler)
include_directories(${CMAKE_SOURCE_DIR}/src/Genie)
include_directories(${CMAKE_SOURCE_DIR}/src/FitBase)
include_directories(${CMAKE_SOURCE_DIR}/src/Statistical)
include_directories(${CMAKE_SOURCE_DIR}/src/Utils)
include_directories(${CMAKE_SOURCE_DIR}/src/Config)
include_directories(${CMAKE_SOURCE_DIR}/src/Logger)
include_directories(${CMAKE_SOURCE_DIR}/src/Splines)
include_directories(${CMAKE_SOURCE_DIR}/src/Reweight)
include_directories(${CMAKE_SOURCE_DIR}/src/FCN)
include_directories(${CMAKE_SOURCE_DIR}/src/MCStudies)
include_directories(${CMAKE_SOURCE_DIR}/src/Smearceptance)
include_directories(${EXP_INCLUDE_DIRECTORIES})

if (USE_NuWro AND NOT NUWRO_BUILT_FROM_FILE)
  add_executable(nuwro_nuisance nuwro_NUISANCE.cxx)
  set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};nuwro_nuisance)
endif()

if (USE_GiBUU)
  add_executable(DumpGiBUUEvents DumpGiBUUEvents.cxx)
  set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};DumpGiBUUEvents)
endif()

add_executable(nuis_flat_tree_combiner nuis_flat_tree_combiner.cxx)
set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};nuis_flat_tree_combiner)

add_executable(nuiscomp nuiscomp.cxx)
set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};nuiscomp)

add_executable(nuisflat nuisflat.cxx)
set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};nuisflat)

add_executable(nuissmear nuissmear.cxx)
set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};nuissmear)

add_executable(nuissyst nuissyst.cxx)
set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};nuissyst)

add_executable(nuisbayes nuisbayes.cxx)
set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};nuisbayes)

if(USE_GENIE)
  add_executable(PrepareGENIE PrepareGENIE.cxx)
  set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};PrepareGENIE)
endif()

if(USE_NEUT)
  add_executable(PrepareNEUT PrepareNEUT.cxx)
  set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};PrepareNEUT)
endif()

# PREPARE NUWRO
# Commented out for the time being until it is finished..
if(USE_NuWro)
  add_executable(PrepareNuwro PrepareNuwroEvents.cxx)
  set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};PrepareNuwro)
endif()

add_executable(nuisbac nuisbac.cxx)
set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};nuisbac)

foreach(targ ${TARGETS_TO_BUILD})
  if(NOT "${CMAKE_LINK_FLAGS} ${NUIS_EXE_FLAGS}" STREQUAL " ")
    set_target_properties(${targ} PROPERTIES LINK_FLAGS "${CMAKE_LINK_FLAGS} ${NUIS_EXE_FLAGS}")
  endif()
  target_link_libraries(${targ} ${MODULETargets})
  target_link_libraries(${targ} ${CMAKE_DEPENDLIB_FLAGS})
endforeach()

install(TARGETS ${TARGETS_TO_BUILD} DESTINATION bin)