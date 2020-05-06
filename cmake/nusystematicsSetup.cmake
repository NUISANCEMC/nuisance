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

if(NUSYST_ROOT STREQUAL "")
  cmessage(FATAL_ERROR "Variable NUSYST_ROOT is not defined. Either configure with -DNUSYST_ROOT or \"\$ export NUSYST_ROOT=/path/to/nusystematics\". This must be set to point to a prebuilt NuSystematics instance.")
endif()

if(SYSTTOOLS_ROOT STREQUAL "")
  cmessage(FATAL_ERROR "Variable SYSTTOOLS_ROOT is not defined. Either configure with -DSYSTTOOLS_ROOT or \"\$ export SYSTTOOLS_ROOT=/path/to/systematicstools\". This must be set to point to a prebuilt ART Systematics Tools instance.")
endif()

LIST(APPEND EXTRA_CXX_FLAGS -D__NUSYST_ENABLED__ -DNO_ART -std=c++1y -Wno-deprecated-declarations -Wno-deprecated)

LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES
  ${NUSYST_ROOT}/include
  ${SYSTTOOLS_ROOT}/include)

LIST(APPEND EXTRA_LINK_DIRS ${NUSYST_ROOT}/lib ${SYSTTOOLS_ROOT}/lib)
LIST(APPEND EXTRA_LIBS  nusystematics_systproviders
                        systematicstools_interface
                        systematicstools_interpreters
                        systematicstools_systproviders
                        systematicstools_utility)
