if(NOT TARGET NEUT::IO OR NOT TARGET NEUT::Generator OR NOT TARGET NEUT::ReWeight)

cmake_minimum_required (VERSION 3.14 FATAL_ERROR)
# This will define the following variables
#
#    NEUT_FOUND
#
# and the following imported targets
#
#    NEUT::IO
#    NEUT::Generator
#    NEUT::ReWeight
#

include(CMessage)

find_path(NEUT_INCLUDE_DIR
  NAMES necardC.h
  PATHS ${NEUT_ROOT}/include
)

find_path(NEUT_LIB_DIR
  NAMES libneutcore_5.4.0.a
  PATHS ${NEUT_ROOT}/lib/Linux_pc/
)

find_path(CERN_INCLUDE_DIR
  NAMES jetset74/pilot.h
  PATHS ${CERN}/${CERN_LEVEL}/include
)

find_path(CERN_LIB_DIR
  NAMES libjetset74.a
  PATHS ${CERN}/${CERN_LEVEL}/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NEUT
    REQUIRED_VARS 
      NEUT_INCLUDE_DIR
      NEUT_LIB_DIR
      CERN_INCLUDE_DIR
      CERN_LIB_DIR
    VERSION_VAR NEUT_VERSION
)

if(NEUT_FOUND)

  FILE(GLOB NEUT_LIB_LIST ${NEUT_LIB_DIR}/*_5.4*.a)

  foreach(item ${NEUT_LIB_LIST})
    get_filename_component(libLIBNAME ${item} NAME_WLE)
    string(REGEX REPLACE ^lib "" LIBNAME ${libLIBNAME})
    LIST(APPEND NEUT_LIBS ${LIBNAME})
  endforeach()

  FILE(GLOB NEUT_CLASS_SO_LIST ${NEUT_ROOT}/src/neutclass/*.so)

  include(CheckCXXCompilerFlag)
  CHECK_CXX_COMPILER_FLAG(-Wl,--allow-multiple-definition 
    COMPILER_SUPPORTS_ALLOW_MULTIPLE_DEFINITION)

  IF(COMPILER_SUPPORTS_ALLOW_MULTIPLE_DEFINITION)
    LIST(APPEND NEUT_LINK_OPTIONS -Wl,--allow-multiple-definition)
  ENDIF()

  CHECK_CXX_COMPILER_FLAG(-no-pie COMPILER_SUPPORTS_NO_PIE)
  CHECK_CXX_COMPILER_FLAG(-fno-pie COMPILER_SUPPORTS_FNO_PIE)
  CHECK_CXX_COMPILER_FLAG(-fno-PIE COMPILER_SUPPORTS_FNO_PIE_CAP)
  if(COMPILER_SUPPORTS_NO_PIE)
    LIST(APPEND NEUT_EXE_FLAGS -no-pie)
  elseif(COMPILER_SUPPORTS_FNO_PIE)
    LIST(APPEND NEUT_EXE_FLAGS -fno-pie)
  elseif(COMPILER_SUPPOERTS_FNO_PIE_CAP)
    LIST(APPEND NEUT_EXE_FLAGS -fno-PIE)
  else()
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no -fno-pie support. Please use a different C++ compiler.")
  endif()

  #This horrow show lets us add the exe-flag only for executable targets.
  foreach(opt ${NEUT_EXE_FLAGS})
    LIST(APPEND NEUT_LINK_OPTIONS $<IF:$<STREQUAL:"$<TARGET_PROPERTY:TYPE>","EXECUTABLE">,${opt},>)
  endforeach()

  LIST(APPEND NEUT_DEFINES -DNEUT_ENABLED -DNEUT_VERSION=${NEUT_VERSION})

  LIST(APPEND NEUT_CERN_LIBS 
    jetset74
    pdflib804
    mathlib
    packlib
    pawlib)

  cmessage(STATUS "NEUT Found: ${NEUT_PREFIX} ")
  cmessage(STATUS "    NEUT_INCLUDE_DIR: ${NEUT_INCLUDE_DIR}")
  cmessage(STATUS "    NEUT_DEFINES: ${NEUT_DEFINES}")
  cmessage(STATUS "    NEUT_LIB_DIR: ${NEUT_LIB_DIR}")
  cmessage(STATUS "    NEUT_LIBS: ${NEUT_LIBS}")
  cmessage(STATUS "    NEUT_CLASS_SOS: ${NEUT_CLASS_SO_LIST}")
  cmessage(STATUS "    NEUT_LINK_OPTIONS: ${NEUT_LINK_OPTIONS}")
  cmessage(STATUS "    CERN_INCLUDE_DIR: ${CERN_INCLUDE_DIR}")
  cmessage(STATUS "    CERN_LIB_DIR: ${CERN_LIB_DIR}")
  cmessage(STATUS "    NEUT_CERN_LIBS: ${NEUT_CERN_LIBS}")

  if(NOT TARGET NEUT::IO)
      add_library(NEUT::IO INTERFACE IMPORTED)
      set_target_properties(NEUT::IO PROPERTIES
          INTERFACE_INCLUDE_DIRECTORIES "${NEUT_INCLUDE_DIR};${CERN_INCLUDE_DIR}"
          INTERFACE_COMPILE_OPTIONS "${NEUT_DEFINES}"
          INTERFACE_LINK_DIRECTORIES "${NEUT_LIB_DIR};${CERN_LIB_DIR}"
          INTERFACE_LINK_LIBRARIES "${NEUT_CLASS_SO_LIST};${NEUT_LIBS};ROOT::ROOT;${NEUT_CERN_LINKFLAGS};gfortran"
          INTERFACE_LINK_OPTIONS "${NEUT_LINK_OPTIONS}"
      )
  endif()

  if(NOT TARGET NEUT::Generator)
      add_library(NEUT::Generator INTERFACE IMPORTED)
      set_target_properties(NEUT::Generator PROPERTIES
          INTERFACE_LINK_LIBRARIES "NEUT::IO"
      )
  endif()

    if(NOT TARGET NEUT::ReWeight)
      add_library(NEUT::ReWeight INTERFACE IMPORTED)
      set_target_properties(NEUT::ReWeight PROPERTIES
          INTERFACE_LINK_LIBRARIES "NReWeight;NEUT::Generator"
      )
  endif()

endif()

endif() # Only run this whole file if it hasn't been run already.