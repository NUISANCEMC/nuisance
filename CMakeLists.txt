# Copyright 2018 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

set (NUISANCE_VERSION_MAJOR 3)
set (NUISANCE_VERSION_MINOR 0)
set (NUISANCE_VERSION_REVISION 0)

set (NUISANCE_VERSION_STRING "v${NUISANCE_VERSION_MAJOR}r${NUISANCE_VERSION_MINOR}")
if(${NUISANCE_VERSION_REVISION} STRGREATER "0")
  set (NUISANCE_VERSION_STRING "${NUISANCE_VERSION_STRING}p${NUISANCE_VERSION_REVISION}")
endif()

#Set this to TRUE to enable build debugging messages
set(BUILD_DEBUG_MSGS TRUE)

include(${CMAKE_SOURCE_DIR}/cmake/cmessage.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/cacheVariables.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/parseConfigApp.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/StringAndListUtils.cmake)

cmessage(STATUS "CMAKE_INSTALL_PREFIX: \"${CMAKE_INSTALL_PREFIX}\"")
cmessage(STATUS "CMAKE_BUILD_TYPE: \"${CMAKE_BUILD_TYPE}\"")

################################################################################
#                            Check Dependencies
################################################################################

##################################  ROOT  ######################################
include(${CMAKE_SOURCE_DIR}/cmake/ROOTSetup.cmake)

################################# InputHandler #################################
include(${CMAKE_SOURCE_DIR}/cmake/InputHandlerSetup.cmake)

#################################  Pythia6/8 ###################################
include(${CMAKE_SOURCE_DIR}/cmake/pythia6Setup.cmake)

#Want this before fhiclcpp which will add the install directory
include_directories(${CMAKE_SOURCE_DIR}/src)

################################## FHICLCPP ####################################
include(${CMAKE_SOURCE_DIR}/cmake/fhiclcppSetup.cmake)

################################## COMPILER ####################################
include(${CMAKE_SOURCE_DIR}/cmake/gperfSetup.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/c++CompilerSetup.cmake)

################################################################################

add_subdirectory(src)

add_subdirectory(scripts)

add_subdirectory(config)
add_subdirectory(data)
