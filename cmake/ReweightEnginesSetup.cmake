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
*    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
################################################################################
set(RWENGINE_INCLUDE_DIRECTORIES "")

##################################  NEUT  ######################################
if(DEFINED USE_NEUT AND USE_NEUT)
  cmessage(FATAL_ERROR "Unfortunately NEUT is not enabled for CMake build at the moment. Fix it yourself or check back later!")
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
  cmessage(FATAL_ERROR "Unfortunately GENIE is not enabled for CMake build at the moment. Fix it yourself or check back later!")
  include(${CMAKE_SOURCE_DIR}/cmake/GENIESetup.cmake)
  cmessage(STATUS "Using GENIE Reweight engine.")
else()
  set(USE_GENIE 0)
endif()
##################################  NIWG  ######################################
if(DEFINED USE_NIWG AND USE_NIWG)
  cmessage(FATAL_ERROR "Unfortunately NIWG is not enabled for CMake build at the moment. Fix it yourself or check back later!")
  include(${CMAKE_SOURCE_DIR}/cmake/NIWGSetup.cmake)
  cmessage(STATUS "Using NIWG Reweight engine.")
else()
  set(USE_NIWG 0)
endif()

cmessage(STATUS "Reweight engine include directories: ${RWENGINE_INCLUDE_DIRECTORIES}")
