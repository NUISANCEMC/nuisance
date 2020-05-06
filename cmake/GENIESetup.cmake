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

# TODO
# check system for libxml2
# check whether we need the includes
# check if we can use a subset of the GENIE libraries

include(${CMAKE_SOURCE_DIR}/cmake/parseConfigApp.cmake)
################################################################################
#                            Check Dependencies
################################################################################

#################################  GENIE  ######################################
if(GENIE STREQUAL "")
  cmessage(FATAL_ERROR "Variable GENIE is not defined. "
    "The location of a pre-built GENIE install must be defined either as"
    " $ cmake -DGENIE=/path/to/GENIE or as an environment variable"
    " $ export GENIE=/path/to/GENIE")
endif()

execute_process(COMMAND genie-config --version
OUTPUT_VARIABLE GENIE_VER OUTPUT_STRIP_TRAILING_WHITESPACE)
cmessage(STATUS "genie_ver: ${GENIE_VER}")
if(GENIE_VER VERSION_GREATER 3.0.0)
  set(GENIE_POST_R3 1)
  string(REPLACE "." "" GENIE_VERSION ${GENIE_VER})
  cmessage(STATUS "set genie_post_r3")
endif()

if(NOT GENIE_POST_R3)
  LIST(APPEND EXTRA_CXX_FLAGS -DGENIE_PRE_R3)
  cmessage(STATUS "setting genie_pre_r3 ${EXTRA_CXX_FLAGS}")
  if(GENIE_EMPMEC_REWEIGHT)
    cmessage(STATUS "Enable EMPMEC dials")
    LIST(APPEND EXTRA_CXX_FLAGS -D__GENIE_EMP_MECRW_ENABLED)
  endif()
else()
  cmessage(STATUS "Enable EMPMEC dials")
  LIST(APPEND EXTRA_CXX_FLAGS -D__GENIE_EMP_MECRW_ENABLED)
  if(USE_GENIE_XSECMEC)
    LIST(APPEND EXTRA_CXX_FLAGS -DUSE_GENIE_XSECMEC)
  endif()
endif()

execute_process (COMMAND genie-config
  --topsrcdir OUTPUT_VARIABLE GENIE_INCLUDES_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)

#Allows for external override in the case where genie-config lies.
if(NOT DEFINED GENIE_LIB_DIR OR GENIE_LIB_DIR STREQUAL "")
  #This looks like it should call libdir, but it strips the argument with -L from the response of --libs
  GETLIBDIRS(genie-config --libs GENIE_LIB_DIR)
endif()
GETLIBS(genie-config --libs GENIE_LIBS)

cmessage(STATUS "GENIE version : ${GENIE_VERSION}")
cmessage(STATUS "GENIE libdir  : ${GENIE_LIB_DIR}")
cmessage(STATUS "GENIE libs    : ${GENIE_LIBS}")

string(REGEX MATCH "ReinSeghal" WASMATCHED ${GENIE_LIBS})
if(WASMATCHED AND GENIE_VERSION STREQUAL "210")
  set(GENIE_SEHGAL ${GENIE_LIBS})
  STRING(REPLACE "ReinSeghal" "ReinSehgal" GENIE_LIBS ${GENIE_SEHGAL})
  cmessage(DEBUG "Fixed inconsistency in library naming: ${GENIE_LIBS}")
endif()

if(NOT USE_REWEIGHT)
  SET(USING_GENIE_RW FALSE)
elseif(NOT GENIE_POST_R3)
  LIST(FIND GENIE_LIBS GReWeight FOUND_GENIE_RW)
  if(FOUND_GENIE_RW EQUAL -1)
    cmessage(DEBUG "Did NOT find ReWeight library. Here are libs: ${GENIE_LIBS}")
    SET(USING_GENIE_RW FALSE)
  else()
    SET(USING_GENIE_RW TRUE)
  endif()
elseif(DEFINED GENIE_REWEIGHT AND NOT GENIE_REWEIGHT STREQUAL "")
  LIST(FIND GENIE_LIBS GRwFwk FOUND_GENIE_RW)
  if(FOUND_GENIE_RW EQUAL -1)
    LIST(APPEND GENIE_LIBS GRwClc GRwFwk GRwIO)
    cmessage(DEBUG "Force added ReWeight library. Here are libs: ${GENIE_LIBS}")
    SET(USING_GENIE_RW TRUE)
  else()
    SET(USING_GENIE_RW FALSE)
  endif()
endif()

if(USING_GENIE_RW)
  cmessage(STATUS "Using GENIE ReWeight library.")
else()
  cmessage(STATUS "Building without GENIE ReWeight support.")
endif()

LIST(APPEND GENIE_LIBS -Wl,--end-group )
LIST(REVERSE GENIE_LIBS)
LIST(APPEND GENIE_LIBS -Wl,--start-group -Wl,--no-as-needed )
LIST(REVERSE GENIE_LIBS)

cmessage(DEBUG "GENIE_LIBS: ${GENIE_LIBS}")

################################  LHAPDF  ######################################
if(LHAPDF_LIB STREQUAL "")
  cmessage(FATAL_ERROR "Variable LHAPDF_LIB is not defined. The location of a pre-built lhapdf install must be defined either as $ cmake -DLHAPDF_LIB=/path/to/LHAPDF_libraries or as an environment variable $ export LHAPDF_LIB=/path/to/LHAPDF_libraries")
endif()

if(LHAPDF_INC STREQUAL "")
  cmessage(FATAL_ERROR "Variable LHAPDF_INC is not defined. The location of a pre-built lhapdf install must be defined either as $ cmake -DLHAPDF_INC=/path/to/LHAPDF_includes or as an environment variable $ export LHAPDF_INC=/path/to/LHAPDF_includes")
endif()

if(LHAPATH STREQUAL "")
  cmessage(FATAL_ERROR "Variable LHAPATH is not defined. The location of a the LHAPATH directory must be defined either as $ cmake -DLHAPATH=/path/to/LHAPATH or as an environment variable $ export LHAPATH=/path/to/LHAPATH")
endif()

################################  LIBXML  ######################################
if(LIBXML2_LIB STREQUAL "")
  GETLIBDIR(xml2-config --libs LIBXML2_LIB IGNORE_EMPTY_RESPONSE)
  if(LIBXML2_LIB STREQUAL "")
    message(WARNING "Variable LIBXML2_LIB is not defined, as xml2-config was found and didn't report a library include path, it is likely that libxml2.so can be found in the standard system location, lets hope so. Alternativly, a location can be forced by configering with -DLIBXML2_LIB=/path/to/LIBXML2_libraries or as an environment variable LIBXML2_LIB.")
  endif()
endif()

if(LIBXML2_INC STREQUAL "")
  GETINCDIR(xml2-config --cflags LIBXML2_INC IGNORE_EMPTY_RESPONSE)
  if(LIBXML2_INC STREQUAL "")
    message(WARNING "Variable LIBXML2_INC is not defined, as xml2-config was found and didn't report an include path, it is likely that libxml2.so can be found in the standard system location, lets hope so. Alternativly, a location can be forced by configering with -DLIBXML2_INC=/path/to/LIBXML2_includes or as an environment variable LIBXML2_INC.")
  endif()
endif()

###############################  log4cpp  ######################################
if(LOG4CPP_LIB STREQUAL "")
  GETLIBDIR(log4cpp-config --libs LOG4CPP_LIB IGNORE_EMPTY_RESPONSE)
  if(LOG4CPP_LIB STREQUAL "")
    message(WARNING "Variable LOG4CPP_LIB is not defined, as xml2-config was found and didn't report a library include path, it is likely that liblog4cpp.so can be found in the standard system location, lets hope so. Alternativly, a location can be forced by configering with -DLOG4CPP_LIB=/path/to/LOG4CPP_libraries or as an environment variable LOG4CPP_LIB.")
  endif()
endif()

if(LOG4CPP_INC STREQUAL "")
  GETINCDIR(log4cpp-config --cflags LOG4CPP_INC IGNORE_EMPTY_RESPONSE)
  if(LOG4CPP_INC STREQUAL "")
    message(WARNING "Variable LOG4CPP_LIB is not defined, as xml2-config was found and didn't report an include path, it is likely that log4cpp headers can be found in the standard system location, lets hope so. Alternativly, a location can be forced by configering with -DLOG4CPP_INC=/path/to/LOG4CPP_includes or as an environment variable LOG4CPP_INC.")
  endif()
endif()
################################################################################

# Set the compiler defines
LIST(APPEND EXTRA_CXX_FLAGS -D__GENIE_ENABLED__ -D__GENIE_VERSION__=${GENIE_VERSION})

LIST(APPEND EXTRA_LIBS ${GENIE_LIBS})

###############################  GSL  ######################################
if(GENIE_POST_R3)
  if(GSL_LIB STREQUAL "")
    GETLIBDIR(gsl-config --libs GSL_LIB)
    if(GSL_LIB STREQUAL "")
      message(FATAL_ERROR "Variable GSL_LIB is not defined and could not be found with gsl-config. The location of a pre-built gsl install must be defined either as $ cmake -DGSL_LIB=/path/to/GSL_libraries or as an environment variable $ export GSL_LIB=/path/to/GSL_libraries")
    endif()
  endif()

  if(GSL_INC STREQUAL "")
    GETINCDIR(gsl-config --cflags GSL_INC)
    if(GSL_INC STREQUAL "")
      message(FATAL_ERROR "Variable GSL_INC is not defined and could not be found with gsl-config. The location of a pre-built gsl install must be defined either as $ cmake -DGSL_INC=/path/to/GSL_includes or as an environment variable $ export GSL_INC=/path/to/GSL_includes")
    endif()
  endif()

  GETLIBS(gsl-config --libs GSL_LIB_LIST)

  if(USING_GENIE_RW AND GENIE_REWEIGHT STREQUAL "")
    message(FATAL_ERROR "Variable GENIE_REWEIGHT is not defined. When using GENIE v3+, we require the reweight product to be built and accessible via the environment variable GENIE_REWEIGHT")
  endif()

endif()
################################################################################

LIST(APPEND EXTRA_LIBS LHAPDF xml2 log4cpp)

LIST(APPEND EXTRA_LINK_DIRS
  ${GENIE_LIB_DIR}
  ${LHAPDF_LIB}
  ${LOG4CPP_LIB})

# Append only if we have found GENIE ReWeight
if(NOT GENIE_POST_R3)
  LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES
    ${GENIE_INCLUDES_DIR}
    ${GENIE_INCLUDES_DIR}/GHEP
    ${GENIE_INCLUDES_DIR}/Ntuple)
  if(USING_GENIE_RW)
    LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES ${GENIE_INCLUDES_DIR}/ReWeight)
  endif()
  LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES
    ${GENIE_INCLUDES_DIR}/Apps
    ${GENIE_INCLUDES_DIR}/FluxDrivers
    ${GENIE_INCLUDES_DIR}/EVGDrivers
    ${LHAPDF_INC}
    ${LIBXML2_INC}
    ${LOG4CPP_INC})
else()
  LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES
    ${GENIE_INCLUDES_DIR})

    if(USING_GENIE_RW)
      LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES ${GENIE_REWEIGHT}/src)
    endif()

    LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES ${GSL_INC}
    ${LHAPDF_INC}
    ${LIBXML2_INC}
    ${LOG4CPP_INC})

    if(USING_GENIE_RW)
      LIST(APPEND EXTRA_LINK_DIRS
      ${GENIE_REWEIGHT}/lib)
    endif()

    LIST(APPEND EXTRA_LINK_DIRS
      ${GSL_LIB}
    )

    LIST(APPEND EXTRA_LIBS ${GSL_LIB_LIST})

endif()

if(USE_PYTHIA8)
  set(NEED_PYTHIA8 TRUE)
  set(NEED_ROOTPYTHIA8 TRUE)
else()
  set(NEED_PYTHIA6 TRUE)
  set(NEED_ROOTPYTHIA6 TRUE)
endif()
set(NEED_ROOTEVEGEN FALSE)

SET(USE_GENIE TRUE CACHE BOOL "Whether to enable GENIE (reweight) support. Requires external libraries. <FALSE>" FORCE)
