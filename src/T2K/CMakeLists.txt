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
set(T2K_Impl_Files
  T2K_CC0pi_XSec_H2O_2DPcos_anu.cxx
  T2K_NuMu_CC0pi_OC_XSec_2DPcos.cxx
  T2K_NuMu_CC0pi_OC_XSec_2DPcos_joint.cxx
  T2K_NuMuAntiNuMu_CC0pi_CH_XSec_2DPcos.cxx
  T2K_NuMuAntiNuMu_CC0pi_CH_XSec_2DPcos_joint.cxx
  T2K_CC0pi_XSec_2DPcos_nu_I.cxx
  T2K_CC0pi_XSec_2DPcos_nu_II.cxx
  T2K_CCinc_XSec_2DPcos_nu_nonuniform.cxx
  T2K_nueCCinc_XSec_1Dpe.cxx
  T2K_nueCCinc_XSec_1Dthe.cxx
  T2K_nueCCinc_XSec_1Dpe_joint.cxx
  T2K_nueCCinc_XSec_1Dthe_joint.cxx
  T2K_nueCCinc_XSec_joint.cxx

  T2K_CC1pip_CH_XSec_2Dpmucosmu_nu.cxx
  T2K_CC1pip_CH_XSec_1Dppi_nu.cxx
  T2K_CC1pip_CH_XSec_1Dthpi_nu.cxx
  T2K_CC1pip_CH_XSec_1Dthmupi_nu.cxx
  T2K_CC1pip_CH_XSec_1DQ2_nu.cxx
  T2K_CC1pip_CH_XSec_1DAdlerPhi_nu.cxx
  T2K_CC1pip_CH_XSec_1DCosThAdler_nu.cxx

  T2K_CCCOH_C12_XSec_1DEnu_nu.cxx

  T2K_CC1pip_H2O_XSec_1DEnuDelta_nu.cxx
  T2K_CC1pip_H2O_XSec_1DEnuMB_nu.cxx
  T2K_CC1pip_H2O_XSec_1Dcosmu_nu.cxx
  T2K_CC1pip_H2O_XSec_1Dcosmupi_nu.cxx
  T2K_CC1pip_H2O_XSec_1Dcospi_nu.cxx
  T2K_CC1pip_H2O_XSec_1Dpmu_nu.cxx
  T2K_CC1pip_H2O_XSec_1Dppi_nu.cxx

  T2K_CC0pinp_STV_XSec_1Ddpt_nu.cxx
  T2K_CC0pinp_STV_XSec_1Ddphit_nu.cxx
  T2K_CC0pinp_STV_XSec_1Ddat_nu.cxx
  T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p_Np.cxx
  T2K_CC0pinp_ifk_XSec_3Dinfp_nu.cxx
  T2K_CC0pinp_ifk_XSec_3Dinfa_nu.cxx
  T2K_CC0pinp_ifk_XSec_3Dinfip_nu.cxx

  T2K_NC1pip_XSec_lin2D.cxx

  T2K_SignalDef.cxx
)

add_library(T2K SHARED ${T2K_Impl_Files})
target_link_libraries(T2K CoreIncludes)

install(TARGETS T2K DESTINATION lib)

