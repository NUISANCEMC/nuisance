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

target_include_directories(${LIBNAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${LIBNAME} PUBLIC ${CMAKE_SOURCE_DIR}/src/Splines)
target_include_directories(${LIBNAME} PUBLIC ${CMAKE_SOURCE_DIR}/src/InputHandler)
target_include_directories(${LIBNAME} PUBLIC ${CMAKE_SOURCE_DIR}/src/FitBase)
target_include_directories(${LIBNAME} PUBLIC ${RWENGINE_INCLUDE_DIRECTORIES})

# if(USE_NuWro AND NUWRO_BUILT_FROM_FILE)
#   target_include_directories(${LIBNAME} PUBLIC ${CMAKE_BINARY_DIR}/NuWro_event1)
# endif()

set_target_properties(${LIBNAME} PROPERTIES VERSION
  "${ExtFit_VERSION_MAJOR}.${ExtFit_VERSION_MINOR}.${ExtFit_VERSION_REVISION}")

set_target_properties(${LIBNAME} PROPERTIES LINK_FLAGS ${ROOT_LD_FLAGS})

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
