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

if(DEFINED BUILD_NuWro_FROM_FILE)

  if(NOT EXISTS ${BUILD_NuWro_FROM_FILE})
    cmessage(FATAL_ERROR "Expected -DBUILD_NuWro_FROM_FILE to point to a valid input file. Cannot find: '${BUILD_NuWro_FROM_FILE}'")
  endif()

  if(CMAKE_BUILD_TYPE MATCHES DEBUG)
    BuildROOTProject(NuWro_event1 ${BUILD_NuWro_FROM_FILE} "event,vec,vect,particle,flags,params,line" STATIC)
    SET(ROOTLIBNAME "libNuWro_event1.a")
  else(CMAKE_BUILD_TYPE MATCHES RELEASE)
    BuildROOTProject(NuWro_event1 ${BUILD_NuWro_FROM_FILE} "event,vec,vect,particle,flags,params,line" SHARED)
    SET(ROOTLIBNAME "libNuWro_event1.so")
  endif()

  ADD_CUSTOM_TARGET(NuWro_event1HeaderLink ALL
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_BINARY_DIR}/NuWro_event1/event.h
    ${CMAKE_BINARY_DIR}/NuWro_event1/event1.h
    DEPENDS NuWro_event1)

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__NUWRO_ENABLED__ ")

  LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES ${CMAKE_BINARY_DIR}/NuWro_event1)

  set(RWENGINE_LINKER_FLAGS "${RWENGINE_LINKER_FLAGS} ${CMAKE_CURRENT_BINARY_DIR}/${ROOTLIBNAME}")

  LIST(APPEND PROJECTWIDE_EXTRA_DEPENDENCIES NuWro_event1HeaderLink)

  install(TARGETS NuWro_event1 DESTINATION lib)

  SET(NUWRO_BUILT_FROM_FILE 1)
else()
  SET(NUWRO_BUILT_FROM_FILE 0)

  if(NOT DEFINED ENV{NUWRO})

    cmessage(FATAL_ERROR "Environment variable NUWRO is not defined. "
      "This must be set to point to a prebuilt NuWro instance.")

  endif()

  set(NUWRO $ENV{NUWRO})

  if(NOT DEFINED NO_NuWro_RW)
    cmessage(STATUS "NO_NuWro_RW not defined, setting to false")
    SET(NO_NuWro_RW 1)
  endif()

# If you are using a version of NuWro without reweighting use this to compile.
  if(DEFINED NO_NuWro_RW AND NO_NuWro_RW)

    cmessage(STATUS "Compiling NuWro WITHOUT ReWeight")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__NUWRO_ENABLED__ ")

    LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES ${NUWRO}/src)

    set(RWENGINE_LINKER_FLAGS "${RWENGINE_LINKER_FLAGS} ${NUWRO}/bin/event1.so")

  else()

    cmessage(STATUS "Compiling NuWro WITH ReWeight")

    if(DEFINED USE_EXP AND USE_EXP)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNW_READHISTFROMINP")
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__NUWRO_ENABLED__ -D__NUWRO_REWEIGHT_ENABLED__ ")

    LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES  ${NUWRO}/src ${NUWRO}/src/reweight ${NUWRO}/build/src)

    set(RWENGINE_LINKER_FLAGS "${RWENGINE_LINKER_FLAGS} -L${NUWRO}/build/${CMAKE_SYSTEM_NAME}/lib -lreweight -levent")

  endif()

  set(NEED_PYTHIA6 TRUE)
  set(NEED_ROOTPYTHIA6 TRUE)
endif()

