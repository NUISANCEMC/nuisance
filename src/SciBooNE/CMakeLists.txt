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

set(SciBooNE_Impl_Files
  SciBooNE_CCCOH_STOP_NTrks_nu.cxx
  SciBooNE_CCCOH_1TRK_1DQ2_nu.cxx
  SciBooNE_CCCOH_1TRK_1Dpmu_nu.cxx
  SciBooNE_CCCOH_1TRK_1Dthetamu_nu.cxx
  SciBooNE_CCCOH_MuPr_1DQ2_nu.cxx
  SciBooNE_CCCOH_MuPr_1Dthetamu_nu.cxx
  SciBooNE_CCCOH_MuPr_1Dpmu_nu.cxx
  SciBooNE_CCCOH_MuPiVA_1DQ2_nu.cxx
  SciBooNE_CCCOH_MuPiVA_1Dthetamu_nu.cxx
  SciBooNE_CCCOH_MuPiVA_1Dpmu_nu.cxx
  SciBooNE_CCCOH_MuPiNoVA_1DQ2_nu.cxx
  SciBooNE_CCCOH_MuPiNoVA_1Dthetapr_nu.cxx
  SciBooNE_CCCOH_MuPiNoVA_1Dthetapi_nu.cxx
  SciBooNE_CCCOH_MuPiNoVA_1Dthetamu_nu.cxx
  SciBooNE_CCCOH_MuPiNoVA_1Dpmu_nu.cxx
  SciBooNE_CCCOH_STOPFINAL_1DQ2_nu.cxx
  SciBooNE_CCInc_XSec_1DEnu_nu.cxx
  SciBooNEUtils.cxx
)

add_library(SciBooNE SHARED ${SciBooNE_Impl_Files})

install(TARGETS SciBooNE DESTINATION lib)

