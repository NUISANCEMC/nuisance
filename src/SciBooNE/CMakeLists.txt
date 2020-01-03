# Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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
set(IMPLFILES
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

set(HEADERFILES
SciBooNE_CCCOH_STOP_NTrks_nu.h
SciBooNE_CCCOH_1TRK_1DQ2_nu.h
SciBooNE_CCCOH_1TRK_1Dpmu_nu.h
SciBooNE_CCCOH_1TRK_1Dthetamu_nu.h
SciBooNE_CCCOH_MuPr_1DQ2_nu.h
SciBooNE_CCCOH_MuPr_1Dthetamu_nu.h
SciBooNE_CCCOH_MuPr_1Dpmu_nu.h
SciBooNE_CCCOH_MuPiVA_1DQ2_nu.h
SciBooNE_CCCOH_MuPiVA_1Dthetamu_nu.h
SciBooNE_CCCOH_MuPiVA_1Dpmu_nu.h
SciBooNE_CCCOH_MuPiNoVA_1DQ2_nu.h
SciBooNE_CCCOH_MuPiNoVA_1Dthetapr_nu.h
SciBooNE_CCCOH_MuPiNoVA_1Dthetapi_nu.h
SciBooNE_CCCOH_MuPiNoVA_1Dthetamu_nu.h
SciBooNE_CCCOH_MuPiNoVA_1Dpmu_nu.h
SciBooNE_CCCOH_STOPFINAL_1DQ2_nu.h
SciBooNE_CCInc_XSec_1DEnu_nu.h
SciBooNEUtils.h
)

set(LIBNAME expSciBooNE)

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  add_library(${LIBNAME} STATIC ${IMPLFILES})
else(CMAKE_BUILD_TYPE MATCHES RELEASE)
  add_library(${LIBNAME} SHARED ${IMPLFILES})
endif()

include_directories(${MINIMUM_INCLUDE_DIRECTORIES})

set_target_properties(${LIBNAME} PROPERTIES VERSION
  "${ExtFit_VERSION_MAJOR}.${ExtFit_VERSION_MINOR}.${ExtFit_VERSION_REVISION}")
#set_target_properties(${LIBNAME} PROPERTIES LINK_FLAGS ${ROOT_LD_FLAGS})

install(TARGETS ${LIBNAME} DESTINATION lib)
#Can uncomment this to install the headers... but is it really neccessary?
install(FILES ${HEADERFILES} DESTINATION include/SciBooNE)

set(MODULETargets ${MODULETargets} ${LIBNAME} PARENT_SCOPE)
