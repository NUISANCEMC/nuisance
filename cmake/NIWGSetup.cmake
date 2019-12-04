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

if(NIWG_ROOT STREQUAL "")
  cmessage(FATAL_ERROR "Variable NIWG_ROOT is not defined. Either configure with -DNIWG_ROOT or \"\$ export NIWG=/path/to/NIWGReWeight\". This must be set to point to a prebuilt NIWGReWeight instance.")
endif()

LIST(APPEND EXTRA_CXX_FLAGS -D__NIWG_ENABLED__)

# Look for CCQE low Q2 suppression in NIWG
find_file(NIWGRWLOWQE NIWGReWeightEffectiveQELowQ2Suppression.h
  PATHS ${NIWG_ROOT})

# Look for 2p2h energy dependent parameter in NIWG
find_file(NIWG2P2HENU NIWGReWeight2p2hEdep.h
  PATH ${NIWG_ROOT})

if( NOT "${NIWGRWLOWQE} " STREQUAL "NIWGRWLOWQE-NOTFOUND ")
  cmessage(STATUS "Found ${NIWGRWLOWQE}")
  LIST(APPEND EXTRA_CXX_FLAGS -DHAVE_NIWGRW_LOWQ2)
endif()

if( NOT "${NIWG2P2HENU} " STREQUAL "NIWG2P2HENU-NOTFOUND ")
  cmessage(STATUS "Found ${NIWG2P2HENU}")
  LIST(APPEND EXTRA_CXX_FLAGS -DHAVE_NIWGRW_2P2HENU)
endif()

LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES ${NIWG_ROOT})

LIST(APPEND EXTRA_LINK_DIRS ${NIWG_ROOT})
LIST(APPEND EXTRA_LIBS NIWGReWeight)
