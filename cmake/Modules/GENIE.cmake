find_program(GENIECONFIG NAMES genie-config)

set(GENIEReWeight_ENABLED FALSE)

set(GENIE2_XSECEMPMEC_ENABLED FALSE)

set(GENIE3_API_ENABLED FALSE)
set(GENIE3_XSECMEC_ENABLED FALSE)

include(CMessage)

if("${GENIECONFIG}" STREQUAL "GENIECONFIG-NOTFOUND")
  cmessage(STATUS "Could not find genie-config, assuming no GENIE build")
  SET(GENIE_FOUND FALSE)
  SET(GENIE_ENABLED FALSE)
  return()
endif()

EnsureVarOrEnvSet(GENIE GENIE)

if("${GENIE}" STREQUAL "GENIE-NOTFOUND")
  cmessage(STATUS "GENIE environment variable is not defined, assuming no GENIE build")
  SET(GENIE_FOUND FALSE)
  SET(GENIE_ENABLED FALSE)
  return()
endif()

  EnsureVarOrEnvSet(LOG4CPP_INC LOG4CPP_INC)
  EnsureVarOrEnvSet(LOG4CPP_LIB LOG4CPP_LIB)
  EnsureVarOrEnvSet(LIBXML2_INC LIBXML2_INC)
  EnsureVarOrEnvSet(LIBXML2_LIB LIBXML2_LIB)
  EnsureVarOrEnvSet(PYTHIA6 PYTHIA6)
  EnsureVarOrEnvSet(PYTHIA6_LIB_DIR PYTHIA6_LIB_DIR)

  if("${LOG4CPP_INC}" STREQUAL "LOG4CPP_INC-NOTFOUND")
    cmessage(FATAL_ERROR "Variable LOG4CPP_INC is not defined. Please export environment variable LOG4CPP_INC or configure with -DLOG4CPP_INC=/path/to/log4cpp/includes.")
  endif()
  if("${LOG4CPP_LIB}" STREQUAL "LOG4CPP_LIB-NOTFOUND")
    cmessage(FATAL_ERROR "Variable LOG4CPP_LIB is not defined. Please export environment variable LOG4CPP_LIB or configure with -DLOG4CPP_LIB=/path/to/log4cpp/lib.")
  endif()

  if("${LIBXML2_INC}" STREQUAL "LIBXML2_INC-NOTFOUND")
    cmessage(FATAL_ERROR "Variable LIBXML2_INC is not defined. Please export environment variable LIBXML2_INC or configure with -DLIBXML2_INC=/path/to/libxml2/includes.")
  endif()
  if("${LIBXML2_LIB}" STREQUAL "LIBXML2_LIB-NOTFOUND")
    cmessage(FATAL_ERROR "Variable LIBXML2_LIB is not defined. Please export environment variable LIBXML2_LIB or configure with -DLIBXML2_LIB=/path/to/libxml2/lib.")
  endif()

  if("${PYTHIA6}" STREQUAL "PYTHIA6-NOTFOUND" AND "${PYTHIA6_LIB_DIR}" STREQUAL "PYTHIA6_LIB_DIR-NOTFOUND")
    cmessage(FATAL_ERROR "Variable PYTHIA6 (or PYTHIA6_LIB_DIR) is not defined. Please export environment variable PYTHIA6 (or PYTHIA6_LIB_DIR) or configure with -DPYTHIA6=/path/to/log4cpp/includes.")
  endif()

execute_process (COMMAND cat ${GENIE}/VERSION OUTPUT_VARIABLE GENIE_VERSION
      OUTPUT_STRIP_TRAILING_WHITESPACE)

if(GENIE_VERSION VERSION_GREATER 2.99.99)
	include(FindGENIE3)
else()
	include(FindGENIE2)
endif()

if(NOT GENIE_FOUND)
  if(GENIE_REQUIRED)
    cmessage(FATAL_ERROR "GENIE was explicitly enabled but cannot be found.")
  endif()
  SET(GENIE_ENABLED FALSE)
else()
	target_link_libraries(GeneratorCompileDependencies INTERFACE GENIE::All)
endif()