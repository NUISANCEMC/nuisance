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
cmake_minimum_required (VERSION 3.14 FATAL_ERROR)

#Use the compilers found in the path
find_program(CMAKE_C_COMPILER NAMES $ENV{CC} gcc PATHS ENV PATH NO_DEFAULT_PATH)
find_program(CMAKE_CXX_COMPILER NAMES $ENV{CXX} g++ PATHS ENV PATH NO_DEFAULT_PATH)

project(NUISANCE)

enable_language(Fortran)
set (NUISANCE_VERSION 2.9.9)

#Changes default install path to be a subdirectory of the build dir.
#Can set build dir at configure time with -DCMAKE_INSTALL_PREFIX=/install/path
if(CMAKE_INSTALL_PREFIX STREQUAL "" OR CMAKE_INSTALL_PREFIX STREQUAL
  "/usr/local")
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}")
elseif(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE RELWITHDEBINFO)
elseif(NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RELWITHDEBINFO)
endif()

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

include(CPM)
if(DEFINED ENV{NuHepMCModules_ROOT})
  include($ENV{NuHepMCModules_ROOT}/NuHepMCModules.cmake)
else()
  CPMFindPackage(
      NAME CMakeModules
      GIT_TAG v0.0.10
      GITHUB_REPOSITORY NuHepMC/CMakeModules
      DOWNLOAD_ONLY
  )
  include(${CMakeModules_SOURCE_DIR}/NuHepMCModules.cmake)
endif()

################################################################################
#                            Check Dependencies
################################################################################

#Set this to TRUE to enable build debugging messages
set(BUILD_DEBUG_MSGS TRUE)
include(CMessage)
include(NuHepMCUtils)

include(NUISANCEDependencies)

add_subdirectory(src)

add_subdirectory(app)

SET(CONFIG_COMPILE_DEFINTIONS)
foreach(FEATURE 
  T2KReWeight
  NIWGLegacy
  NOvARwgt
  nusystematics
  NuWro
  Prob3plusplus)
  
  if(${FEATURE}_ENABLED)
    LIST(APPEND CONFIG_COMPILE_DEFINTIONS -D${FEATURE}_ENABLED)
  endif()
endforeach()

if(GENIE_ENABLED)
  LIST(APPEND CONFIG_COMPILE_DEFINTIONS -DGENIE_ENABLED)
  LIST(APPEND CONFIG_INCLUDE_DIRECTORIES -I${GENIE_INC_DIR})
  if(GENIE3_FOUND)
      LIST(APPEND CONFIG_COMPILE_DEFINTIONS -DGENIE3_API_ENABLED)
  endif()
endif()

if(NEUT_ENABLED)
  LIST(APPEND CONFIG_COMPILE_DEFINTIONS -DNEUT_ENABLED)
  LIST(APPEND CONFIG_INCLUDE_DIRECTORIES -I${NEUT_INCLUDE_DIR})
endif()

STRING(REPLACE ";" " " CONFIG_COMPILE_DEFINTIONS "${CONFIG_COMPILE_DEFINTIONS}")
STRING(REPLACE ";" " " CONFIG_INCLUDE_DIRECTORIES "${CONFIG_INCLUDE_DIRECTORIES}")


configure_file(cmake/Templates/setup.sh.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/setup.sh" @ONLY)
configure_file(cmake/Templates/BuildDynamicSample.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/BuildDynamicSample" @ONLY)
configure_file(cmake/Templates/BuildDynamicSmearcepter.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/BuildDynamicSmearcepter" @ONLY)

install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/setup.sh" 
  DESTINATION
    ${CMAKE_INSTALL_PREFIX})

install(PROGRAMS
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/BuildDynamicSample" 
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/BuildDynamicSmearcepter" 
  DESTINATION
    ${CMAKE_INSTALL_PREFIX}/bin)

install(PROGRAMS
  "${PROJECT_SOURCE_DIR}/scripts/nuissamples" DESTINATION
  bin)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/data DESTINATION
                  ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY ${PROJECT_SOURCE_DIR}/parameters DESTINATION
                  ${CMAKE_INSTALL_PREFIX})

add_subdirectory(CLI)
add_subdirectory(validation)
