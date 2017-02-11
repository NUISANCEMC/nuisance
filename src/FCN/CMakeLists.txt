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
JointFCN.cxx
SampleList.cxx
)

set(HEADERFILES
JointFCN.h
MinimizerFCN.h
SampleList.h
)

set(LIBNAME FCN)

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  add_library(${LIBNAME} STATIC ${IMPLFILES})
else(CMAKE_BUILD_TYPE MATCHES RELEASE)
  add_library(${LIBNAME} SHARED ${IMPLFILES})
endif()

include_directories(${RWENGINE_INCLUDE_DIRECTORIES})
include_directories(${EXP_INCLUDE_DIRECTORIES})

include_directories(${CMAKE_SOURCE_DIR}/src/Splines)
include_directories(${CMAKE_SOURCE_DIR}/src/Utils)
include_directories(${CMAKE_SOURCE_DIR}/src/FitBase)

include_directories(${CMAKE_SOURCE_DIR}/src/ANL)
include_directories(${CMAKE_SOURCE_DIR}/src/BNL)
include_directories(${CMAKE_SOURCE_DIR}/src/FNAL)
include_directories(${CMAKE_SOURCE_DIR}/src/GGM)
include_directories(${CMAKE_SOURCE_DIR}/src/MINERvA)
include_directories(${CMAKE_SOURCE_DIR}/src/MiniBooNE)
include_directories(${CMAKE_SOURCE_DIR}/src/T2K)

include_directories(${CMAKE_SOURCE_DIR}/src/MCStudies)
include_directories(${CMAKE_SOURCE_DIR}/src/FCN)

set_target_properties(${LIBNAME} PROPERTIES VERSION
  "${ExtFit_VERSION_MAJOR}.${ExtFit_VERSION_MINOR}.${ExtFit_VERSION_REVISION}")
set_target_properties(${LIBNAME} PROPERTIES LINK_FLAGS ${ROOT_LD_FLAGS})
add_dependencies(${LIBNAME} ${PROJECTWIDE_EXTRA_DEPENDENCIES})

install(TARGETS ${LIBNAME} DESTINATION lib)
#Can uncomment this to install the headers... but is it really neccessary?
#install(FILES ${HEADERFILES} DESTINATION include)

set(MODULETargets ${MODULETargets} ${LIBNAME} PARENT_SCOPE)
