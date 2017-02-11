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

set(RWENGINE_INCLUDE_DIRECTORIES "")

##################################  NEUT  ######################################
if(DEFINED USE_NEUT AND USE_NEUT)
  include(${CMAKE_SOURCE_DIR}/cmake/NEUTSetup.cmake)
  cmessage(STATUS "Using NEUT Reweight engine.")
else()
  set(USE_NEUT 0)
endif()
#################################  NuWro  ######################################
if(DEFINED USE_NuWro AND USE_NuWro)
  include(${CMAKE_SOURCE_DIR}/cmake/NuWroSetup.cmake)
  cmessage(STATUS "Using NuWro Reweight engine.")
else()
  set(USE_NuWro 0)
endif()
##################################  GENIE  #####################################
if(DEFINED USE_GENIE AND USE_GENIE)
  include(${CMAKE_SOURCE_DIR}/cmake/GENIESetup.cmake)
  cmessage(STATUS "Using GENIE Reweight engine.")
else()
  set(USE_GENIE 0)
endif()
##################################  NIWG  ######################################
if(DEFINED USE_NIWG AND USE_NIWG)
  include(${CMAKE_SOURCE_DIR}/cmake/NIWGSetup.cmake)
  cmessage(STATUS "Using NIWG Reweight engine.")
else()
  set(USE_NIWG 0)
endif()
##################################  T2K   ######################################
if(DEFINED USE_T2K AND USE_T2K)
  include(${CMAKE_SOURCE_DIR}/cmake/T2KSetup.cmake)
  cmessage(STATUS "Using T2K Reweight engine.")
else()
  set(USE_T2K 0)
endif()

cmessage(STATUS "Reweight engine include directories: ${RWENGINE_INCLUDE_DIRECTORIES}")

if(DEFINED NEED_ROOTEVEGEN AND NEED_ROOTEVEGEN)
  cmessage(STATUS "Require ROOT eve generation libraries")
  set(ROOT_LIBS Eve;EG;TreePlayer;Geom;Ged;Gui;${ROOT_LIBS})
endif()
if(DEFINED NEED_ROOTPYTHIA6 AND NEED_ROOTPYTHIA6)
  cmessage(STATUS "Require ROOT Pythia6 libraries")
  set(ROOT_LIBS ${ROOT_LIBS};EGPythia6;Pythia6)
endif()
