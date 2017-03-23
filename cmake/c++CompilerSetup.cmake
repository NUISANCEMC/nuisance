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

set(CXX_WARNINGS "-Wall ") #-Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_WARNINGS}")

if(DEFINED USE_EXP AND USE_EXP)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -fopenmp")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC -O3 -fopenmp")

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  set(CURRENT_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_DEBUG})
elseif(CMAKE_BUILD_TYPE MATCHES RELEASE)
  set(CURRENT_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_RELEASE})
else()
  cmessage(FATAL_ERROR "[ERROR]: Unknown CMAKE_BUILD_TYPE (\"${CMAKE_BUILD_TYPE}\"): Should be \"DEBUG\" or \"RELEASE\".")
endif()

set(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS} ")
set(CMAKE_DEPENDLIB_FLAGS "${ROOT_LD_FLAGS}")

if(NOT ${RWENGINE_LINKER_FLAGS} STREQUAL "")
  set(CMAKE_DEPENDLIB_FLAGS "${CMAKE_DEPENDLIB_FLAGS} ${RWENGINE_LINKER_FLAGS}")
endif()

if (DEFINED USE_MYPERFTOOLS AND USE_MYPERFTOOLS)
#  add_dependencies(gperftools libunwind)
  set(CMAKE_CXX_FLAGS "-fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free ${CMAKE_CXX_FLAGS}")
  set(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS} -ltcmalloc_and_profiler")
  cmessage(STATUS "Using google performance libraries")
endif()

if(DEFINED USE_EXP AND USE_EXP)
  set(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS}")
endif()

if (VERBOSE)
  cmessage (STATUS "C++ Compiler      : " ${CXX_COMPILER_NAME})
  cmessage (STATUS "    flags         : " ${CMAKE_CXX_FLAGS})
  cmessage (STATUS "    Release flags : " ${CMAKE_CXX_FLAGS_RELEASE})
  cmessage (STATUS "    Debug flags   : " ${CMAKE_CXX_FLAGS_DEBUG})
  cmessage (STATUS "    Link Flags    : " ${CMAKE_LINK_FLAGS})
  cmessage (STATUS "    Lib Flags     : " ${CMAKE_DEPENDLIB_FLAGS})
endif()
