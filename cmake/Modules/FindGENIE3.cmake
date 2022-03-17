if(NOT TARGET GENIE::All)

EnsureVarOrEnvSet(GENIE_REWEIGHT GENIE_REWEIGHT)

set(GENIE_REWEIGHT_FOUND FALSE)
if("${GENIE_REWEIGHT}" STREQUAL "GENIE_REWEIGHT-NOTFOUND")
  cmessage(STATUS "GENIE_REWEIGHT environment variable is not defined, assuming no GENIE_REWEIGHT build")
endif()

include(FindPackageHandleStandardArgs)

find_path(GENIE_INC_DIR
  NAMES Framework/GHEP/GHepRecord.h
  PATHS ${GENIE}/include/GENIE)

find_path(GENIE_LIB_DIR
  NAMES libGFwGHEP.so
  PATHS ${GENIE}/lib)

find_package_handle_standard_args(GENIE
  REQUIRED_VARS 
    GENIE 
    GENIE_INC_DIR 
    GENIE_LIB_DIR
    LOG4CPP_INC_DIR
    LOG4CPP_LIB_DIR
    PYTHIA6_LIB_DIR
  VERSION_VAR GENIE_VERSION
)

if(GENIE_FOUND)

  include(ParseConfigApps)

  GetLibs(CONFIG_APP genie-config ARGS --libs OUTPUT_VARIABLE GENIE_LIBS)

  string(REGEX REPLACE "\([0-9]\)\.\([0-9]*\).*" "\\1\\2" GENIE_SINGLE_VERSION ${GENIE_VERSION})
  LIST(APPEND GENIE_DEFINES -DGENIE_ENABLED -DGENIE3_API_ENABLED -DGENIE_VERSION=${GENIE_SINGLE_VERSION})

  if(GENIEReWeight_FOUND)
    set(GENIEReWeight_ENABLED TRUE)
    foreach(RWLIBNAME GRwClc GRwFwk GRwIO)
      LIST(REMOVE_ITEM GENIE_LIBS ${RWLIBNAME})
      if(EXISTS ${GENIE_LIB_DIR}/lib${RWLIBNAME}.so)
        LIST(APPEND GENIE_LIBS ${RWLIBNAME})
      else()
        cmessage(WARNING "Failed to find expected reweight library: ${GENIE_LIB_DIR}/lib${RWLIBNAME}.so disabling GENIE3 reweight.")
        set(GENIEReWeight_ENABLED FALSE)
      endif()
    endforeach()
  
    if(EXISTS ${GENIE_INC_DIR}/RwCalculators/GReWeightXSecMEC.h)
      SET(GENIE3_XSECMEC_ENABLED TRUE)
    endif()

  endif()

  #duplicate because CMake gets its grubby mitts on repeated -Wl,--start-group options
  SET(GENIE_LIBS "-Wl,--no-as-needed;${GENIE_LIBS};${GENIE_LIBS};-Wl,--as-needed")

  cmessage(STATUS "GENIE 3 (Version: ${GENIE_VERSION})")
  cmessage(STATUS "                GENIE: ${GENIE}")
  cmessage(STATUS "       GENIE_REWEIGHT: ${GENIE_REWEIGHT}")
  cmessage(STATUS "              OPTIONS: GENIEReWeight: ${GENIEReWeight_ENABLED}, XSecMECReWeight: ${GENIE3_XSECMEC_ENABLED}")
  cmessage(STATUS " GENIE_SINGLE_VERSION: ${GENIE_SINGLE_VERSION}")
  cmessage(STATUS "        GENIE DEFINES: ${GENIE_DEFINES}")
  cmessage(STATUS "       GENIE INC_DIRS: ${GENIE_INC_DIR}")
  cmessage(STATUS "       GENIE LIB_DIRS: ${GENIE_LIB_DIR}")
  cmessage(STATUS "           GENIE LIBS: ${GENIE_LIBS}")
  cmessage(STATUS "        DEPS INC_DIRS: ${LOG4CPP_INC_DIR}")
  cmessage(STATUS "        DEPS LIB_DIRS: ${LOG4CPP_LIB_DIR};${PYTHIA6_LIB_DIR}")
  cmessage(STATUS "            DEPS LIBS: Pythia6;log4cpp;LibXml2::LibXml2;GSL::gsl")

  if(NOT TARGET GENIE::All)
    add_library(GENIE::All INTERFACE IMPORTED)
    set_target_properties(GENIE::All PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${GENIE_INC_DIR};${LOG4CPP_INC_DIR}"
        INTERFACE_COMPILE_OPTIONS "${GENIE_DEFINES}"
        INTERFACE_LINK_DIRECTORIES "${GENIE_LIB_DIR};${LOG4CPP_LIB_DIR};${PYTHIA6_LIB_DIR}"
        INTERFACE_LINK_LIBRARIES "${GENIE_LIBS};EGPythia6;Pythia6;log4cpp;LibXml2::LibXml2;GSL::gsl"
    )
  endif()

  set(GENIE3_API_ENABLED TRUE)
endif()

endif()
