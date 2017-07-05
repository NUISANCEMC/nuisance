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

if(T2KREWEIGHT_ROOT STREQUAL "")
  cmessage(FATAL_ERROR "Variable T2KREWEIGHT_ROOT is not defined. Either configure with -DT2KREWEIGHT_ROOT or \"\$ export T2KREWEIGHT=/path/to/T2KReWeight\". This must be set to point to a prebuilt T2KReWeight instance.")
endif()

LIST(APPEND EXTRA_CXX_FLAGS -D__T2KREW_ENABLED__ )

LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES ${T2KREWEIGHT_ROOT}/src/)

LIST(APPEND EXTRA_LINK_DIRS ${T2KREWEIGHT_ROOT}/lib)
LIST(APPEND EXTRA_LIBS T2KReWeight)


