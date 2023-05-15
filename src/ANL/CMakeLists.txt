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

set(ANL_Impl_Files
  ANL_CCQE_Evt_1DQ2_nu.cxx
  ANL_CCQE_XSec_1DEnu_nu.cxx

  ANL_CCQE_HEPDATA.cxx
  ANL_CCQE_HEPDATA_2D.cxx

  ANL_CC1npip_Evt_1DcosmuStar_nu.cxx
  ANL_CC1npip_Evt_1Dppi_nu.cxx
  ANL_CC1npip_Evt_1DQ2_nu.cxx
  ANL_CC1npip_XSec_1DEnu_nu.cxx
  ANL_CC1npip_Evt_1DWNpi_nu.cxx
  ANL_CC1npip_Evt_1DWNmu_nu.cxx
  ANL_CC1npip_Evt_1DWmupi_nu.cxx

  ANL_CC1pi0_Evt_1DcosmuStar_nu.cxx
  ANL_CC1pi0_Evt_1DQ2_nu.cxx
  ANL_CC1pi0_XSec_1DEnu_nu.cxx
  ANL_CC1pi0_Evt_1DWNpi_nu.cxx
  ANL_CC1pi0_Evt_1DWNmu_nu.cxx
  ANL_CC1pi0_Evt_1DWmupi_nu.cxx

  ANL_CC1ppip_Evt_1DcosmuStar_nu.cxx
  ANL_CC1ppip_Evt_1Dppi_nu.cxx
  ANL_CC1ppip_Evt_1DQ2_nu.cxx
  ANL_CC1ppip_Evt_1Dthpr_nu.cxx
  ANL_CC1ppip_XSec_1DEnu_nu.cxx
  ANL_CC1ppip_XSec_1DQ2_nu.cxx
  ANL_CC1ppip_Evt_1DcosthAdler_nu.cxx
  ANL_CC1ppip_Evt_1Dphi_nu.cxx
  ANL_CC1ppip_Evt_1DWNpi_nu.cxx
  ANL_CC1ppip_Evt_1DWNmu_nu.cxx
  ANL_CC1ppip_Evt_1DWmupi_nu.cxx

  ANL_NC1npip_Evt_1Dppi_nu.cxx

  ANL_NC1ppim_XSec_1DEnu_nu.cxx
  ANL_NC1ppim_Evt_1DcosmuStar_nu.cxx

  ANL_CC2pi_1pim1pip_XSec_1DEnu_nu.cxx
  ANL_CC2pi_1pim1pip_Evt_1Dpmu_nu.cxx
  ANL_CC2pi_1pim1pip_Evt_1Dppim_nu.cxx
  ANL_CC2pi_1pim1pip_Evt_1Dppip_nu.cxx
  ANL_CC2pi_1pim1pip_Evt_1Dpprot_nu.cxx

  ANL_CC2pi_1pip1pi0_XSec_1DEnu_nu.cxx
  ANL_CC2pi_1pip1pi0_Evt_1Dpmu_nu.cxx
  ANL_CC2pi_1pip1pi0_Evt_1Dppi0_nu.cxx
  ANL_CC2pi_1pip1pi0_Evt_1Dppip_nu.cxx
  ANL_CC2pi_1pip1pi0_Evt_1Dpprot_nu.cxx

  ANL_CC2pi_1pip1pip_XSec_1DEnu_nu.cxx
  ANL_CC2pi_1pip1pip_Evt_1Dpmu_nu.cxx
  ANL_CC2pi_1pip1pip_Evt_1Dpneut_nu.cxx
  ANL_CC2pi_1pip1pip_Evt_1DppipHigh_nu.cxx
  ANL_CC2pi_1pip1pip_Evt_1DppipLow_nu.cxx
)


add_library(ANL SHARED ${ANL_Impl_Files})
target_link_libraries(ANL CoreIncludes yaml-cpp)

install(TARGETS ANL DESTINATION lib)
