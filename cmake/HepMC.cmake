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

LIST(APPEND EXTRA_CXX_FLAGS -D__HEPMC_ENABLED__)

if(HEPMC STREQUAL "")
  if (NO_EXTERNAL_UPDATE)
      set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                   PROPERTY EP_UPDATE_DISCONNECTED 1)
      cmessage(STATUS "Will not attempt to update third party HepMC tools for each build.")
  endif()

  ExternalProject_Add(HepMC
  PREFIX "${PROJECT_BINARY_DIR}/HepMC"
  GIT_REPOSITORY https://gitlab.cern.ch/hepmc/HepMC.git
  GIT_TAG HEPMC_02_06_09
  CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
  -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -Dmomentum=${HEPMC_MOMUNIT}
  -Dlength=${HEPMC_LENUNIT})


  #Have to re-include the doc string here as the whole cache entry will be
  #overwritten when this happens
  set(HEPMC "${CMAKE_INSTALL_PREFIX}" CACHE PATH
    "Path to HEPMC source tree root directory. Overrides environment variable \$HEPMC <>"
    FORCE)

  set(HEPMC_USED_EP TRUE CACHE INTERNAL "Whether the we built HepMC or not. <FALSE>"
    FORCE)

  cmessage(STATUS "Building HepMC")
endif()

LIST(APPEND EXTRA_CXX_FLAGS
  -I${HEPMC}/include/)

LIST(APPEND EXTRA_LINK_DIRS ${HEPMC}/lib/)
LIST(APPEND EXTRA_LIBS HepMC HepMCfio)
