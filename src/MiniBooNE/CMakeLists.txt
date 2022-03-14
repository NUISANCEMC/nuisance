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

set(MiniBooNE_Impl_Files
  MiniBooNE_CC1pi0_XSec_1Dcosmu_nu.cxx
  MiniBooNE_CC1pip_XSec_2DTpiCospi_nu.cxx
  MiniBooNE_CC1pi0_XSec_1Dcospi0_nu.cxx
  MiniBooNE_CC1pip_XSec_2DTpiEnu_nu.cxx
  MiniBooNE_CC1pi0_XSec_1DEnu_nu.cxx
  MiniBooNE_CCQE_XSec_1DEnu_nu.cxx
  MiniBooNE_CC1pip_XSec_2DTuCosmu_nu.cxx
  MiniBooNE_CC1pi0_XSec_1Dppi0_nu.cxx
  MiniBooNE_CC1pip_XSec_2DTuEnu_nu.cxx
  MiniBooNE_CC1pi0_XSec_1DQ2_nu.cxx
  MiniBooNE_CCQE_XSec_1DQ2_antinu.cxx
  MiniBooNE_CC1pi0_XSec_1DTu_nu.cxx
  MiniBooNE_CC1pip_XSec_1DEnu_nu.cxx
  MiniBooNE_CCQE_XSec_1DQ2_nu.cxx
  MiniBooNE_CC1pip_XSec_1DQ2_nu.cxx
  MiniBooNE_CCQE_XSec_2DTcos_antinu.cxx
  MiniBooNE_CC1pip_XSec_1DTpi_nu.cxx
  MiniBooNE_CCQE_XSec_2DTcos_nu.cxx
  MiniBooNE_CC1pip_XSec_1DTu_nu.cxx
  MiniBooNE_NCEL_XSec_Treco_nu.cxx
  MiniBooNE_CC1pip_XSec_2DQ2Enu_nu.cxx
  MiniBooNE_NC1pi0_XSec_1Dppi0_nu.cxx
  MiniBooNE_NC1pi0_XSec_1Dcospi0_nu.cxx
  MiniBooNE_NC1pi0_XSec_1Dppi0_antinu.cxx
  MiniBooNE_NC1pi0_XSec_1Dcospi0_antinu.cxx
)

add_library(MiniBooNE SHARED ${MiniBooNE_Impl_Files})

install(TARGETS MiniBooNE DESTINATION lib)
