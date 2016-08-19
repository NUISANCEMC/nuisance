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


if(NOT DEFINED ENV{NEUT_ROOT} OR $ENV{NEUT_ROOT} STREQUAL "")

  cmessage(FATAL_ERROR "Environment variable NEUT_ROOT is not defined. "
    "This must be set to point to a prebuilt NEUT instance.")

endif()

if(NOT DEFINED ENV{CERN} OR $ENV{CERN} STREQUAL "")

  cmessage(FATAL_ERROR "Environment variable CERN is not defined. "
    "This must be set to point to a prebuilt CERNLIB instance.")

endif()

if(NOT DEFINED ENV{CERN_LEVEL} OR $ENV{CERN_LEVEL} STREQUAL "")

  cmessage(FATAL_ERROR "Environment variable CERN_LEVEL is not defined. "
    "This must be set correctly for a prebuilt CERNLIB instance.")

endif()

set(NEUT_ROOT $ENV{NEUT_ROOT})
set(NEUT_LIB_DIR ${NEUT_ROOT}/lib/Linux_pc)
set(CERN $ENV{CERN})
set(CERN_LEVEL $ENV{CERN_LEVEL})
set(NEUT_CLASS ${NEUT_ROOT}/src/neutclass)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__NEUT_ENABLED__ ")

set(RWENGINE_INCLUDE_DIRECTORIES ${RWENGINE_INCLUDE_DIRECTORIES} ${NEUT_ROOT}/include ${NEUT_ROOT}/src/neutclass ${NEUT_ROOT}/src/reweight)

set(RWENGINE_LINKER_FLAGS "-L${NEUT_ROOT}/lib/Linux_pc  -lNReWeight -L${CERN}/${CERN_LEVEL}/lib -ljetset74 -lpdflib804 -lmathlib -lpacklib -lpawlib ${NEUT_CLASS}/neutctrl.so ${NEUT_CLASS}/neutfsivert.so ${NEUT_CLASS}/neutnucfsivert.so ${NEUT_CLASS}/neutrootTreeSingleton.so ${NEUT_CLASS}/neutvtx.so ${NEUT_CLASS}/neutfsipart.so  ${NEUT_CLASS}/neutnucfsistep.so ${NEUT_CLASS}/neutpart.so ${NEUT_CLASS}/neutvect.so -L${NEUT_ROOT}/lib/Linux_pc  -lneutcore -lnuccorrspl -lnuceff -lpartnuck -lskmcsvc -ltauola -L${NEUT_ROOT}/src/reweight -lNReWeight")
