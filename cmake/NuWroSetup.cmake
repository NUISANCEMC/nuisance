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

if(NOT NUWRO_INPUT_FILE STREQUAL "")

  if(NOT EXISTS ${NUWRO_INPUT_FILE})
    cmessage(FATAL_ERROR "Expected -DNUWRO_INPUT_FILE to point to a valid input file. Cannot find: '${NUWRO_INPUT_FILE}'")
  endif()

  if(CMAKE_BUILD_TYPE MATCHES DEBUG)
    BuildROOTProject(NuWro_event1 ${NUWRO_INPUT_FILE} "event,vec,vect,particle,flags,params,line" STATIC)
    SET(ROOTLIBNAME "libNuWro_event1.a")
  else(CMAKE_BUILD_TYPE MATCHES RELEASE)
    BuildROOTProject(NuWro_event1 ${NUWRO_INPUT_FILE} "event,vec,vect,particle,flags,params,line" SHARED)
    SET(ROOTLIBNAME "libNuWro_event1.so")
  endif()

  ADD_CUSTOM_TARGET(NuWro_event1HeaderLink ALL
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_BINARY_DIR}/NuWro_event1/event.h
    ${CMAKE_BINARY_DIR}/NuWro_event1/event1.h
    DEPENDS NuWro_event1)

  LIST(APPEND EXTRA_CXX_FLAGS -D__NUWRO_ENABLED__)

  LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES ${CMAKE_BINARY_DIR}/NuWro_event1)

  LIST(APPEND EXTRA_LINK_DIRS ${CMAKE_BINARY_DIR})

  LIST(APPEND EXTRA_LIBS NuWro_event1)

  LIST(APPEND PROJECTWIDE_EXTRA_DEPENDENCIES NuWro_event1HeaderLink )

  install(TARGETS NuWro_event1 DESTINATION lib)

  SET(NUWRO_BUILT_FROM_FILE TRUE)

else()

  if(NUWRO STREQUAL "")
    cmessage(FATAL_ERROR "Variable NUWRO is not defined. "
      "This must be set to point to a prebuilt NuWro instance.")
  endif()

  # If you are using a version of NuWro without reweighting use this to compile.
  if(USE_NuWro_RW)

    if(NUWRO_INC STREQUAL "")
      cmessage(FATAL_ERROR "Variable NUWRO_INC is not defined. "
        "This must be set to point to an installed NuWro instance.")
    endif()

    LIST(APPEND EXTRA_CXX_FLAGS -D__NUWRO_ENABLED__ -D__NUWRO_REWEIGHT_ENABLED__)

    if(USE_NuWro_SRW_Event)
          LIST(APPEND EXTRA_CXX_FLAGS -D__USE_NUWRO_SRW_EVENTS__)
    endif()

    LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES
      ${NUWRO}/src
      ${NUWRO}/src/reweight
      ${NUWRO_INC}/nuwro)

    #LIST(APPEND EXTRA_LINK_DIRS ${NUWRO}/build/${CMAKE_SYSTEM_NAME}/lib)
    LIST(APPEND EXTRA_LIBS reweight event)

  else ()
    LIST(APPEND EXTRA_CXX_FLAGS -D__NUWRO_ENABLED__)

    LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES ${NUWRO}/src)

    if(NOT EXISTS ${NUWRO}/bin/event1.so)
      if(EXISTS ${NUWRO}/build/${CMAKE_SYSTEM_NAME}/lib)

        if(NUWRO_INC STREQUAL "")
          cmessage(FATAL_ERROR "Variable NUWRO_INC is not defined. "
            "This must be set to point to an installed NuWro instance.")
        endif()

        LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES ${NUWRO_INC}/nuwro)

        #LIST(APPEND EXTRA_LINK_DIRS ${NUWRO}/build/${CMAKE_SYSTEM_NAME}/lib)
        LIST(APPEND EXTRA_LIBS -Wl,--no-as-needed event -Wl,--as-needed)
      else()
        cmessage(FATAL_ERROR "Expected to find the NuWro event library in: ${NUWRO}/bin/event1.so, or if using NuWro with reweight support: ${NUWRO}/build/${CMAKE_SYSTEM_NAME}/lib/libevent.a. Is NuWro built?")
      endif()
    else()
      LIST(APPEND EXTRA_SHAREDOBJS -Wl,--no-as-needed ${NUWRO}/bin/event1.so -Wl,--as-needed)
    endif()

  endif()

  set(NEED_PYTHIA6 TRUE)
  set(NEED_ROOTPYTHIA6 TRUE)
endif()
