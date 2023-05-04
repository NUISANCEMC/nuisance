# Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

set(MINERvA_Impl_Files
  MINERvA_CCQE_XSec_1DQ2_antinu.cxx
  MINERvA_CCQE_XSec_1DQ2_joint.cxx
  MINERvA_CCQE_XSec_1DQ2_nu.cxx

  MINERvA_CC0pi_XSec_1DEe_nue.cxx
  MINERvA_CC0pi_XSec_1DQ2_nue.cxx
  MINERvA_CC0pi_XSec_1DQ2_nu_proton.cxx
  MINERvA_CC0pi_XSec_1DThetae_nue.cxx

  MINERvA_CC0pinp_STV_XSec_1D_nu.cxx

  MINERvA_CC1pi0_XSec_1DEnu_antinu.cxx
  MINERvA_CC1pi0_XSec_1DQ2_antinu.cxx
  MINERvA_CC1pi0_XSec_1Dpmu_antinu.cxx
  MINERvA_CC1pi0_XSec_1Dppi0_antinu.cxx
  MINERvA_CC1pi0_XSec_1DTpi0_antinu.cxx
  MINERvA_CC1pi0_XSec_1Dth_antinu.cxx
  MINERvA_CC1pi0_XSec_1Dthmu_antinu.cxx

  MINERvA_CC1pi0_XSec_1D_nu.cxx

  MINERvA_CC1pip_XSec_1DTpi_20deg_nu.cxx
  MINERvA_CC1pip_XSec_1DTpi_nu.cxx
  MINERvA_CC1pip_XSec_1Dth_20deg_nu.cxx
  MINERvA_CC1pip_XSec_1Dth_nu.cxx
  MINERvA_CC1pip_XSec_1D_2017Update.cxx

  MINERvA_CC1pim_XSec_1DEnu_antinu.cxx
  MINERvA_CC1pim_XSec_1DQ2_antinu.cxx
  MINERvA_CC1pim_XSec_1Dthmu_antinu.cxx
  MINERvA_CC1pim_XSec_1Dpmu_antinu.cxx
  MINERvA_CC1pim_XSec_1Dth_antinu.cxx
  MINERvA_CC1pim_XSec_1DTpi_antinu.cxx

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

  MINERvA_CC0pi_XSec_2D_antinu.cxx
  MINERvA_CC0pi_XSec_2D_nu.cxx
  MINERvA_NukeCC0pi_XSec_2D_nu.cxx
  # MINERvA_CC0pi_XSec_3DptpzTp_nu.cxx
  MINERvA_CC0pi_XSec_1D_2018_nu.cxx

  MINERvA_CCCOHPI_XSec_1DEnu_nu.cxx
  MINERvA_CCCOHPI_XSec_1DEpi_nu.cxx
  MINERvA_CCCOHPI_XSec_1Dth_nu.cxx
  MINERvA_CCCOHPI_XSec_1DQ2_nu.cxx

  MINERvA_CCCOHPI_XSec_1DEnu_antinu.cxx
  MINERvA_CCCOHPI_XSec_1DEpi_antinu.cxx
  MINERvA_CCCOHPI_XSec_1Dth_antinu.cxx
  MINERvA_CCCOHPI_XSec_1DQ2_antinu.cxx

  MINERvA_CCCOHPI_XSec_joint.cxx

  MINERvAUtils.cxx
  MINERvA_SignalDef.cxx
)

add_library(MINERvA SHARED ${MINERvA_Impl_Files})
target_link_libraries(MINERvA CoreIncludes)

install(TARGETS MINERvA DESTINATION lib)
