# Copyright 2018 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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


if(NUWRO STREQUAL "")
  cmessage(FATAL_ERROR "Variable NUWRO is not defined. "
    "This must be set to point to a prebuilt NuWro instance.")
endif()

LIST(APPEND EXTRA_CXX_FLAGS -D__NUWRO_ENABLED__)

LIST(APPEND EXTRA_CXX_FLAGS -I${NUWRO}/src)

if(NOT EXISTS ${NUWRO}/bin/event1.so)
  if(EXISTS ${NUWRO}/build/${CMAKE_SYSTEM_NAME}/lib)

    if(NUWRO_INC STREQUAL "")
      cmessage(FATAL_ERROR "Variable NUWRO_INC is not defined. "
        "This must be set to point to an installed NuWro instance.")
    endif()

    LIST(APPEND EXTRA_LINK_DIRS ${NUWRO}/build/${CMAKE_SYSTEM_NAME}/lib)
    LIST(APPEND EXTRA_LIBS event)
  else()
    cmessage(FATAL_ERROR "Expected to find the NuWro event library in: ${NUWRO}/bin/event1.so, or if using NuWro with reweight support: ${NUWRO}/build/${CMAKE_SYSTEM_NAME}/lib/libevent.a. Is NuWro built?")
  endif()
else()
  LIST(APPEND EXTRA_SHAREDOBJS ${NUWRO}/bin/event1.so)
endif()

set(NEED_PYTHIA6 TRUE)
set(NEED_ROOTPYTHIA6 TRUE)
