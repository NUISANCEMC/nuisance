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

if ( NOT DEFINED ENV{ROOTSYS} )
  cmessage (FATAL_ERROR "$ROOTSYS is not defined, please set up root first.")
else()
  cmessage(STATUS "Using ROOT installed at $ENV{ROOTSYS}")
  set(CMAKE_ROOTSYS $ENV{ROOTSYS})
endif()

execute_process (COMMAND root-config
  --cflags OUTPUT_VARIABLE ROOT_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process (COMMAND root-config
  --libdir OUTPUT_VARIABLE ROOT_LIBDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process (COMMAND root-config
  --version OUTPUT_VARIABLE ROOT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process (COMMAND root-config
  --features OUTPUT_VARIABLE ROOT_FEATURES OUTPUT_STRIP_TRAILING_WHITESPACE)

set(ROOT_LD_FLAGS "-L${ROOT_LIBDIR}")

set(ROOT_LIBS Core;Cint;RIO;Net;Hist;Graf;Graf3d;Gpad;Tree;Rint;Postscript;Matrix;Physics;MathCore;Thread;EG;Geom;GenVector)

if(USE_GENIE)
  cmessage(STATUS "GENIE requires eve generation libraries")
  set(ROOT_LIBS Eve;EG;TreePlayer;Geom;Ged;Gui;${ROOT_LIBS})
endif()

# Check if we have Minuit2 enabled
if(NOT DEFINED USE_MINIMIZER)
  if("${ROOT_FEATURES}" MATCHES "minuit2")
    cmessage(STATUS "ROOT built with MINUIT2 support")
    set(USE_MINIMIZER 1)
  else()
    cmessage(STATUS "ROOT built without MINUIT2 support, minimizer functionality will be disabled.")
    set(USE_MINIMIZER 0)
  endif()
endif()

# Check ROOT version is 5.34/34 or 5.34/36
string(REGEX MATCH "5.34/([0-9]+)" ROOTVERSMATCH ${ROOT_VERSION})
if( NOT ROOTVERSMATCH OR  ${CMAKE_MATCH_1} LESS "19")
  cmessage(WARNING "ROOT Version: ${ROOT_VERSION} has out of date minimizer interface. Disabling minimizer, please update to 5.34/19 or greater to enable minimization features.")
  set(USE_MINIMIZER 0)
endif()


if("${ROOT_VERSION}" MATCHES "5.34/34" OR "${ROOT_VERSION}" MATCHES "5.34/36" OR "${ROOT_VERSION}" MATCHES "5.34/32")
  cmessage(STATUS "Found valid ROOT version ${ROOT_VERSION}")
else()
  cmessage(STATUS "You've supplied an invalid ROOT version! NUISANCE only support 5.34/34 and 5.34/36: turning minimizer OFF")
  set(USE_MINIMIZER 0)
endif()

if("${ROOT_FEATURES}" MATCHES "opengl")
  cmessage(STATUS "ROOT built with OpenGL support")
  set(ROOT_LIBS ${ROOT_LIBS};RGL)
endif()

if(DEFINED NEED_ROOTPYTHIA6 AND NEED_ROOTPYTHIA6)
  set(ROOT_LIBS ${ROOT_LIBS};EGPythia6;Pythia6)
endif()

cmessage ( STATUS "[ROOT]: root-config --version: " ${ROOT_VERSION})
cmessage ( STATUS "[ROOT]: root-config --cflags: " ${ROOT_CXX_FLAGS} )
cmessage ( STATUS "[ROOT]: root-config --libs: " ${ROOT_LD_FLAGS} )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOT_CXX_FLAGS}")
