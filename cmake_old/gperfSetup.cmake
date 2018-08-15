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

if(USE_GPERFTOOLS)

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

  LIST(APPEND EXTRA_CXX_FLAGS
    -fno-builtin-malloc
    -fno-builtin-calloc
    -fno-builtin-realloc
    -fno-builtin-free)


  LIST(APPEND EXTRA_LINK_DIRS ${CMAKE_INSTALL_PREFIX}/lib)

  ##Want to prepend them
  LIST(REVERSE EXTRA_LIBS)
  LIST(APPEND EXTRA_LIBS tcmalloc_and_profiler)
  LIST(REVERSE EXTRA_LIBS)

  cmessage(STATUS "Using google performance libraries")
endif()
