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

set(BigEuropeanBC_Impl_Files
  BEBC_CC1npim_XSec_1DEnu_antinu.cxx
  BEBC_CC1npim_XSec_1DQ2_antinu.cxx
  BEBC_CC1npip_XSec_1DEnu_nu.cxx
  BEBC_CC1npip_XSec_1DQ2_nu.cxx
  BEBC_CC1pi0_XSec_1DEnu_nu.cxx
  BEBC_CC1pi0_XSec_1DQ2_nu.cxx
  BEBC_CC1ppim_XSec_1DEnu_antinu.cxx
  BEBC_CC1ppim_XSec_1DQ2_antinu.cxx
  BEBC_CC1ppip_XSec_1DEnu_nu.cxx
  BEBC_CC1ppip_XSec_1DQ2_nu.cxx
  BEBC_CCQE_XSec_1DQ2_nu.cxx
)

add_library(BigEuropeanBC SHARED ${BigEuropeanBC_Impl_Files})

install(TARGETS BigEuropeanBC DESTINATION lib)
