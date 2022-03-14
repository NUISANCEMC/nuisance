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
set(FermilabBC_Impl_Files
  FNAL_CC1ppip_Evt_1DQ2_nu.cxx
  FNAL_CC1ppip_XSec_1DEnu_nu.cxx
  FNAL_CC1ppip_XSec_1DQ2_nu.cxx
  FNAL_CCQE_Evt_1DQ2_nu.cxx
  FNAL_CC1ppim_XSec_1DEnu_antinu.cxx
)

add_library(FermilabBC SHARED ${FermilabBC_Impl_Files})

install(TARGETS FermilabBC DESTINATION lib)
