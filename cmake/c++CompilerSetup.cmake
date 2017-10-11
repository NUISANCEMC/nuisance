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

if(USE_OMP)
  LIST(APPEND EXTRA_CXX_FLAGS -fopenmp)
endif()

if(USE_DYNSAMPLES)
  LIST(APPEND EXTRA_LIBS dl)
  LIST(APPEND EXTRA_CXX_FLAGS -D__USE_DYNSAMPLES__)
endif()

set(CXX_WARNINGS -Wall )

cmessage(DEBUG "EXTRA_CXX_FLAGS: ${EXTRA_CXX_FLAGS}")
string(REPLACE ";" " " STR_EXTRA_CXX_FLAGS "${EXTRA_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STR_EXTRA_CXX_FLAGS} ${CXX_WARNINGS}")
  set(CMAKE_Fortran_FLAGS_RELEASE "-fPIC")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

if(USE_DYNSAMPLES)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fPIC")
  set(CMAKE_Fortran_FLAGS_DEBUG "-fPIC")
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC -O3")

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  set(CURRENT_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_DEBUG})
elseif(CMAKE_BUILD_TYPE MATCHES RELEASE)
  set(CURRENT_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_RELEASE})
else()
  cmessage(FATAL_ERROR "[ERROR]: Unknown CMAKE_BUILD_TYPE (\"${CMAKE_BUILD_TYPE}\"): Should be \"DEBUG\" or \"RELEASE\".")
endif()


SET(STR_EXTRA_LINK_DIRS)
if(NOT EXTRA_LINK_DIRS STREQUAL "")
  string(REPLACE ";" " -L" STR_EXTRA_LINK_DIRS "-L${EXTRA_LINK_DIRS}")
endif()
SET(STR_EXTRA_LIBS)
if(NOT EXTRA_LIBS STREQUAL "")
  string(REPLACE ";" " -l" STR_EXTRA_LIBS "-l${EXTRA_LIBS}")
endif()
SET(STR_EXTRA_SHAREDOBJS)
if(NOT EXTRA_SHAREDOBJS STREQUAL "")
  string(REPLACE ";" " " STR_EXTRA_SHAREDOBJS "${EXTRA_SHAREDOBJS}")
endif()

SET(STR_EXTRA_LINK_FLAGS)
if(NOT EXTRA_LINK_FLAGS STREQUAL "")
  string(REPLACE ";" " " STR_EXTRA_LINK_FLAGS "${EXTRA_LINK_FLAGS}")
endif()

cmessage(DEBUG "EXTRA_LINK_DIRS: ${STR_EXTRA_LINK_DIRS}")
cmessage(DEBUG "EXTRA_LIBS: ${STR_EXTRA_LIBS}")
cmessage(DEBUG "EXTRA_SHAREDOBJS: ${STR_EXTRA_SHAREDOBJS}")
cmessage(DEBUG "EXTRA_LINK_FLAGS: ${STR_EXTRA_LINK_FLAGS}")

if(NOT STR_EXTRA_LINK_DIRS STREQUAL "" AND NOT STR_EXTRA_LIBS STREQUAL "")
  SET(CMAKE_DEPENDLIB_FLAGS "${STR_EXTRA_LINK_DIRS} ${STR_EXTRA_LIBS}")
endif()

if(NOT EXTRA_SHAREDOBJS STREQUAL "")
  if(NOT STR_EXTRA_LINK_FLAGS STREQUAL "")
    SET(STR_EXTRA_LINK_FLAGS "${STR_EXTRA_SHAREDOBJS} ${STR_EXTRA_LINK_FLAGS}")
  else()
    SET(STR_EXTRA_LINK_FLAGS "${STR_EXTRA_SHAREDOBJS}")
  endif()
endif()

if(NOT EXTRA_LINK_FLAGS STREQUAL "")
  if(NOT CMAKE_LINK_FLAGS STREQUAL "")
    SET(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS} ${STR_EXTRA_LINK_FLAGS}")
  else()
    SET(CMAKE_LINK_FLAGS "${STR_EXTRA_LINK_FLAGS}")
  endif()
endif()

if(USE_OMP)
  cmessage(FATAL_ERROR "No OMP features currently enabled so this is a FATAL_ERROR to let you know that you don't gain anything with this declaration.")
endif()


if (VERBOSE)
  cmessage (STATUS "C++ Compiler      : ${CXX_COMPILER_NAME}")
  cmessage (STATUS "    flags         : ${CMAKE_CXX_FLAGS}")
  cmessage (STATUS "    Release flags : ${CMAKE_CXX_FLAGS_RELEASE}")
  cmessage (STATUS "    Debug flags   : ${CMAKE_CXX_FLAGS_DEBUG}")
  cmessage (STATUS "    Link Flags    : ${CMAKE_LINK_FLAGS}")
  cmessage (STATUS "    Lib Flags     : ${CMAKE_DEPENDLIB_FLAGS}")
endif()
