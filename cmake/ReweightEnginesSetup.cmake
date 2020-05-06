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

if(NOT USE_REWEIGHT)
  LIST(APPEND EXTRA_CXX_FLAGS -D__NO_REWEIGHT__)
endif()

##################################  T2K   ######################################
if(USE_T2K)
  include(${CMAKE_SOURCE_DIR}/cmake/T2KSetup.cmake)
  cmessage(STATUS "Using T2K Reweight engine.")
  set(USE_T2K TRUE CACHE BOOL "Whether to enable T2KReWeight support. Requires external libraries. <FALSE>" FORCE)
endif()
##################################  NIWG  ######################################
if(USE_NIWG)
  include(${CMAKE_SOURCE_DIR}/cmake/NIWGSetup.cmake)
  cmessage(STATUS "Using NIWG Reweight engine.")
  set(USE_NIWG TRUE CACHE BOOL "Whether to enable (T2K) NIWG ReWeight support. Requires external libraries. <FALSE>" FORCE)
endif()
##################################  MINERvA   ######################################
if(USE_MINERvA_RW)
  include(${CMAKE_SOURCE_DIR}/cmake/MINERvASetup.cmake)
  cmessage(STATUS "Using MINERvA Reweight engine.")
  set(USE_MINERvA_RW TRUE CACHE BOOL "Whether to enable MINERvA ReWeight support. <FALSE>" FORCE)
endif()
##################################  NEUT  ######################################
if(USE_NEUT)
  include(${CMAKE_SOURCE_DIR}/cmake/NEUTSetup.cmake)
  cmessage(STATUS "Using NEUT Reweight engine.")
  set(USE_NEUT TRUE CACHE BOOL "Whether to enable NEUT (reweight) support. Requires external libraries. <FALSE>" FORCE)
endif()
#################################  NuWro  ######################################
if(USE_NuWro)
  include(${CMAKE_SOURCE_DIR}/cmake/NuWroSetup.cmake)
  cmessage(STATUS "Using NuWro Reweight engine.")
  set(USE_NuWro TRUE CACHE BOOL "Whether to enable NuWro support. <FALSE>" FORCE)
endif()
##################################  GENIE  #####################################
if(USE_GENIE)
  include(${CMAKE_SOURCE_DIR}/cmake/GENIESetup.cmake)
  cmessage(STATUS "Using GENIE.")
  set(USE_GENIE TRUE CACHE BOOL "Whether to enable GENIE support. Requires external libraries. <FALSE>" FORCE)
endif()

################################################################################
################################  NOvARwgt  ####################################
if(USE_NOvARwgt)
  include(${CMAKE_SOURCE_DIR}/cmake/NOvARwgtSetup.cmake)
  cmessage(STATUS "Using NOvARwgt Reweight engine.")
  set(USE_NOvARwgt TRUE CACHE BOOL "Whether to enable NOvARwgt (reweight) support. Requires external libraries. <FALSE>" FORCE)
endif()

################################################################################
if(USE_NUSYST)
  include(${CMAKE_SOURCE_DIR}/cmake/nusystematicsSetup.cmake)
  cmessage(STATUS "Using NOvARwgt Reweight engine.")
  set(USE_NUSYST TRUE CACHE BOOL "Whether to enable DUNERwt (reweight) support. Requires external libraries. <FALSE>" FORCE)
endif()
################################  Prob3++   ####################################
include(${CMAKE_SOURCE_DIR}/cmake/Prob3++Setup.cmake)
################################################################################

cmessage(STATUS "Reweight engine include directories: ${RWENGINE_INCLUDE_DIRECTORIES}")

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
    EGPythia6)
endif()


LIST(APPEND EXTRA_LIBS ${ROOT_LIBS})
