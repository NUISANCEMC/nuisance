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
cmake_minimum_required (VERSION 2.8 FATAL_ERROR)

#Use the compilers found in the path
find_program(CMAKE_C_COMPILER NAMES $ENV{CC} gcc PATHS ENV PATH NO_DEFAULT_PATH)
find_program(CMAKE_CXX_COMPILER NAMES $ENV{CXX} g++ PATHS ENV PATH NO_DEFAULT_PATH)

project(NUISANCE)

include(ExternalProject)

enable_language(Fortran)

set (NUISANCE_VERSION_MAJOR 2)
set (NUISANCE_VERSION_MINOR 7)
set (NUISANCE_VERSION_REVISION 0)

set (NUISANCE_VERSION_STRING "v${NUISANCE_VERSION_MAJOR}r${NUISANCE_VERSION_MINOR}")
if(${NUISANCE_VERSION_REVISION} STRGREATER "0")
  set (NUISANCE_VERSION_STRING "${NUISANCE_VERSION_STRING}p${NUISANCE_VERSION_REVISION}")
endif()

#Set this to TRUE to enable build debugging messages
set(BUILD_DEBUG_MSGS TRUE)
include(${CMAKE_SOURCE_DIR}/cmake/cmessage.cmake)

include(${CMAKE_SOURCE_DIR}/cmake/cacheVariables.cmake)

cmessage(STATUS "CMAKE_INSTALL_PREFIX: \"${CMAKE_INSTALL_PREFIX}\"")
cmessage(STATUS "CMAKE_BUILD_TYPE: \"${CMAKE_BUILD_TYPE}\"")

################################################################################
#                            Check Dependencies
################################################################################

##################################  ROOT  ######################################
include(${CMAKE_SOURCE_DIR}/cmake/ROOTSetup.cmake)

#################################  HEPMC  ######################################
include(${CMAKE_SOURCE_DIR}/cmake/HepMC.cmake)
###############################  HepMCNuEvt  ###################################
include(${CMAKE_SOURCE_DIR}/cmake/HepMCNuEvtSetup.cmake)

############################  Reweight Engines  ################################
include(${CMAKE_SOURCE_DIR}/cmake/ReweightEnginesSetup.cmake)

############################  Other Generators  ################################

include(${CMAKE_SOURCE_DIR}/cmake/GiBUUSetup.cmake)

if(USE_NUANCE)
  LIST(APPEND EXTRA_CXX_FLAGS -D__NUANCE_ENABLED__)
endif()

#################################  Pythia6/8  ####################################
include(${CMAKE_SOURCE_DIR}/cmake/pythia6Setup.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/pythia8Setup.cmake)

################################# gperftools ###################################

include(${CMAKE_SOURCE_DIR}/cmake/gperfSetup.cmake)

if(NOT NOTEST)
  enable_testing()
endif()

SET(GENERATOR_SUPPORT)

foreach(gen NEUT;NuWro;GENIE;GiBUU;NUANCE)
  if(USE_${gen})
    SET(GENERATOR_SUPPORT "${GENERATOR_SUPPORT}${gen} ")
  endif()
endforeach(gen)

cmessage(STATUS "Generator Input Support: ${GENERATOR_SUPPORT}")

set(MINCODE
  Routines
  FCN)

set(CORE
  MCStudies
  FitBase
  Config
  Logger
  InputHandler
  Splines
  Utils
  Statistical
  #Devel
  Smearceptance
  )

LIST(APPEND ALLEXPERIMENTS
  ANL
  ArgoNeuT
  BEBC
  BNL
  Electron
  FNAL
  GGM
  K2K
  MINERvA
  MicroBooNE
  MiniBooNE
  SciBooNE
  T2K)

foreach(exp ${ALLEXPERIMENTS})
  if(NOT NO_${exp})
    LIST(APPEND EXPERIMENTS_TO_BUILD ${exp})
  else()
    LIST(REVERSE EXTRA_CXX_FLAGS)
    LIST(APPEND EXTRA_CXX_FLAGS -D__NO_${exp}__)
    LIST(REVERSE EXTRA_CXX_FLAGS)
  endif()
endforeach()

################################## COMPILER ####################################
include(${CMAKE_SOURCE_DIR}/cmake/c++CompilerSetup.cmake)

################################### doxygen ###################################

include(${CMAKE_SOURCE_DIR}/cmake/docsSetup.cmake)

################################################################################


set(MINIMUM_INCLUDE_DIRECTORIES)
LIST(APPEND MINIMUM_INCLUDE_DIRECTORIES
  ${RWENGINE_INCLUDE_DIRECTORIES}
  ${CMAKE_SOURCE_DIR}/src/FitBase
  ${CMAKE_SOURCE_DIR}/src/Reweight
  ${CMAKE_SOURCE_DIR}/src/InputHandler
  ${CMAKE_SOURCE_DIR}/src/Config
  ${CMAKE_SOURCE_DIR}/src/Logger
  ${CMAKE_SOURCE_DIR}/src/Statistical
  ${CMAKE_SOURCE_DIR}/src/Splines
  ${CMAKE_SOURCE_DIR}/src/Utils)

#Not yet capable of just building without the rwdirs.
#if(USE_REWEIGHT)
  LIST(APPEND MINIMUM_INCLUDE_DIRECTORIES
    ${CMAKE_SOURCE_DIR}/src/Reweight)
  LIST(APPEND CORE Reweight)
#endif()

cmessage(DEBUG "Base include directories: ${MINIMUM_INCLUDE_DIRECTORIES}")

set(EXP_INCLUDE_DIRECTORIES)

foreach(edir ${EXPERIMENTS_TO_BUILD})
  LIST(APPEND EXP_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/src/${edir})
endforeach()
cmessage(DEBUG "Included experiments: ${EXP_INCLUDE_DIRECTORIES}")

foreach(mdir ${MINCODE})
  cmessage (DEBUG "Configuring directory: src/${mdir}")
  add_subdirectory(src/${mdir})
endforeach()

foreach(edir ${EXPERIMENTS_TO_BUILD})
  cmessage (DEBUG "Configuring directory: src/${edir}")
  add_subdirectory(src/${edir})
endforeach()

foreach(cdir ${CORE})
  cmessage (DEBUG "Configuring directory: src/${cdir}")
  add_subdirectory(src/${cdir})
endforeach()

cmessage(DEBUG "Module targets: ${MODULETargets}")

LIST(APPEND MODULETargets -Wl,--end-group)
LIST(REVERSE MODULETargets)
LIST(APPEND MODULETargets -Wl,--start-group)
LIST(REVERSE MODULETargets)

add_subdirectory(app)
add_subdirectory(src/Tests)

configure_file(cmake/setup.sh.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/setup.sh" @ONLY)
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/setup.sh" DESTINATION
  ${CMAKE_INSTALL_PREFIX})

configure_file(cmake/MakeBinaryBlob.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/MakeBinaryBlob" @ONLY)
install(PROGRAMS
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/MakeBinaryBlob" DESTINATION
  bin)

if(USE_DYNSAMPLES)
  SET(ALL_INCLUDES ${MINIMUM_INCLUDE_DIRECTORIES})
  LIST(APPEND ALL_INCLUDES ${CMAKE_SOURCE_DIR}/src/Smearceptance)
  LIST(APPEND ALL_INCLUDES ${EXP_INCLUDE_DIRECTORIES})

  string(REPLACE ";" " -I" ALL_INCLUDES_STR "${ALL_INCLUDES}")
  string(REPLACE "${CMAKE_SOURCE_DIR}/src" "${CMAKE_INSTALL_PREFIX}/include"
                 ALL_INCLUDES_STR "${ALL_INCLUDES_STR}")

  cmessage(DEBUG ${CMAKE_DEPENDLIB_FLAGS})
  string(REPLACE "-levent " "" CMAKE_DEPENDLIB_FLAGS_NEW ${CMAKE_DEPENDLIB_FLAGS})
  set(CMAKE_DEPENDLIB_FLAGS ${CMAKE_DEPENDLIB_FLAGS_NEW})
  cmessage(DEBUG ${CMAKE_DEPENDLIB_FLAGS})

  string(REPLACE ";" " -l" ALL_MODULETARGETS_STR "${MODULETargets}")
  string(REPLACE "-l-" "-" ALL_MODULETARGETS_STR "${ALL_MODULETARGETS_STR}")

  configure_file(cmake/BuildDynamicSample.in
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/BuildDynamicSample" @ONLY)
  install(PROGRAMS
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/BuildDynamicSample" DESTINATION
    bin)

  configure_file(cmake/BuildDynamicSmearcepter.in
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/BuildDynamicSmearcepter" @ONLY)
  install(PROGRAMS
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/BuildDynamicSmearcepter" DESTINATION
    bin)
endif()

install(PROGRAMS
  "${PROJECT_SOURCE_DIR}/scripts/nuiscardgen" DESTINATION
  bin)

install(PROGRAMS
  "${PROJECT_SOURCE_DIR}/scripts/nuissamples" DESTINATION
  bin)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/data DESTINATION
                  ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY ${PROJECT_SOURCE_DIR}/event_gen DESTINATION
                  ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY ${PROJECT_SOURCE_DIR}/parameters DESTINATION
                  ${CMAKE_INSTALL_PREFIX})
