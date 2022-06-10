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

set(BNL_Impl_Files
  BNL_CC1npip_Evt_1DQ2_nu.cxx
  BNL_CC1npip_XSec_1DEnu_nu.cxx
  BNL_CC1npip_Evt_1DWmupi_nu.cxx
  BNL_CC1npip_Evt_1DWNpi_nu.cxx
  BNL_CC1npip_Evt_1DWNmu_nu.cxx
  BNL_CC1pi0_Evt_1DQ2_nu.cxx
  BNL_CC1pi0_XSec_1DEnu_nu.cxx
  BNL_CC1pi0_Evt_1DWmupi_nu.cxx
  BNL_CC1pi0_Evt_1DWNpi_nu.cxx
  BNL_CC1pi0_Evt_1DWNmu_nu.cxx
  BNL_CC1ppip_Evt_1DQ2_nu.cxx
  BNL_CC1ppip_XSec_1DEnu_nu.cxx
  BNL_CC1ppip_Evt_1DWmupi_nu.cxx
  BNL_CC1ppip_Evt_1DWNpi_nu.cxx
  BNL_CC1ppip_Evt_1DWNmu_nu.cxx
  BNL_CCQE_Evt_1DQ2_nu.cxx
  BNL_CCQE_XSec_1DEnu_nu.cxx
  BNL_CC1ppip_Evt_1DcosthAdler_nu.cxx
  BNL_CC1ppip_Evt_1Dphi_nu.cxx
  BNL_CC2pi_1pim1pip_XSec_1DEnu_nu.cxx
  BNL_CC3pi_1pim2pip_XSec_1DEnu_nu.cxx
  BNL_CC4pi_2pim2pip_XSec_1DEnu_nu.cxx
  BNL_CC2pi_1pim1pip_Evt_1DWpippim_nu.cxx
  BNL_CC2pi_1pim1pip_Evt_1DWpippr_nu.cxx
)

add_library(BNL SHARED ${BNL_Impl_Files})
target_link_libraries(BNL CoreIncludes)

install(TARGETS BNL DESTINATION lib)
