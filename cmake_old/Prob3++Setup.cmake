# Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

if(USE_PROB3PP)

  LIST(APPEND EXTRA_CXX_FLAGS -D__PROB3PP_ENABLED__)

  ExternalProject_Add(prob3pp
    PREFIX "${CMAKE_BINARY_DIR}/Ext"
    URL "http://webhome.phy.duke.edu/~raw22/public/Prob3++/Prob3++.20121225.tar.gz"
    CONFIGURE_COMMAND ""
    BUILD_IN_SOURCE 1
    UPDATE_COMMAND ""
    BUILD_COMMAND CXXFLAGS=-fPIC CFLAGS=-fPIC make
    INSTALL_COMMAND ""
    )

  LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES ${CMAKE_BINARY_DIR}/Ext/src/prob3pp)

  LIST(APPEND EXTRA_LINK_DIRS ${CMAKE_BINARY_DIR}/Ext/src/prob3pp)

  LIST(APPEND EXTRA_LIBS ThreeProb_2.10)

  cmessage(STATUS "Using Prob3++ 2.10")

endif()
