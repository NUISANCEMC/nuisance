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
set(IMPLFILES
FNAL_CC1ppip_Evt_1DQ2_nu.cxx
FNAL_CC1ppip_XSec_1DEnu_nu.cxx
FNAL_CC1ppip_XSec_1DQ2_nu.cxx
FNAL_CCQE_Evt_1DQ2_nu.cxx
FNAL_CC1ppim_XSec_1DEnu_antinu.cxx
)

set(HEADERFILES
FNAL_CC1ppim_XSec_1DEnu_antinu.h
FNAL_CC1ppip_Evt_1DQ2_nu.h
FNAL_CC1ppip_XSec_1DEnu_nu.h
FNAL_CC1ppip_XSec_1DQ2_nu.h
FNAL_CCQE_Evt_1DQ2_nu.h
)

set(LIBNAME expFNAL)

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  add_library(${LIBNAME} STATIC ${IMPLFILES})
else(CMAKE_BUILD_TYPE MATCHES RELEASE)
  add_library(${LIBNAME} SHARED ${IMPLFILES})
endif()

include_directories(${RWENGINE_INCLUDE_DIRECTORIES})

include_directories(${CMAKE_SOURCE_DIR}/src/FitBase)
include_directories(${CMAKE_SOURCE_DIR}/src/InputHandler)
include_directories(${CMAKE_SOURCE_DIR}/src/Genie)
include_directories(${CMAKE_SOURCE_DIR}/src/Utils)
include_directories(${CMAKE_SOURCE_DIR}/src/Reweight)
include_directories(${CMAKE_SOURCE_DIR}/src/Splines)

set_target_properties(${LIBNAME} PROPERTIES VERSION
  "${ExtFit_VERSION_MAJOR}.${ExtFit_VERSION_MINOR}.${ExtFit_VERSION_REVISION}")
set_target_properties(${LIBNAME} PROPERTIES LINK_FLAGS ${ROOT_LD_FLAGS})
if(DEFINED PROJECTWIDE_EXTRA_DEPENDENCIES)
  add_dependencies(${LIBNAME} ${PROJECTWIDE_EXTRA_DEPENDENCIES})
endif()
install(TARGETS ${LIBNAME} DESTINATION lib)
#Can uncomment this to install the headers... but is it really neccessary?
#install(FILES ${HEADERFILES} DESTINATION include)

set(MODULETargets ${MODULETargets} ${LIBNAME} PARENT_SCOPE)
