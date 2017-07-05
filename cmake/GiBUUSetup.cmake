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

if(USE_GiBUU)
  LIST(APPEND EXTRA_CXX_FLAGS -D__GiBUU_ENABLED__)

  if(BUILD_GiBUU)
    if (NO_EXTERNAL_UPDATE)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                     PROPERTY EP_UPDATE_DISCONNECTED 1)
        cmessage(STATUS "Will not attempt to update third party GiBUU tools for each build.")
    endif()

    ExternalProject_Add(GiBUUTools
    PREFIX "${PROJECT_BINARY_DIR}/GiBUUTools"
    GIT_REPOSITORY https://github.com/luketpickering/GiBUU-t2k-dev.git
    CMAKE_ARGS
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DUSE_GiBUU=1
    -DFORCECPP03=1
    -DNO_EXTERNAL_UPDATE=${NO_EXTERNAL_UPDATE})

    cmessage(STATUS "Building GiBUU and GiBUUTools")
    SET(BUILD_GiBUU TRUE CACHE BOOL "Whether to build supporting GiBUU event tools along with a patched version of GiBUU. <FALSE>" FORCE)
  endif()

  SET(USE_GiBUU TRUE CACHE BOOL "Whether to enable GiBUU event support. <TRUE>" FORCE)
endif()
