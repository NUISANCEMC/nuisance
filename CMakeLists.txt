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
cmake_minimum_required (VERSION 2.6 FATAL_ERROR)

project(ExternalDataFitter)

set (ExtFit_VERSION_MAJOR 1)
set (ExtFit_VERSION_MINOR 0) #The q+1'th letter of the alphabet
set (ExtFit_VERSION_REVISION 0)

set (VERBOSE TRUE)

set (CMAKE_SKIP_BUILD_RPATH TRUE)

include(${CMAKE_SOURCE_DIR}/cmake/cmessage.cmake)

if(NOT DEFINED USE_NEUT AND
  NOT DEFINED USE_NuWro AND
  NOT DEFINED USE_GENIE AND
  NOT DEFINED USE_NIWG)
  cmessage(FATAL_ERROR "No reweight engines requested. Configure with at least "
    "one of -DUSE_{NEUT,NuWro,GENIE,NIWG}.")
else()
  cmessage(STATUS "Reweight engines: NEUT:${USE_NEUT},  NuWro:${USE_NuWro},  GENIE:${USE_GENIE},  NIWG:${USE_NIWG}")
endif()
#Changes default install path to be a subdirectory of the build dir.
#Can set build dir at configure time with -DCMAKE_INSTALL_PREFIX=/install/path
if(CMAKE_INSTALL_PREFIX STREQUAL "" OR CMAKE_INSTALL_PREFIX STREQUAL
  "/usr/local")
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}")
elseif(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}")
endif()

cmessage(STATUS "CMAKE_INSTALL_PREFIX: \"${CMAKE_INSTALL_PREFIX}\"")

if(CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE DEBUG)
elseif(NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE DEBUG)
endif()

cmessage(STATUS "CMAKE_BUILD_TYPE: \"${CMAKE_BUILD_TYPE}\"")

################################################################################
#                            Check Dependencies
################################################################################

##################################  ROOT  ######################################
include(${CMAKE_SOURCE_DIR}/cmake/ROOTSetup.cmake)

############################  Reweight Engines  ################################
include(${CMAKE_SOURCE_DIR}/cmake/ReweightEnginesSetup.cmake)

################################## COMPILER ####################################
include(${CMAKE_SOURCE_DIR}/cmake/c++CompilerSetup.cmake)

################################################################################

set(CORE
    Utils
    FitBase
    MCStudies
    )
    
set(MINCODE
  Minimizer
  FCN)

set(EXPERIMENTS
  ANL
  BEBC
  BNL
  FNAL
  GGM
  K2K
  MINERvA
  MiniBooNE
  T2K)

set(EXP_INCLUDE_DIRECTORIES)

foreach(cdir ${CORE})
  cmessage (STATUS "Configuring directory: src/${cdir}")
  add_subdirectory(src/${cdir})
endforeach()

foreach(edir ${EXPERIMENTS})
  set(EXP_INCLUDE_DIRECTORIES ${EXP_INCLUDE_DIRECTORIES};${CMAKE_SOURCE_DIR}/src/${edir})
endforeach()
cmessage(STATUS "Included experiments: ${EXP_INCLUDE_DIRECTORIES}")

foreach(mdir ${MINCODE})
  cmessage (STATUS "Configuring directory: src/${mdir}")
  add_subdirectory(src/${mdir})
endforeach()

foreach(edir ${EXPERIMENTS})
  cmessage (STATUS "Configuring directory: src/${edir}")
  add_subdirectory(src/${edir})
endforeach()

cmessage(STATUS "Module targets:  ${MODULETargets}")

add_subdirectory(app)

configure_file(cmake/setup.sh.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/setup.sh" @ONLY)
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/setup.sh" DESTINATION
  ${CMAKE_INSTALL_PREFIX})
