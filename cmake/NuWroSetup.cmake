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

LIST(APPEND NUWRO_CXX_FLAGS -DUSE_NUWRO -Wno-sign-compare -Wno-unused-variable -Wno-reorder)

LIST(APPEND NUWRO_INCLUDE_DIRS ${NUWRO}/src)

add_library(NuWro_event1 SHARED IMPORTED)
set_property(TARGET NuWro_event1 PROPERTY IMPORTED_LOCATION ${NUWRO}/bin/event1.so)
LIST(APPEND NUWRO_IMPORTED_TARGETS NuWro_event1)

set(NEED_PYTHIA6 TRUE)
set(NEED_ROOTPYTHIA6 TRUE)

cmessage(STATUS "NuWro")
cmessage(STATUS "     Flags     : ${NUWRO_CXX_FLAGS}")
cmessage(STATUS "     Includes  : ${NUWRO_INCLUDE_DIRS}")
cmessage(STATUS "     SOs       : ${NUWRO_IMPORTED_TARGETS}")

INSTALL(FILES ${NUWRO}/src/event1dict_rdict.pcm DESTINATION lib)
