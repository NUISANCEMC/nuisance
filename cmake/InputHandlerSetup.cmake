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

#################################  NuWro  ######################################
if(USE_NUWRO)
  include(${CMAKE_SOURCE_DIR}/cmake/NuWroSetup.cmake)
  cmessage(STATUS "Using NuWro.")
  set(USE_NUWRO TRUE CACHE BOOL "Whether to enable NuWro support. <FALSE>" FORCE)
endif()

#################################  NEUT  #######################################
if(USE_NEUT)
  include(${CMAKE_SOURCE_DIR}/cmake/NEUTSetup.cmake)
  cmessage(STATUS "Using NEUT.")
  set(USE_NEUT TRUE CACHE BOOL "Whether to enable NEUT support. <FALSE>" FORCE)
endif()

################################  GENIE  #######################################
if(USE_GENIE)
  include(${CMAKE_SOURCE_DIR}/cmake/GENIESetup.cmake)
  cmessage(STATUS "Using GENIE.")
  set(USE_GENIE TRUE CACHE BOOL "Whether to enable GENIE support. <FALSE>" FORCE)
endif()


if(NEED_ROOTEVEGEN)
  cmessage(STATUS "Require ROOT eve generation libraries")
  LIST(REVERSE ROOT_LIBS)
  LIST(APPEND ROOT_LIBS
    Gui
    Ged
    Geom
    TreePlayer
    EG
    Eve)
  LIST(REVERSE ROOT_LIBS)
endif()

if(NEED_ROOTPYTHIA6)
  cmessage(STATUS "Require ROOT Pythia6 libraries")
  LIST(APPEND ROOT_LIBS
    EGPythia6
    Pythia6)
endif()


LIST(APPEND EXTRA_LIBS ${ROOT_LIBS})
