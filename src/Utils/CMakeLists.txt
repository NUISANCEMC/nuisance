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
set(HEADERFILES
FitLogger.h
FitParameters.h
FitUtils.h
GeneralUtils.h
ParserUtils.h
PlotUtils.h
StatUtils.h
SignalDef.h
NuisConfig.h
NuisKey.h
BeamUtils.h
TargetUtils.h
StackBase.h
StandardStacks.h
OpenMPWrapper.h
PhysConst.h
Initialiser.h
)

set(IMPLFILES
PythiaQuiet.f
FitLogger.cxx
FitParameters.cxx
FitUtils.cxx
GeneralUtils.cxx
PlotUtils.cxx
StatUtils.cxx
SignalDef.cxx
NuisConfig.cxx
NuisKey.cxx
BeamUtils.cxx
TargetUtils.cxx
StackBase.cxx
ParserUtils.cxx
StandardStacks.cxx
Initialiser.cxx
)

set(LIBNAME Utils)

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  add_library(${LIBNAME} STATIC ${IMPLFILES})
else(CMAKE_BUILD_TYPE MATCHES RELEASE)
  add_library(${LIBNAME} SHARED ${IMPLFILES})
endif()

include_directories(${MINIMUM_INCLUDE_DIRECTORIES})

set_target_properties(${LIBNAME} PROPERTIES VERSION
  "${NUISANCE_VERSION_MAJOR}.${NUISANCE_VERSION_MINOR}.${NUISANCE_VERSION_REVISION}")

#set_target_properties(${LIBNAME} PROPERTIES LINK_FLAGS ${ROOT_LD_FLAGS})

if(DEFINED PROJECTWIDE_EXTRA_DEPENDENCIES)
  add_dependencies(${LIBNAME} ${PROJECTWIDE_EXTRA_DEPENDENCIES})
endif()

install(TARGETS ${LIBNAME} DESTINATION lib)
#Can uncomment this to install the headers... but is it really neccessary?
#install(FILES ${HEADERFILES} DESTINATION include)

set(MODULETargets ${MODULETargets} ${LIBNAME} PARENT_SCOPE)

#add_executable(DumpROOTClassesFromVector DumpROOTClassesFromVector.cxx GeneralUtils.cxx FitLogger.cxx PythiaQuiet.f)

#target_link_libraries(DumpROOTClassesFromVector ${ROOT_LIBS})
#if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
#  set_target_properties(DumpROOTClassesFromVector PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
#endif()
#set_target_properties(DumpROOTClassesFromVector PROPERTIES LINK_FLAGS ${ROOT_LD_FLAGS})

#install(TARGETS DumpROOTClassesFromVector DESTINATION bin)
