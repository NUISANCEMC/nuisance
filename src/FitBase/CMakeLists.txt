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
set(IMPLFILES
ParamPull.cxx
EventManager.cxx
Measurement1D.cxx
Measurement2D.cxx
JointMeas1D.cxx
MeasurementBase.cxx
TemplateMeas1D.cxx
SampleSettings.cxx
MeasurementVariableBox.cxx
MeasurementVariableBox2D.cxx
MeasurementVariableBox1D.cxx
StandardStacks.cxx
StackBase.cxx
)

set(HEADERFILES
ParamPull.h
JointMeas1D.h
Measurement2D.h
EventManager.h
MeasurementBase.h
Measurement1D.h
TemplateMeas1D.h
SampleSettings.h
MeasurementVariableBox.h
MeasurementVariableBox2D.h
MeasurementVariableBox1D.h
CustomVariableBoxes.h
StandardStacks.h
StackBase.h
)

set(LIBNAME FitBase)

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  add_library(${LIBNAME} STATIC ${IMPLFILES})
else(CMAKE_BUILD_TYPE MATCHES RELEASE)
  add_library(${LIBNAME} SHARED ${IMPLFILES})
endif()

include_directories(${MINIMUM_INCLUDE_DIRECTORIES})
cmessage(DEBUG "FitBase includes: ${MINIMUM_INCLUDE_DIRECTORIES}")

set_target_properties(${LIBNAME} PROPERTIES VERSION
  "${NUISANCE_VERSION_MAJOR}.${NUISANCE_VERSION_MINOR}.${NUISANCE_VERSION_REVISION}")
#set_target_properties(${LIBNAME} PROPERTIES LINK_FLAGS ${ROOT_LD_FLAGS})

if(DEFINED PROJECTWIDE_EXTRA_DEPENDENCIES)
  add_dependencies(${LIBNAME} ${PROJECTWIDE_EXTRA_DEPENDENCIES})
endif()

install(TARGETS ${LIBNAME} DESTINATION lib)
#Can uncomment this to install the headers... but is it really neccessary?
install(FILES ${HEADERFILES} DESTINATION include/FitBase)

set(MODULETargets ${MODULETargets} ${LIBNAME} PARENT_SCOPE)
