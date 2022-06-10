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
set(ArgoNeuT_Impl_Files
  ArgoNeuT_CC1Pi_XSec_1Dpmu_nu.cxx
  ArgoNeuT_CC1Pi_XSec_1Dpmu_antinu.cxx
  ArgoNeuT_CC1Pi_XSec_1Dthetamu_nu.cxx
  ArgoNeuT_CC1Pi_XSec_1Dthetamu_antinu.cxx
  ArgoNeuT_CC1Pi_XSec_1Dthetapi_nu.cxx
  ArgoNeuT_CC1Pi_XSec_1Dthetapi_antinu.cxx
  ArgoNeuT_CC1Pi_XSec_1Dthetamupi_nu.cxx
  ArgoNeuT_CC1Pi_XSec_1Dthetamupi_antinu.cxx
  ArgoNeuT_CCInc_XSec_1Dpmu_antinu.cxx
  ArgoNeuT_CCInc_XSec_1Dpmu_nu.cxx
  ArgoNeuT_CCInc_XSec_1Dthetamu_antinu.cxx
  ArgoNeuT_CCInc_XSec_1Dthetamu_nu.cxx
)

add_library(ArgoNeuT SHARED ${ArgoNeuT_Impl_Files})
target_link_libraries(ArgoNeuT CoreIncludes)

install(TARGETS ArgoNeuT DESTINATION lib)
