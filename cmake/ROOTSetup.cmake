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
if ( NOT DEFINED ENV{ROOTSYS} )
  cmessage (FATAL_ERROR "$ROOTSYS is not defined, please set up root first.")
else()
  cmessage(STATUS "Using ROOT installed at $ENV{ROOTSYS}")
  set(CMAKE_ROOTSYS $ENV{ROOTSYS})
endif()

#If you want to try an use the terminally buggy ROOT CMake scripts
if (DEFINED USEROOTCMAKE AND USEROOTCMAKE)
  include ($ENV{ROOTSYS}/etc/cmake/FindROOT.cmake)
  if ( NOT ROOT_FOUND )
    cmessage (FATAL_ERROR "[ROOT]: FindROOT.cmake could not be found, or did not execute as expected is ROOT installed in $ROOTSYS = $ENV{ROOTSYS}?")
  endif()
else()
  # cmessage(STATUS "Including local GENERATE_ROOT_DICTIONARY implementation.")
  # include(${CMAKE_SOURCE_DIR}/cmake/GenROOTDictionary.cmake)
endif()

execute_process (COMMAND root-config --cflags OUTPUT_VARIABLE ROOT_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process (COMMAND root-config --libs OUTPUT_VARIABLE ROOT_LD_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process (COMMAND root-config --version OUTPUT_VARIABLE ROOT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
cmessage ( STATUS "[ROOT]: root-config --version: " ${ROOT_VERSION})
cmessage ( STATUS "[ROOT]: root-config --cflags: " ${ROOT_CXX_FLAGS} )
cmessage ( STATUS "[ROOT]: root-config --libs: " ${ROOT_LD_FLAGS} )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOT_CXX_FLAGS}")
