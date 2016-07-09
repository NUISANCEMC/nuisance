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
ANL_CC1npip_1DEnu_nu.cxx
ANL_CC1npip_1Dppi_nu.cxx
ANL_CC1npip_1DQ2Ev_nu.cxx
ANL_CC1pi0_1DEnu_nu.cxx
ANL_CC1pi0_1DQ2Ev_nu.cxx
ANL_CC1ppip_1DEnu_nu.cxx
ANL_CC1ppip_1Dppi_nu.cxx
ANL_CC1ppip_1DQ2Ev_nu.cxx
ANL_CC1ppip_1DQ2_nu.cxx
ANL_CC1ppip_1Dthpr_nu.cxx
ANL_NC1npip_1Dppi_nu.cxx
BEBC_CC1npim_1DEnu_antinu.cxx
BEBC_CC1npim_1DQ2_antinu.cxx
BEBC_CC1npip_1DEnu_nu.cxx
BEBC_CC1npip_1DQ2_nu.cxx
BEBC_CC1pi0_1DEnu_nu.cxx
BEBC_CC1pi0_1DQ2_nu.cxx
BEBC_CC1ppim_1DEnu_antinu.cxx
BEBC_CC1ppim_1DQ2_antinu.cxx
BEBC_CC1ppip_1DEnu_nu.cxx
BEBC_CC1ppip_1DQ2_nu.cxx
BNL_CC1npip_1DEnu_nu.cxx
BNL_CC1npip_1DQ2Ev_nu.cxx
BNL_CC1pi0_1DEnu_nu.cxx
BNL_CC1pi0_1DQ2Ev_nu.cxx
BNL_CC1ppip_1DEnu_nu.cxx
BNL_CC1ppip_1DQ2Ev_nu.cxx
BNL_NC1ppim_1DEnu_nu.cxx
BNL_NC1ppim_1DQ2_nu.cxx
FitUtils.cxx
FNAL_CC1ppim_1DEnu_anu.cxx
FNAL_CC1ppim_1DQ2_anu.cxx
FNAL_CC1ppip_1DEnu_nu.cxx
FNAL_CC1ppip_1DQ2Ev_nu.cxx
FNAL_CC1ppip_1DQ2_nu.cxx
GGM_CC1ppip_1DEnu_nu.cxx
GGM_CC1ppip_1DQ2Ev_nu.cxx
K2K_CCpip_CCQE_1DEnu_nu.cxx
K2K_NCpi0_1Dppi0_nu.cxx
MINERvA_1DQ2_antinu_20deg.cxx
MINERvA_1DQ2_antinu.cxx
MINERvA_1DQ2_nu_20deg.cxx
MINERvA_1DQ2_nu.cxx
MINERvA_CCcoh_1DEnu_antinu.cxx
MINERvA_CCcoh_1DEnu_nu.cxx
MINERvA_CCcoh_1DEpi_antinu.cxx
MINERvA_CCcoh_1DEpi_nu.cxx
MINERvA_CCcoh_1Dth_antinu.cxx
MINERvA_CCcoh_1Dth_nu.cxx
MINERvA_CCpi0_1Dppi0_antinu.cxx
MINERvA_CCpi0_1Dth_antinu.cxx
MINERvA_CCpip_1Dth_20deg_nu.cxx
MINERvA_CCpip_1Dth_nu.cxx
MINERvA_CCpip_1DTpi_20deg_nu.cxx
MINERvA_CCpip_1DTpi_nu.cxx
MiniBooNE_1DEnuLike_nu.cxx
MiniBooNE_1DEnu_nu.cxx
MiniBooNE_1DQ2_antinu.cxx
MiniBooNE_1DQ2_nu.cxx
MiniBooNE_2DTcos_antinu_CCQELike.cxx
MiniBooNE_2DTcos_antinu.cxx
MiniBooNE_2DTcos_nu_CCQELike.cxx
MiniBooNE_2DTcos_nu.cxx
MiniBooNE_CCpi0_1Dcosmu_nu.cxx
MiniBooNE_CCpi0_1Dcospi0_nu.cxx
MiniBooNE_CCpi0_1DEnu_nu.cxx
MiniBooNE_CCpi0_1Dppi0_nu.cxx
MiniBooNE_CCpi0_1DQ2_nu.cxx
MiniBooNE_CCpi0_1DTu_nu.cxx
MiniBooNE_CCpip_1DEnu_nu.cxx
MiniBooNE_CCpip_1DQ2_nu.cxx
MiniBooNE_CCpip_1DTpi_nu.cxx
MiniBooNE_CCpip_1DTu_nu.cxx
MiniBooNE_CCpip_2DQ2Enu_nu.cxx
MiniBooNE_CCpip_2DTpiCospi_nu.cxx
MiniBooNE_CCpip_2DTpiEnu_nu.cxx
MiniBooNE_CCpip_2DTuCosmu_nu.cxx
MiniBooNE_CCpip_2DTuEnu_nu.cxx
MiniBooNE_CCpip_CCQE_1DEnu_nu.cxx
MiniBooNE_CCpip_CCQELike_1DEnu_nu.cxx
MiniBooNE_NCpi0_1Dcospi0_antinu.cxx
MiniBooNE_NCpi0_1Dcospi0_joint_antinu.cxx
MiniBooNE_NCpi0_1Dcospi0_joint_nu.cxx
MiniBooNE_NCpi0_1Dcospi0_nu.cxx
MiniBooNE_NCpi0_1Dppi0_antinu.cxx
MiniBooNE_NCpi0_1Dppi0_joint_antinu.cxx
MiniBooNE_NCpi0_1Dppi0_joint_nu.cxx
MiniBooNE_NCpi0_1Dppi0_nu.cxx
SignalDef.cxx
T2K_CCpip_1Dcosmu_nu.cxx
T2K_CCpip_1Dcosmupi_nu.cxx
T2K_CCpip_1Dcospi_nu.cxx
T2K_CCpip_1DEnuDelta_nu.cxx
T2K_CCpip_1DEnuMB_nu.cxx
T2K_CCpip_1Dpmu_nu.cxx
T2K_CCpip_1Dppip_nu.cxx
)

set(HEADERFILES
ANL_CC1npip_1DEnu_nu.h
ANL_CC1npip_1Dppi_nu.h
ANL_CC1npip_1DQ2Ev_nu.h
ANL_CC1pi0_1DEnu_nu.h
ANL_CC1pi0_1DQ2Ev_nu.h
ANL_CC1ppip_1DEnu_nu.h
ANL_CC1ppip_1Dppi_nu.h
ANL_CC1ppip_1DQ2Ev_nu.h
ANL_CC1ppip_1DQ2_nu.h
ANL_CC1ppip_1Dthpr_nu.h
ANL_NC1npip_1Dppi_nu.h
BEBC_CC1npim_1DEnu_antinu.h
BEBC_CC1npim_1DQ2_antinu.h
BEBC_CC1npip_1DEnu_nu.h
BEBC_CC1npip_1DQ2_nu.h
BEBC_CC1pi0_1DEnu_nu.h
BEBC_CC1pi0_1DQ2_nu.h
BEBC_CC1ppim_1DEnu_antinu.h
BEBC_CC1ppim_1DQ2_antinu.h
BEBC_CC1ppip_1DEnu_nu.h
BEBC_CC1ppip_1DQ2_nu.h
BNL_CC1npip_1DEnu_nu.h
BNL_CC1npip_1DQ2Ev_nu.h
BNL_CC1pi0_1DEnu_nu.h
BNL_CC1pi0_1DQ2Ev_nu.h
BNL_CC1ppip_1DEnu_nu.h
BNL_CC1ppip_1DQ2Ev_nu.h
BNL_NC1ppim_1DEnu_nu.h
BNL_NC1ppim_1DQ2_nu.h
FitUtils.h
FNAL_CC1ppim_1DEnu_anu.h
FNAL_CC1ppim_1DQ2_anu.h
FNAL_CC1ppip_1DEnu_nu.h
FNAL_CC1ppip_1DQ2Ev_nu.h
FNAL_CC1ppip_1DQ2_nu.h
GGM_CC1ppip_1DEnu_nu.h
GGM_CC1ppip_1DQ2Ev_nu.h
K2K_CCpip_CCQE_1DEnu_nu.h
K2K_NCpi0_1Dppi0_nu.h
MINERvA_1DQ2_antinu_20deg.h
MINERvA_1DQ2_antinu.h
MINERvA_1DQ2_nu_20deg.h
MINERvA_1DQ2_nu.h
MINERvA_CCcoh_1DEnu_antinu.h
MINERvA_CCcoh_1DEnu_nu.h
MINERvA_CCcoh_1DEpi_antinu.h
MINERvA_CCcoh_1DEpi_nu.h
MINERvA_CCcoh_1Dth_antinu.h
MINERvA_CCcoh_1Dth_nu.h
MINERvA_CCpi0_1Dppi0_antinu.h
MINERvA_CCpi0_1Dth_antinu.h
MINERvA_CCpip_1Dth_20deg_nu.h
MINERvA_CCpip_1Dth_nu.h
MINERvA_CCpip_1DTpi_20deg_nu.h
MINERvA_CCpip_1DTpi_nu.h
MiniBooNE_1DEnuLike_nu.h
MiniBooNE_1DEnu_nu.h
MiniBooNE_1DQ2_antinu.h
MiniBooNE_1DQ2_nu.h
MiniBooNE_2DTcos_antinu_CCQELike.h
MiniBooNE_2DTcos_antinu.h
MiniBooNE_2DTcos_nu_CCQELike.h
MiniBooNE_2DTcos_nu.h
MiniBooNE_CCpi0_1Dcosmu_nu.h
MiniBooNE_CCpi0_1Dcospi0_nu.h
MiniBooNE_CCpi0_1DEnu_nu.h
MiniBooNE_CCpi0_1Dppi0_nu.h
MiniBooNE_CCpi0_1DQ2_nu.h
MiniBooNE_CCpi0_1DTu_nu.h
MiniBooNE_CCpip_1DEnu_nu.h
MiniBooNE_CCpip_1DQ2_nu.h
MiniBooNE_CCpip_1DTpi_nu.h
MiniBooNE_CCpip_1DTu_nu.h
MiniBooNE_CCpip_2DQ2Enu_nu.h
MiniBooNE_CCpip_2DTpiCospi_nu.h
MiniBooNE_CCpip_2DTpiEnu_nu.h
MiniBooNE_CCpip_2DTuCosmu_nu.h
MiniBooNE_CCpip_2DTuEnu_nu.h
MiniBooNE_CCpip_CCQE_1DEnu_nu.h
MiniBooNE_CCpip_CCQELike_1DEnu_nu.h
MiniBooNE_NCpi0_1Dcospi0_antinu.h
MiniBooNE_NCpi0_1Dcospi0_joint_antinu.h
MiniBooNE_NCpi0_1Dcospi0_joint_nu.h
MiniBooNE_NCpi0_1Dcospi0_nu.h
MiniBooNE_NCpi0_1Dppi0_antinu.h
MiniBooNE_NCpi0_1Dppi0_joint_antinu.h
MiniBooNE_NCpi0_1Dppi0_joint_nu.h
MiniBooNE_NCpi0_1Dppi0_nu.h
SignalDef.h
T2K_CCpip_1Dcosmu_nu.h
T2K_CCpip_1Dcosmupi_nu.h
T2K_CCpip_1Dcospi_nu.h
T2K_CCpip_1DEnuDelta_nu.h
T2K_CCpip_1DEnuMB_nu.h
T2K_CCpip_1Dpmu_nu.h
T2K_CCpip_1Dppip_nu.h
)

set(LIBNAME Devel)

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  add_library(${LIBNAME} STATIC ${IMPLFILES})
else(CMAKE_BUILD_TYPE MATCHES RELEASE)
  add_library(${LIBNAME} SHARED ${IMPLFILES})
endif()

set_target_properties(${LIBNAME} PROPERTIES VERSION
  "${ExtFit_VERSION_MAJOR}.${ExtFit_VERSION_MINOR}.${ExtFit_VERSION_REVISION}")
set_target_properties(${LIBNAME} PROPERTIES LINK_FLAGS ${ROOT_LD_FLAGS})

install(TARGETS ${LIBNAME} DESTINATION lib)
#Can uncomment this to install the headers... but is it really neccessary?
#install(FILES ${HEADERFILES} DESTINATION include)

set(MODULETargets ${MODULETargets} ${LIBNAME} PARENT_SCOPE)
