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
set(IMPLFILES
ANL_CC1npip_Evt_1DcosmuStar_nu.cxx
ANL_CC1npip_Evt_1Dppi_nu.cxx
ANL_CC1npip_Evt_1DQ2_nu.cxx
ANL_CC1npip_XSec_1DEnu_nu.cxx
ANL_CC1pi0_Evt_1DcosmuStar_nu.cxx
ANL_CC1pi0_Evt_1DQ2_nu.cxx
ANL_CC1pi0_XSec_1DEnu_nu.cxx
ANL_CC1ppip_Evt_1DcosmuStar_nu.cxx
ANL_CC1ppip_Evt_1Dppi_nu.cxx
ANL_CC1ppip_Evt_1DQ2_nu.cxx
ANL_CC1ppip_Evt_1Dthpr_nu.cxx
ANL_CC1ppip_XSec_1DEnu_nu.cxx
ANL_CC1ppip_XSec_1DQ2_nu.cxx
ANL_CCQE_Evt_1DQ2_nu.cxx
ANL_CCQE_XSec_1DEnu_nu.cxx
ANL_NC1npip_Evt_1Dppi_nu.cxx
ANL_CC1ppip_Evt_1DcosthAdler_nu.cxx
ANL_CC1ppip_Evt_1Dphi_nu.cxx
)

set(HEADERFILES
ANL_CC1npip_Evt_1DcosmuStar_nu.h
ANL_CC1npip_Evt_1Dppi_nu.h
ANL_CC1npip_Evt_1DQ2_nu.h
ANL_CC1npip_XSec_1DEnu_nu.h
ANL_CC1pi0_Evt_1DcosmuStar_nu.h
ANL_CC1pi0_Evt_1DQ2_nu.h
ANL_CC1pi0_XSec_1DEnu_nu.h
ANL_CC1ppip_Evt_1DcosmuStar_nu.h
ANL_CC1ppip_Evt_1Dppi_nu.h
ANL_CC1ppip_Evt_1DQ2_nu.h
ANL_CC1ppip_Evt_1Dthpr_nu.h
ANL_CC1ppip_XSec_1DEnu_nu.h
ANL_CC1ppip_XSec_1DQ2_nu.h
ANL_CCQE_Evt_1DQ2_nu.h
ANL_CCQE_XSec_1DEnu_nu.h
ANL_NC1npip_Evt_1Dppi_nu.h
ANL_CC1ppip_Evt_1DcosthAdler_nu.h
ANL_CC1ppip_Evt_1Dphi_nu.h
)

set(LIBNAME expANL)

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  add_library(${LIBNAME} STATIC ${IMPLFILES})
else(CMAKE_BUILD_TYPE MATCHES RELEASE)
  add_library(${LIBNAME} SHARED ${IMPLFILES})
endif()

include_directories(${RWENGINE_INCLUDE_DIRECTORIES})

include_directories(${CMAKE_SOURCE_DIR}/src/FitBase)
include_directories(${CMAKE_SOURCE_DIR}/src/Utils)

set_target_properties(${LIBNAME} PROPERTIES VERSION
  "${ExtFit_VERSION_MAJOR}.${ExtFit_VERSION_MINOR}.${ExtFit_VERSION_REVISION}")
set_target_properties(${LIBNAME} PROPERTIES LINK_FLAGS ${ROOT_LD_FLAGS})

install(TARGETS ${LIBNAME} DESTINATION lib)
#Can uncomment this to install the headers... but is it really neccessary?
#install(FILES ${HEADERFILES} DESTINATION include)

set(MODULETargets ${MODULETargets} ${LIBNAME} PARENT_SCOPE)
