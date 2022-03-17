set(GENIEReWeight_ENABLED FALSE)

set(GENIE2_XSECEMPMEC_ENABLED FALSE)

set(GENIE3_API_ENABLED FALSE)
set(GENIE3_XSECMEC_ENABLED FALSE)

include(CMessage)

find_program(GENIECONFIG NAMES genie-config)
if("${GENIECONFIG}" STREQUAL "GENIECONFIG-NOTFOUND")
  cmessage(STATUS "Could not find genie-config, assuming no GENIE build")
  SET(GENIE_FOUND FALSE)
  return()
endif()

EnsureVarOrEnvSet(GENIE GENIE)

if("${GENIE}" STREQUAL "GENIE-NOTFOUND")
  cmessage(STATUS "GENIE environment variable is not defined, assuming no GENIE build")
  SET(GENIE_FOUND FALSE)
  return()
endif()

#Check and set up GENIE Dependencies

###### log4cpp
EnsureVarOrEnvSet(LOG4CPP_INC LOG4CPP_INC)
if("${LOG4CPP_INC}" STREQUAL "LOG4CPP_INC-NOTFOUND")
  find_program(LOG4CPPCONFIG NAMES log4cpp-config)
  if("${LOG4CPPCONFIG}" STREQUAL "LOG4CPPCONFIG-NOTFOUND")
    cmessage(WARNING "Could not find log4cpp-config, but LOG4CPP_INC is not set in the environment")
    SET(GENIE_FOUND FALSE)
    return()
  endif()

  execute_process (COMMAND log4cpp-config --prefix 
                    OUTPUT_VARIABLE LOG4CPP_PREFIX 
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(LOG4CPP_INC ${LOG4CPP_PREFIX}/include)
  cmessage(STATUS "Used log4cpp-config to resolve LOG4CPP_INC=${LOG4CPP_INC}")
endif()

find_path(LOG4CPP_INC_DIR
  NAMES log4cpp/config.h
  PATHS ${LOG4CPP_INC})

if("${LOG4CPP_INC_DIR}" STREQUAL "LOG4CPP_INC_DIR-NOTFOUND")
  cmessage(WARNING "When configuring GENIE with LOG4CPP_INC=\"${LOG4CPP_INC}\", failed find required file \"log4cpp/config.h\".")
  SET(GENIE_FOUND FALSE)
  return()
endif()

EnsureVarOrEnvSet(LOG4CPP_LIB LOG4CPP_LIB)
if("${LOG4CPP_LIB}" STREQUAL "LOG4CPP_LIB-NOTFOUND")
  find_program(LOG4CPPCONFIG NAMES log4cpp-config)
  if("${LOG4CPPCONFIG}" STREQUAL "LOG4CPPCONFIG-NOTFOUND")
    cmessage(WARNING "Could not find log4cpp-config, but LOG4CPP_LIB is not set in the environment")
    SET(GENIE_FOUND FALSE)
    return()
  endif()

  GetLibDir(CONFIG_APP log4cpp-config ARGS --libs OUTPUT_VARIABLE LOG4CPP_LIB)
  cmessage(STATUS "Used log4cpp-config to resolve LOG4CPP_LIB=${LOG4CPP_LIB}")
endif()

find_path(LOG4CPP_LIB_DIR
  NAMES liblog4cpp.so
  PATHS ${LOG4CPP_LIB})

if("${LOG4CPP_LIB_DIR}" STREQUAL "LOG4CPP_LIB_DIR-NOTFOUND")
  cmessage(WARNING "When configuring GENIE with LOG4CPP_LIB=\"${LOG4CPP_LIB}\", failed find required file \"liblog4cpp.so\".")
  SET(GENIE_FOUND FALSE)
  return()
endif()

###### LibXml2
find_package(LibXml2)
if(NOT LibXml2_FOUND)
  cmessage(STATUS "Attempting to use environment to find libxml2")
  EnsureVarOrEnvSet(LIBXML2_INC LIBXML2_INC)
  EnsureVarOrEnvSet(LIBXML2_LIB LIBXML2_LIB)

  if("${LIBXML2_INC}" STREQUAL "LIBXML2_INC-NOTFOUND")
    cmessage(STATUS "Variable LIBXML2_INC is not defined. Please export environment variable LIBXML2_INC or configure with -DLIBXML2_INC=/path/to/libxml2/includes.")
    SET(GENIE_FOUND FALSE)
    return()
  endif()

  if("${LIBXML2_LIB}" STREQUAL "LIBXML2_LIB-NOTFOUND")
    cmessage(STATUS "Variable LIBXML2_LIB is not defined. Please export environment variable LIBXML2_LIB or configure with -DLIBXML2_LIB=/path/to/libxml2/lib.")
    SET(GENIE_FOUND FALSE)
    return()
  endif()

  find_path(LIBXML2_INC_DIR
    NAMES libxml/parser.h
    PATHS ${LIBXML2_INC})

  if("${LIBXML2_INC_DIR}" STREQUAL "LIBXML2_INC_DIR-NOTFOUND")
    cmessage(STATUS "When configuring GENIE with LIBXML2_INC=\"${LIBXML2_INC}\", failed find required file \"libxml/parser.h\".")
    SET(GENIE_FOUND FALSE)
    return()
  endif()

  find_path(LIBXML2_LIB_DIR
    NAMES libxml2.so
    PATHS ${LIBXML2_LIB})

  if("${LIBXML2_LIB_DIR}" STREQUAL "LIBXML2_LIB_DIR-NOTFOUND")
    cmessage(STATUS "When configuring GENIE with LIBXML2_LIB=\"${LIBXML2_LIB}\", failed find required file \"llibxml2.so\".")
    SET(GENIE_FOUND FALSE)
    return()
  endif()

  if(NOT TARGET LibXml2::LibXml2)
    add_library(LibXml2::LibXml2 INTERFACE IMPORTED)
    set_target_properties(LibXml2::LibXml2 PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${LIBXML2_INC}
        INTERFACE_LINK_DIRECTORIES ${LIBXML2_LIB}
        INTERFACE_LINK_LIBRARIES libxml2.so
      )
  endif()
endif()

###### LibXml2
find_package(LibXml2)
if(NOT LibXml2_FOUND)
  cmessage(STATUS "Attempting to use environment to find libxml2")
  EnsureVarOrEnvSet(LIBXML2_INC LIBXML2_INC)
  EnsureVarOrEnvSet(LIBXML2_LIB LIBXML2_LIB)

  if("${LIBXML2_INC}" STREQUAL "LIBXML2_INC-NOTFOUND")
    cmessage(STATUS "Variable LIBXML2_INC is not defined. Please export environment variable LIBXML2_INC or configure with -DLIBXML2_INC=/path/to/libxml2/includes.")
    SET(GENIE_FOUND FALSE)
    return()
  endif()

  if("${LIBXML2_LIB}" STREQUAL "LIBXML2_LIB-NOTFOUND")
    cmessage(STATUS "Variable LIBXML2_LIB is not defined. Please export environment variable LIBXML2_LIB or configure with -DLIBXML2_LIB=/path/to/libxml2/lib.")
    SET(GENIE_FOUND FALSE)
    return()
  endif()

  find_path(LIBXML2_INC_DIR
    NAMES libxml/parser.h
    PATHS ${LIBXML2_INC})

  if("${LIBXML2_INC_DIR}" STREQUAL "LIBXML2_INC_DIR-NOTFOUND")
    cmessage(STATUS "When configuring GENIE with LIBXML2_INC=\"${LIBXML2_INC}\", failed find required file \"libxml/parser.h\".")
    SET(GENIE_FOUND FALSE)
    return()
  endif()

  find_path(LIBXML2_LIB_DIR
    NAMES libxml2.so
    PATHS ${LIBXML2_LIB})

  if("${LIBXML2_LIB_DIR}" STREQUAL "LIBXML2_LIB_DIR-NOTFOUND")
    cmessage(STATUS "When configuring GENIE with LIBXML2_LIB=\"${LIBXML2_LIB}\", failed find required file \"libxml2.so\".")
    SET(GENIE_FOUND FALSE)
    return()
  endif()

  if(NOT TARGET LibXml2::LibXml2)
    add_library(LibXml2::LibXml2 INTERFACE IMPORTED)
    set_target_properties(LibXml2::LibXml2 PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${LIBXML2_INC}
        INTERFACE_LINK_DIRECTORIES ${LIBXML2_LIB}
        INTERFACE_LINK_LIBRARIES libxml2.so
      )
  endif()
endif()

find_package(GSL)
if(NOT GSL_FOUND)
  cmessage(STATUS "Attempting to use environment to find GSL")
  EnsureVarOrEnvSet(GSL_INC GSL_INC)
  EnsureVarOrEnvSet(GSL_LIB GSL_LIB)

  if("${GSL_INC}" STREQUAL "GSL_INC-NOTFOUND")
    cmessage(STATUS "Variable GSL_INC is not defined. Please export environment variable GSL_INC or configure with -DGSL_INC=/path/to/GSL/includes.")
    SET(GENIE_FOUND FALSE)
    return()
  endif()

  if("${GSL_LIB}" STREQUAL "GSL_LIB-NOTFOUND")
    cmessage(STATUS "Variable GSL_LIB is not defined. Please export environment variable GSL_LIB or configure with -DGSL_LIB=/path/to/GSL/lib.")
    SET(GENIE_FOUND FALSE)
    return()
  endif()

  find_path(GSL_INC_DIR
    NAMES gsl/gsl_version.h
    PATHS ${GSL_INC})

  if("${GSL_INC_DIR}" STREQUAL "GSL_INC_DIR-NOTFOUND")
    cmessage(STATUS "When configuring GENIE with GSL_INC=\"${GSL_INC}\", failed find required file \"libxml/parser.h\".")
    SET(GENIE_FOUND FALSE)
    return()
  endif()

  find_path(GSL_LIB_DIR
    NAMES libgsl.so
    PATHS ${GSL_LIB})

  if("${GSL_LIB_DIR}" STREQUAL "GSL_LIB_DIR-NOTFOUND")
    cmessage(STATUS "When configuring GENIE with GSL_LIB=\"${GSL_LIB}\", failed find required file \"libgsl.so\".")
    SET(GENIE_FOUND FALSE)
    return()
  endif()

  if(NOT TARGET GSL::gsl)
    add_library(GSL::gsl INTERFACE IMPORTED)
    set_target_properties(GSL::gsl PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${GSL_INC}
        INTERFACE_LINK_DIRECTORIES ${GSL_LIB}
        INTERFACE_LINK_LIBRARIES "gsl;gslcblas;m"
      )
  endif()
endif()

EnsureVarOrEnvSet(PYTHIA6 PYTHIA6)
EnsureVarOrEnvSet(PYTHIA6_LIB_DIR PYTHIA6_LIB_DIR)

if("${PYTHIA6}" STREQUAL "PYTHIA6-NOTFOUND" AND "${PYTHIA6_LIB_DIR}" STREQUAL "PYTHIA6_LIB_DIR-NOTFOUND")
  cmessage(STATUS "Variable PYTHIA6 (or PYTHIA6_LIB_DIR) is not defined. Please export environment variable PYTHIA6 (or PYTHIA6_LIB_DIR) or configure with -DPYTHIA6=/path/to/log4cpp/includes.")
  SET(GENIE_FOUND FALSE)
  return()
endif()

find_path(PYTHIA6_LIB_DIR
NAMES libPythia6.so
PATHS ${PYTHIA6} ${PYTHIA6_LIB_DIR})

if("${PYTHIA6_LIB_DIR}" STREQUAL "PYTHIA6_LIB_DIR-NOTFOUND")
  cmessage(STATUS "When configuring GENIE failed find required file \"libPythia6.so\" (Search paths: PYTHIA6=\"${PYTHIA6}\", PYTHIA6_LIB_DIR=\"${PYTHIA6_LIB_DIR}\").")
  SET(GENIE_FOUND FALSE)
  return()
endif()

execute_process (COMMAND cat ${GENIE}/VERSION OUTPUT_VARIABLE GENIE_VERSION
      OUTPUT_STRIP_TRAILING_WHITESPACE)

if(GENIE_VERSION VERSION_GREATER 2.99.99)
	include(FindGENIE3)
else()
	include(FindGENIE2)
endif()

if(GENIE_FOUND)
	target_link_libraries(GeneratorCompileDependencies INTERFACE GENIE::All)
endif()