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
BaseFitEvt.cxx
FitParticle.cxx
FitEvent.cxx
GeneratorUtils.cxx
StdHepEvt.cxx
InputUtils.cxx
NEUTInputHandler.cxx
GENIEInputHandler.cxx
NuWroInputHandler.cxx
GIBUUInputHandler.cxx
NUANCEInputHandler.cxx
InputHandler.cxx
NuanceEvent.cxx
FitEventInputHandler.cxx
SplineInputHandler.cxx
GeneratorInfoBase.h
InputTypes.h
HepMCTextInputHandler.cxx
InputFactory.cxx
)

set(HEADERFILES
NuanceEvent.h
BaseFitEvt.h
FitEvent.h
FitParticle.h
GeneratorUtils.h
StdHepEvt.h
InputUtils.h
NEUTInputHandler.h
HepMCTextInputHandler.h
GENIEInputHandler.h
NuWroInputHandler.h
InputHandler.h
GIBUUInputHandler.h
NUANCEInputHandler.h
FitEventInputHandler.h
SplineInputHandler.h
GeneratorInfoBase.h
InputFactory.h
InteractionModes.h
)

set(LIBNAME InputHandler)

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

if(HEPMC_USED_EP)
  add_dependencies(${LIBNAME} HepMC)
endif()

install(TARGETS ${LIBNAME} DESTINATION lib)
#Can uncomment this to install the headers... but is it really neccessary?
install(FILES ${HEADERFILES} DESTINATION include)

set(MODULETargets ${MODULETargets} ${LIBNAME} PARENT_SCOPE)
