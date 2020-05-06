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
GlobalDialList.cxx
FitWeight.cxx
WeightEngineBase.cxx
NEUTWeightEngine.cxx
NuWroWeightEngine.cxx
GENIEWeightEngine.cxx
WeightUtils.cxx
SampleNormEngine.cxx
LikelihoodWeightEngine.cxx
SplineWeightEngine.cxx
NUISANCESyst.cxx
T2KWeightEngine.cxx
NUISANCEWeightEngine.cxx
NUISANCEWeightCalcs.cxx
NIWGWeightEngine.cxx
OscWeightEngine.cxx
MINERvAWeightCalcs.cxx
weightRPA.h
)

if(USE_NOvARwgt)
  LIST(APPEND IMPLFILES NOvARwgtEngine.cxx)
endif()

if(USE_NUSYST)
  LIST(APPEND IMPLFILES nusystematicsWeightEngine.cxx)
endif()

set(HEADERFILES
GlobalDialList.h
FitWeight.h
WeightEngineBase.h
NEUTWeightEngine.h
NuWroWeightEngine.h
GENIEWeightEngine.h
WeightUtils.h
SampleNormEngine.h
LikelihoodWeightEngine.h
SplineWeightEngine.h
NUISANCESyst.h
T2KWeightEngine.h
NUISANCEWeightEngine.h
NUISANCEWeightCalcs.h
NIWGWeightEngine.h
OscWeightEngine.h
MINERvAWeightCalcs.h
weightRPA.h
BeRPA.h
)

if(USE_NOvARwgt)
  LIST(APPEND HEADERFILES NOvARwgtEngine.h)
endif()

set(LIBNAME Reweight)

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  add_library(${LIBNAME} STATIC ${IMPLFILES})
else(CMAKE_BUILD_TYPE MATCHES RELEASE)
  add_library(${LIBNAME} SHARED ${IMPLFILES})
endif()

include_directories(${MINIMUM_INCLUDE_DIRECTORIES})

set_target_properties(${LIBNAME} PROPERTIES VERSION
  "${ExtFit_VERSION_MAJOR}.${ExtFit_VERSION_MINOR}.${ExtFit_VERSION_REVISION}")
#set_target_properties(${LIBNAME} PROPERTIES LINK_FLAGS ${ROOT_LD_FLAGS})

if(DEFINED PROJECTWIDE_EXTRA_DEPENDENCIES)
  add_dependencies(${LIBNAME} ${PROJECTWIDE_EXTRA_DEPENDENCIES})
endif()

install(TARGETS ${LIBNAME} DESTINATION lib)
#Can uncomment this to install the headers... but is it really neccessary?
install(FILES ${HEADERFILES} DESTINATION include/Reweight)

set(MODULETargets ${MODULETargets} ${LIBNAME} PARENT_SCOPE)
