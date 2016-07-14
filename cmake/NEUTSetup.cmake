# Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

################################################################################
#    This file is part of NuFiX.
#
#    NuFiX is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    NuFiX is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
################################################################################

if(NOT DEFINED ENV{NEUT_ROOT})

  cmessage(FATAL_ERROR "Environment variable NEUT_ROOT is not defined. "
    "This must be set to point to a prebuilt NEUT instance.")

endif()

set(NEUT_ROOT $ENV{NEUT_ROOT})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__NEUT_ENABLED__ ")

set(RWENGINE_INCLUDE_DIRECTORIES ${RWENGINE_INCLUDE_DIRECTORIES} ${NEUT_ROOT}/include ${NEUT_ROOT}/src/neutclass ${NEUT_ROOT}/src/reweight)

set(RWENGINE_LINKER_FLAGS "-L${NEUT_ROOT}/lib/${CMAKE_SYSTEM_NAME}")

