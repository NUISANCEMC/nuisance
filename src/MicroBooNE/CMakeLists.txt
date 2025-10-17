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
set(MicroBooNE_Impl_Files
  MicroBooNE_CCInc_XSec_2DPcos_nu.cxx
  MicroBooNE_CC1MuNp_XSec_1D_nu.cxx
  MicroBooNE_CC1ENp_XSec_1D_nu.cxx
  MicroBooNE_CC1Mu2p_XSec_1D_nu.cxx
  MicroBooNE_CC1Mu1p_XSec_1D_nu.cxx
  MicroBooNE_BNB_NueCC0pi_2022_XSec_1D_nu.cxx
  MicroBooNE_SignalDef.cxx
)

set(MicroBooNE_Hdr_Files
  MicroBooNE_CCInc_XSec_2DPcos_nu.h
  MicroBooNE_CC1MuNp_XSec_1D_nu.h
  MicroBooNE_CC1Mu2p_XSec_1D_nu.h
  MicroBooNE_CC1Mu1p_XSec_1D_nu.h
  MicroBooNE_BNB_NueCC0pi_2022_XSec_1D_nu.h
  MicroBooNE_SignalDef.h
)

add_library(MicroBooNE SHARED ${MicroBooNE_Impl_Files})
target_link_libraries(MicroBooNE FrameworkLibraries CoreIncludes)

install(TARGETS MicroBooNE
    EXPORT nuisance-targets
    LIBRARY DESTINATION lib/
    PUBLIC_HEADER DESTINATION include)

add_library(NUIS::MicroBooNE ALIAS MicroBooNE)
