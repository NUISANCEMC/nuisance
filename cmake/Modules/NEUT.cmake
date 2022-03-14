find_program(NEUTCONFIG NAMES neut-config)

if(NOT "${NEUTCONFIG}x" STREQUAL "NEUTCONFIG-NOTFOUNDx")
  cmessage(STATUS "Found neut-config, using it to determine configuration.")

  execute_process (COMMAND neut-config --version 
    OUTPUT_VARIABLE NEUT_CONFIG_VERSION 
    OUTPUT_STRIP_TRAILING_WHITESPACE)

    if(NEUT_CONFIG_VERSION VERSION_LESS 5.5.0)
      include(FindNEUT541)
    else()
      find_package(NEUT)
    endif()
else()

  include(CMessage)

  EnsureVarOrEnvSet(NEUT_VERSION NEUT_VERSION)
  EnsureVarOrEnvSet(NEUT_ROOT NEUT_ROOT)
  EnsureVarOrEnvSet(CERN CERN)
  EnsureVarOrEnvSet(CERN_LEVEL CERN_LEVEL)
  
  if("${NEUT_ROOT}x" STREQUAL "x")
    cmessage(STATUS "Variable NEUT_ROOT is not defined and could not find neut-config, disabling NEUT support")
    set(NEUT_FOUND FALSE)
  else()

    if("${CERN}x" STREQUAL "x")
      cmessage(FATAL_ERROR "Variable CERN is not defined. Please export environment variable CERN or configure with -DCERN=/path/to/CERNLIB. This must be set to point to a prebuilt CERNLIB instance.")
    endif()

    if("${CERN_LEVEL}x" STREQUAL "x")
      cmessage(FATAL_ERROR "Variable CERN_LEVEL is not defined. Please export environment variable CERN_LEVEL or configure with -DCERN_LEVEL=XXXX (likely to be 2005).")
    endif()

    if("${NEUT_VERSION}x" STREQUAL "x")
      cmessage(FATAL_ERROR "Variable NEUT_VERSION is not defined. Please export environment variable NEUT_VERSION or configure with -DNEUT_VERSION=5.X.Y")
    endif()

    if(NEUT_VERSION VERSION_LESS 5.4.0)
      include(FindNEUT53X)
    else()
      include(FindNEUT540)
    endif()
    
  endif()

endif()