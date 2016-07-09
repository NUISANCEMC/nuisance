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
*    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
################################################################################
set(IMPLFILES
MINERvA_CC0pi_XSec_1DEe_nue.cxx
MINERvA_CC1pip_XSec_1DTpi_nu.cxx
MINERvA_CC0pi_XSec_1DFill_nue.cxx
MINERvA_CCinc_XSec_2DEavq3_nu.cxx
MINERvA_CC0pi_XSec_1DQ2_nue.cxx
MINERvA_CCNpip_XSec_1Dth_20deg_nu.cxx
MINERvA_CC0pi_XSec_1DQ2_nu_proton.cxx
MINERvA_CCNpip_XSec_1Dth_nu.cxx
MINERvA_CC0pi_XSec_1DThetae_nue.cxx
MINERvA_CCNpip_XSec_1DTpi_20deg_nu.cxx
MINERvA_CC1pi0_XSec_1Dppi0_antinu.cxx
MINERvA_CCNpip_XSec_1DTpi_nu.cxx
MINERvA_CC1pi0_XSec_1Dth_antinu.cxx
MINERvA_CCQE_XSec_1DQ2_antinu.cxx
MINERvA_CC1pip_XSec_1Dth_20deg_nu.cxx
MINERvA_CCQE_XSec_1DQ2_joint.cxx
MINERvA_CC1pip_XSec_1Dth_nu.cxx
MINERvA_CCQE_XSec_1DQ2_nu.cxx
MINERvA_CC1pip_XSec_1DTpi_20deg_nu.cxx
)

set(HEADERFILES
MINERvA_CC0pi_XSec_1DEe_nue.h
MINERvA_CC1pip_XSec_1DTpi_nu.h
MINERvA_CC0pi_XSec_1DFill_nue.h
MINERvA_CCinc_XSec_2DEavq3_nu.h
MINERvA_CC0pi_XSec_1DQ2_nue.h
MINERvA_CCNpip_XSec_1Dth_20deg_nu.h
MINERvA_CC0pi_XSec_1DQ2_nu_proton.h
MINERvA_CCNpip_XSec_1Dth_nu.h
MINERvA_CC0pi_XSec_1DThetae_nue.h
MINERvA_CCNpip_XSec_1DTpi_20deg_nu.h
MINERvA_CC1pi0_XSec_1Dppi0_antinu.h
MINERvA_CCNpip_XSec_1DTpi_nu.h
MINERvA_CC1pi0_XSec_1Dth_antinu.h
MINERvA_CCQE_XSec_1DQ2_antinu.h
MINERvA_CC1pip_XSec_1Dth_20deg_nu.h
MINERvA_CCQE_XSec_1DQ2_joint.h
MINERvA_CC1pip_XSec_1Dth_nu.h
MINERvA_CCQE_XSec_1DQ2_nu.h
MINERvA_CC1pip_XSec_1DTpi_20deg_nu.h
)

set(LIBNAME expMINERvA)

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
