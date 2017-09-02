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
  # target_link_libraries(nuismin ${ROOT_LIBS})
  if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
    set_target_properties(nuismin PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
  endif()

  add_executable(nuissplines nuissplines.cxx)
  set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};nuissplines)
  target_link_libraries(nuissplines ${MODULETargets})
  target_link_libraries(nuissplines ${CMAKE_DEPENDLIB_FLAGS})
  # target_link_libraries(nuissplines ${ROOT_LIBS})
  if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
    set_target_properties(nuissplines PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
  endif()

endif()

include_directories(${RWENGINE_INCLUDE_DIRECTORIES})
include_directories(${CMAKE_SOURCE_DIR}/src/Routines)
include_directories(${CMAKE_SOURCE_DIR}/src/InputHandler)
include_directories(${CMAKE_SOURCE_DIR}/src/Genie)
include_directories(${CMAKE_SOURCE_DIR}/src/FitBase)
include_directories(${CMAKE_SOURCE_DIR}/src/Utils)
include_directories(${CMAKE_SOURCE_DIR}/src/Config)
include_directories(${CMAKE_SOURCE_DIR}/src/Logger)
include_directories(${CMAKE_SOURCE_DIR}/src/Statistical)
include_directories(${CMAKE_SOURCE_DIR}/src/Splines)
include_directories(${CMAKE_SOURCE_DIR}/src/Reweight)
include_directories(${CMAKE_SOURCE_DIR}/src/FCN)
include_directories(${CMAKE_SOURCE_DIR}/src/MCStudies)
include_directories(${CMAKE_SOURCE_DIR}/src/Smearceptance)
include_directories(${EXP_INCLUDE_DIRECTORIES})

if (USE_NuWro AND NOT NUWRO_BUILT_FROM_FILE)
  add_executable(nuwro_nuisance nuwro_NUISANCE.cxx)
  set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};nuwro_nuisance)
  target_link_libraries(nuwro_nuisance ${MODULETargets})
  target_link_libraries(nuwro_nuisance ${CMAKE_DEPENDLIB_FLAGS})
  # target_link_libraries(nuwro_nuisance ${ROOT_LIBS})
  include_directories(${CMAKE_SOURCE_DIR}/src/FitBase)
  if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
    set_target_properties(nuwro_nuisance PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
  endif()
endif()

if (USE_NEUT)
  add_executable(neut_nuisance neut_NUISANCE.cxx)
  set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};neut_nuisance)
  target_link_libraries(neut_nuisance ${MODULETargets})
  target_link_libraries(neut_nuisance ${CMAKE_DEPENDLIB_FLAGS})
  target_link_libraries(neut_nuisance ${ROOT_LIBS})
  include_directories(${CMAKE_SOURCE_DIR}/src/FitBase)
  if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
    set_target_properties(neut_nuisance PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
  endif()
endif()

if (BUILD_GEVGEN)
  add_executable(gevgen_nuisance gEvGen_NUISANCE.cxx)
  set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};gevgen_nuisance)
  target_link_libraries(gevgen_nuisance ${MODULETargets})
  target_link_libraries(gevgen_nuisance ${CMAKE_DEPENDLIB_FLAGS})
  # target_link_libraries(gevgen_nuisance ${ROOT_LIBS})
  include_directories(${CMAKE_SOURCE_DIR}/src/FitBase)
  include_directories(${GENIE_INCLUDES}/Apps)
  include_directories(${GENIE_INCLUDES}/FluxDrivers)
  include_directories(${GENIE_INCLUDES}/EVGDrivers)
  if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
    set_target_properties(gevgen_nuisance PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
  endif()


  add_executable(gevgen_nuisance_mixed gEvGen_NUISANCE_MIXED.cxx)
  set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};gevgen_nuisance_mixed)
  target_link_libraries(gevgen_nuisance_mixed ${MODULETargets})
  target_link_libraries(gevgen_nuisance_mixed ${CMAKE_DEPENDLIB_FLAGS})
  # target_link_libraries(gevgen_nuisance_mixed ${ROOT_LIBS})
  include_directories(${CMAKE_SOURCE_DIR}/src/FitBase)
  include_directories(${GENIE_INCLUDES}/Apps)
  include_directories(${GENIE_INCLUDES}/FluxDrivers)
  include_directories(${GENIE_INCLUDES}/EVGDrivers)
  if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
    set_target_properties(gevgen_nuisance_mixed PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
  endif()
endif()

if (USE_GiBUU)
  add_executable(DumpGiBUUEvents DumpGiBUUEvents.cxx)
  set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};DumpGiBUUEvents)
  target_link_libraries(DumpGiBUUEvents ${MODULETargets})
  target_link_libraries(DumpGiBUUEvents ${CMAKE_DEPENDLIB_FLAGS})
  # target_link_libraries(DumpGiBUUEvents ${ROOT_LIBS})
  include_directories(${CMAKE_SOURCE_DIR}/src/FitBase)
  if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
    set_target_properties(DumpGiBUUEvents PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
  endif()
endif()

add_executable(nuiscomp nuiscomp.cxx)
set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};nuiscomp)
target_link_libraries(nuiscomp ${MODULETargets})
target_link_libraries(nuiscomp ${CMAKE_DEPENDLIB_FLAGS})
# target_link_libraries(nuiscomp ${ROOT_LIBS})
if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
  set_target_properties(nuiscomp PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
endif()

add_executable(nuisflat nuisflat.cxx)
set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};nuisflat)
target_link_libraries(nuisflat ${MODULETargets})
target_link_libraries(nuisflat ${CMAKE_DEPENDLIB_FLAGS})
# target_link_libraries(nuisflat ${ROOT_LIBS})
if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
  set_target_properties(nuisflat PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
endif()

add_executable(nuissmear nuissmear.cxx)
set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};nuissmear)
target_link_libraries(nuissmear ${MODULETargets})
target_link_libraries(nuissmear ${CMAKE_DEPENDLIB_FLAGS})
# target_link_libraries(nuissmear ${ROOT_LIBS})
if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
  set_target_properties(nuissmear PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
endif()

add_executable(nuissyst nuissyst.cxx)
set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};nuissyst)
target_link_libraries(nuissyst ${MODULETargets})
target_link_libraries(nuissyst ${CMAKE_DEPENDLIB_FLAGS})
# target_link_libraries(nuissyst ${ROOT_LIBS})
if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
  set_target_properties(nuissyst PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
endif()

add_executable(nuisbayes nuisbayes.cxx)
set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};nuisbayes)
target_link_libraries(nuisbayes ${MODULETargets})
target_link_libraries(nuisbayes ${CMAKE_DEPENDLIB_FLAGS})
# target_link_libraries(nuisbayes ${ROOT_LIBS})
if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
  set_target_properties(nuisbayes PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
endif()

if(USE_GENIE)
  add_executable(PrepareGENIE PrepareGENIE.cxx)
  set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};PrepareGENIE)
  target_link_libraries(PrepareGENIE ${MODULETargets})
  target_link_libraries(PrepareGENIE ${CMAKE_DEPENDLIB_FLAGS})
  # target_link_libraries(PrepareGENIE ${ROOT_LIBS})
  if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
     set_target_properties(PrepareGENIE PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
  endif()
endif()

if(USE_NEUT)
  add_executable(PrepareNEUT PrepareNEUT.cxx)
  set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};PrepareNEUT)
  target_link_libraries(PrepareNEUT ${MODULETargets})
  target_link_libraries(PrepareNEUT ${CMAKE_DEPENDLIB_FLAGS})
  # target_link_libraries(PrepareNEUT ${ROOT_LIBS})
  if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
     set_target_properties(PrepareNEUT PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
  endif()
endif()

# PREPARE NUWRO
# Commented out for the time being until it is finished..
if(USE_NuWro)
  add_executable(PrepareNuwro PrepareNuwroEvents.cxx)
  set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};PrepareNuwro)
  target_link_libraries(PrepareNuwro ${MODULETargets})
  target_link_libraries(PrepareNuwro ${CMAKE_DEPENDLIB_FLAGS})
  # target_link_libraries(PrepareNuwro ${ROOT_LIBS})
  if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
    set_target_properties(PrepareNuwro PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
  endif()
endif()

install(TARGETS ${TARGETS_TO_BUILD} DESTINATION bin)

#add_executable(DumpROOTClassesFromVector DumpROOTClassesFromVector.cxx)
#  #Strip out -lNuWro_event1
#  string(REPLACE "-lNuWro_event1" "" NWEVSTRIPPED_CDF ${CMAKE_DEPENDLIB_FLAGS})
#  cmessage(DEBUG "Attempted to strip out nuwro library: \"${CMAKE_DEPENDLIB_FLAGS}\" -> \"${NWEVSTRIPPED_CDF}\"")
#  add_executable(PrepareNEUT PrepareNEUT.cxx)
#  target_link_libraries(DumpROOTClassesFromVector ${MODULETargets})
#  target_link_libraries(DumpROOTClassesFromVector ${NWEVSTRIPPED_CDF})
#  if(NOT CMAKE_LINK_FLAGS STREQUAL "")
#    set_target_properties(DumpROOTClassesFromVector PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
#  endif()
#install(TARGETS DumpROOTClassesFromVector DESTINATION bin)

