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
MINERvA_CCQE_XSec_1DQ2_antinu.cxx
MINERvA_CCQE_XSec_1DQ2_joint.cxx
MINERvA_CCQE_XSec_1DQ2_nu.cxx

MINERvA_CC0pi_XSec_1DEe_nue.cxx
MINERvA_CC0pi_XSec_1DQ2_nue.cxx
MINERvA_CC0pi_XSec_1DQ2_nu_proton.cxx
MINERvA_CC0pi_XSec_1DThetae_nue.cxx

MINERvA_CC1pi0_XSec_1DEnu_antinu.cxx
MINERvA_CC1pi0_XSec_1DQ2_antinu.cxx
MINERvA_CC1pi0_XSec_1Dpmu_antinu.cxx
MINERvA_CC1pi0_XSec_1Dppi0_antinu.cxx
MINERvA_CC1pi0_XSec_1DTpi0_antinu.cxx
MINERvA_CC1pi0_XSec_1Dth_antinu.cxx
MINERvA_CC1pi0_XSec_1Dthmu_antinu.cxx

MINERvA_CC1pip_XSec_1DTpi_20deg_nu.cxx
MINERvA_CC1pip_XSec_1DTpi_nu.cxx
MINERvA_CC1pip_XSec_1Dth_20deg_nu.cxx
MINERvA_CC1pip_XSec_1Dth_nu.cxx

MINERvA_CCNpip_XSec_1DEnu_nu.cxx
MINERvA_CCNpip_XSec_1DQ2_nu.cxx
MINERvA_CCNpip_XSec_1DTpi_nu.cxx
MINERvA_CCNpip_XSec_1Dpmu_nu.cxx
MINERvA_CCNpip_XSec_1Dth_nu.cxx
MINERvA_CCNpip_XSec_1Dthmu_nu.cxx

MINERvA_CCinc_XSec_2DEavq3_nu.cxx
MINERvA_CCinc_XSec_1Dx_ratio.cxx
MINERvA_CCinc_XSec_1DEnu_ratio.cxx
MINERvA_CCinc_XSec_1Dx_nu.cxx
MINERvA_CCinc_XSec_1DEnu_nu.cxx

MINERvA_CCDIS_XSec_1Dx_ratio.cxx
MINERvA_CCDIS_XSec_1DEnu_ratio.cxx
MINERvA_CCDIS_XSec_1Dx_nu.cxx
MINERvA_CCDIS_XSec_1DEnu_nu.cxx

MINERvA_CC0pi_XSec_1DQ2_Tgt_nu.cxx
MINERvA_CC0pi_XSec_1DQ2_TgtRatio_nu.cxx
MINERvA_CC1pip_XSec_1D_2017Update.cxx

MINERvA_CCCOHPI_XSec_1DEnu_nu.cxx
MINERvA_CCCOHPI_XSec_1DEpi_nu.cxx
MINERvA_CCCOHPI_XSec_1Dth_nu.cxx
MINERvA_CCCOHPI_XSec_1DQ2_nu.cxx

MINERvA_CCCOHPI_XSec_1DEnu_antinu.cxx
MINERvA_CCCOHPI_XSec_1DEpi_antinu.cxx
MINERvA_CCCOHPI_XSec_1Dth_antinu.cxx
MINERvA_CCCOHPI_XSec_1DQ2_antinu.cxx

MINERvAUtils.cxx
MINERvA_SignalDef.cxx
)

set(HEADERFILES
MINERvA_CCQE_XSec_1DQ2_antinu.h
MINERvA_CCQE_XSec_1DQ2_joint.h
MINERvA_CCQE_XSec_1DQ2_nu.h

MINERvA_CC0pi_XSec_1DEe_nue.h
MINERvA_CC0pi_XSec_1DQ2_nue.h
MINERvA_CC0pi_XSec_1DQ2_nu_proton.h
MINERvA_CC0pi_XSec_1DThetae_nue.h

MINERvA_CC1pi0_XSec_1DEnu_antinu.h
MINERvA_CC1pi0_XSec_1DQ2_antinu.h
MINERvA_CC1pi0_XSec_1Dpmu_antinu.h
MINERvA_CC1pi0_XSec_1Dppi0_antinu.h
MINERvA_CC1pi0_XSec_1DTpi0_antinu.h
MINERvA_CC1pi0_XSec_1Dth_antinu.h
MINERvA_CC1pi0_XSec_1Dthmu_antinu.h

MINERvA_CC1pip_XSec_1DTpi_20deg_nu.h
MINERvA_CC1pip_XSec_1DTpi_nu.h
MINERvA_CC1pip_XSec_1Dth_20deg_nu.h
MINERvA_CC1pip_XSec_1Dth_nu.h

MINERvA_CCNpip_XSec_1DEnu_nu.h
MINERvA_CCNpip_XSec_1DQ2_nu.h
MINERvA_CCNpip_XSec_1DTpi_nu.h
MINERvA_CCNpip_XSec_1Dpmu_nu.h
MINERvA_CCNpip_XSec_1Dth_nu.h
MINERvA_CCNpip_XSec_1Dthmu_nu.h

MINERvA_CCinc_XSec_2DEavq3_nu.h
MINERvA_CCinc_XSec_1Dx_ratio.h
MINERvA_CCinc_XSec_1DEnu_ratio.h
MINERvA_CCinc_XSec_1Dx_nu.h
MINERvA_CCinc_XSec_1DEnu_nu.h

MINERvA_CCDIS_XSec_1Dx_ratio.h
MINERvA_CCDIS_XSec_1DEnu_ratio.h
MINERvA_CCDIS_XSec_1Dx_nu.h
MINERvA_CCDIS_XSec_1DEnu_nu.h

MINERvA_CC0pi_XSec_1DQ2_Tgt_nu.h
MINERvA_CC0pi_XSec_1DQ2_TgtRatio_nu.h
MINERvA_CC1pip_XSec_1D_2017Update.h

MINERvA_CCCOHPI_XSec_1DEnu_nu.h
MINERvA_CCCOHPI_XSec_1DEpi_nu.h
MINERvA_CCCOHPI_XSec_1Dth_nu.h
MINERvA_CCCOHPI_XSec_1DEnu_antinu.h
MINERvA_CCCOHPI_XSec_1DEpi_antinu.h
MINERvA_CCCOHPI_XSec_1Dth_antinu.h

MINERvAUtils.h
MINERvA_SignalDef.h
MINERvAVariableBoxes.h
)

set(LIBNAME expMINERvA)

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  add_library(${LIBNAME} STATIC ${IMPLFILES})
else(CMAKE_BUILD_TYPE MATCHES RELEASE)
  add_library(${LIBNAME} SHARED ${IMPLFILES})
endif()

include_directories(${MINIMUM_INCLUDE_DIRECTORIES})

set_target_properties(${LIBNAME} PROPERTIES VERSION
  "${NUISANCE_VERSION_MAJOR}.${NUISANCE_VERSION_MINOR}.${NUISANCE_VERSION_REVISION}")
#set_target_properties(${LIBNAME} PROPERTIES LINK_FLAGS ${ROOT_LD_FLAGS})
if(DEFINED PROJECTWIDE_EXTRA_DEPENDENCIES)
  add_dependencies(${LIBNAME} ${PROJECTWIDE_EXTRA_DEPENDENCIES})
endif()

install(TARGETS ${LIBNAME} DESTINATION lib)
#Can uncomment this to install the headers... but is it really neccessary?
#install(FILES ${HEADERFILES} DESTINATION include)

set(MODULETargets ${MODULETargets} ${LIBNAME} PARENT_SCOPE)
