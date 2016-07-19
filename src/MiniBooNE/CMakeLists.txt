# Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

################################################################################
#    This file is part of NuFiX.
#
#    NuFiX is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    NuFiX is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
################################################################################
set(IMPLFILES
MiniBooNE_CC1pi0_XSec_1Dcosmu_nu.cxx
MiniBooNE_CC1pip_XSec_2DTpiCospi_nu.cxx
MiniBooNE_CC1pi0_XSec_1Dcospi0_nu.cxx
MiniBooNE_CC1pip_XSec_2DTpiEnu_nu.cxx
MiniBooNE_CC1pi0_XSec_1DEnu_nu.cxx
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
# MiniBooNE_NCEL_XSec_Treco_nu.cxx
MiniBooNE_CC1pip_XSec_2DQ2Enu_nu.cxx
MiniBooNE_NCpi0_XSec_1Dppi0_nu.cxx
)

set(HEADERFILES
MiniBooNE_CC1pi0_XSec_1Dcosmu_nu.h
MiniBooNE_CC1pip_XSec_2DTpiCospi_nu.h
MiniBooNE_CC1pi0_XSec_1Dcospi0_nu.h
MiniBooNE_CC1pip_XSec_2DTpiEnu_nu.h
MiniBooNE_CC1pi0_XSec_1DEnu_nu.h
MiniBooNE_CC1pip_XSec_2DTuCosmu_nu.h
MiniBooNE_CC1pi0_XSec_1Dppi0_nu.h
MiniBooNE_CC1pip_XSec_2DTuEnu_nu.h
MiniBooNE_CC1pi0_XSec_1DQ2_nu.h
MiniBooNE_CCQE_XSec_1DQ2_antinu.h
MiniBooNE_CC1pi0_XSec_1DTu_nu.h
MiniBooNE_CC1pip_XSec_1DEnu_nu.h
MiniBooNE_CCQE_XSec_1DQ2_nu.h
MiniBooNE_CC1pip_XSec_1DQ2_nu.h
MiniBooNE_CCQE_XSec_2DTcos_antinu.h
MiniBooNE_CC1pip_XSec_1DTpi_nu.h
MiniBooNE_CCQE_XSec_2DTcos_nu.h
MiniBooNE_CC1pip_XSec_1DTu_nu.h
# MiniBooNE_NCEL_XSec_Treco_nu.h
MiniBooNE_CC1pip_XSec_2DQ2Enu_nu.h
MiniBooNE_NCpi0_XSec_1Dppi0_nu.h
)

set(LIBNAME expMiniBooNE)

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  add_library(${LIBNAME} STATIC ${IMPLFILES})
else(CMAKE_BUILD_TYPE MATCHES RELEASE)
  add_library(${LIBNAME} SHARED ${IMPLFILES})
endif()

include_directories(${RWENGINE_INCLUDE_DIRECTORIES})

include_directories(${CMAKE_SOURCE_DIR}/src/FitBase)
include_directories(${CMAKE_SOURCE_DIR}/src/Utils)

set_target_properties(${LIBNAME} PROPERTIES VERSION
  "${ExtFit_VERSION_MAJOR}.${ExtFit_VERSION_MINOR}.${ExtFit_VERSION_REVISION}")
set_target_properties(${LIBNAME} PROPERTIES LINK_FLAGS ${ROOT_LD_FLAGS})

install(TARGETS ${LIBNAME} DESTINATION lib)
#Can uncomment this to install the headers... but is it really neccessary?
#install(FILES ${HEADERFILES} DESTINATION include)

set(MODULETargets ${MODULETargets} ${LIBNAME} PARENT_SCOPE)
