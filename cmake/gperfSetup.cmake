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

if(DEFINED USE_GPERFTOOLS AND USE_GPERFTOOLS)
  include(ExternalProject)

  ExternalProject_Add(libunwind
    PREFIX "${CMAKE_BINARY_DIR}/Ext"
    GIT_REPOSITORY "git://git.sv.gnu.org/libunwind.git"
    CONFIGURE_COMMAND ${CMAKE_BINARY_DIR}/Ext/src/libunwind/autogen.sh --prefix=${CMAKE_INSTALL_PREFIX}
    UPDATE_COMMAND ""
    BUILD_COMMAND make -j6
    INSTALL_COMMAND make install
    )

  ExternalProject_Add(gperftools
    PREFIX "${CMAKE_BINARY_DIR}/Ext"
    GIT_REPOSITORY "https://github.com/gperftools/gperftools.git"
    GIT_TAG "gperftools-2.5"
    CONFIGURE_COMMAND ./autogen.sh && ./configure --prefix=${CMAKE_INSTALL_PREFIX} CPPFLAGS=-I${CMAKE_INSTALL_PREFIX}/include LDFLAGS=-L${CMAKE_INSTALL_PREFIX}/lib
    BUILD_IN_SOURCE 1
    UPDATE_COMMAND ""
    BUILD_COMMAND make -j6
    INSTALL_COMMAND make install
    )

  add_dependencies(gperftools libunwind)

  set(CMAKE_CXX_FLAGS "-fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free ${CMAKE_CXX_FLAGS}")
  set(CMAKE_LINK_FLAGS "-L${CMAKE_INSTALL_PREFIX}/lib -ltcmalloc_and_profiler ${CMAKE_LINK_FLAGS}")

  cmessage(STATUS "Using google performance libraries")
endif()
