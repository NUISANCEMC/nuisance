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
OUTPUT_VARIABLE GENIE_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)

if(GENIE_VERSION VERSION_GREATER 2.12.999)
  LIST(APPEND GENIE_CXX_FLAGS -DGENIE_V3_INTERFACE -DGENIE_EMP_MECRW_ENABLED)
endif()

execute_process (COMMAND genie-config
  --topsrcdir OUTPUT_VARIABLE GENIE_INCLUDES_DIR
              OUTPUT_STRIP_TRAILING_WHITESPACE)

#Allows for external override in the case where genie-config lies.
if(NOT DEFINED GENIE_LIB_DIR OR GENIE_LIB_DIR STREQUAL "")
  GETLIBDIRS(genie-config --libs GENIE_LIB_DIR)
endif()
GETLIBS(genie-config --libs GENIE_LIBS)

string(REGEX MATCH "ReinSeghal" WASMATCHED ${GENIE_LIBS})
if(WASMATCHED AND GENIE_VERSION VERSION_GREATER 2.9.999 AND GENIE_VERSION VERSION_LESS 2.11.0 )
  set(GENIE_SEHGAL ${GENIE_LIBS})
  STRING(REPLACE "ReinSeghal" "ReinSehgal" GENIE_LIBS ${GENIE_SEHGAL})
  cmessage(DEBUG "Fixed inconsistency in library naming: ${GENIE_LIBS}")
endif()

if(USE_GENERATOR_REWEIGHT)
  if(GENIE_VERSION VERSION_GREATER 2.12.999)
    LIST(FIND GENIE_LIBS GRwFwk WAS_FOUND)
    if(WAS_FOUND STREQUAL "-1")
      LIST(APPEND GENIE_LIBS GRwClc GRwFwk GRwIO)
      cmessage(DEBUG "Force added ReWeight library: ${GENIE_LIBS}")
    endif()
  else()
    LIST(FIND GENIE_LIBS GReWeight WAS_FOUND)
    if(WAS_FOUND STREQUAL "-1")
      LIST(APPEND GENIE_LIBS GReWeight)
      cmessage(DEBUG "Force added ReWeight library: ${GENIE_LIBS}")
    endif()
  endif(GENIE_VERSION VERSION_GREATER 2.12.999)
endif(USE_GENERATOR_REWEIGHT)

LIST(APPEND GENIE_LIBS -Wl,--end-group )
LIST(REVERSE GENIE_LIBS)
LIST(APPEND GENIE_LIBS -Wl,--start-group -Wl,--no-as-needed )
LIST(REVERSE GENIE_LIBS)

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
    message(WARNING "Variable LIBXML2_LIB is not defined, as xml2-config was found and didn't report a library include path, it is likely that libxml2.so can be found in the standard system location, lets hope so.")
  endif()
endif()

if(LIBXML2_INC STREQUAL "")
  GETINCDIR(xml2-config --cflags LIBXML2_INC)
  if(LIBXML2_INC STREQUAL "")
    message(FATAL_ERROR "Variable LIBXML2_INC is not defined and could not be found with xml2-config. The location of a pre-built libxml2 install must be defined either as $ cmake -DLIBXML2_INC=/path/to/LIBXML2_includes or as an environment variable $ export LIBXML2_INC=/path/to/LIBXML2_includes")
  endif()
endif()

###############################  log4cpp  ######################################
if(LOG4CPP_LIB STREQUAL "")
  GETLIBDIR(log4cpp-config --libs LOG4CPP_LIB)
  if(LOG4CPP_LIB STREQUAL "")
    message(FATAL_ERROR "Variable LOG4CPP_LIB is not defined and could not be found with log4cpp-config. The location of a pre-built log4cpp install must be defined either as $ cmake -DLOG4CPP_LIB=/path/to/LOG4CPP_libraries or as an environment variable $ export LOG4CPP_LIB=/path/to/LOG4CPP_libraries")
  endif()
endif()

if(LOG4CPP_INC STREQUAL "")
  GETINCDIR(log4cpp-config --cflags LOG4CPP_INC)
  if(LOG4CPP_INC STREQUAL "")
    message(FATAL_ERROR "Variable LOG4CPP_INC is not defined and could not be found with log4cpp-config. The location of a pre-built log4cpp install must be defined either as $ cmake -DLOG4CPP_INC=/path/to/LOG4CPP_includes or as an environment variable $ export LOG4CPP_INC=/path/to/LOG4CPP_includes")
  endif()
endif()
################################################################################

LIST(APPEND GENIE_CXX_FLAGS -DGENIE_VERSION=${GENIE_VERSION})

###############################  GSL  ######################################
if(GENIE_VERSION VERSION_GREATER 2.12.999)
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

  if(GENIE_REWEIGHT STREQUAL "")
    message(FATAL_ERROR "Variable GENIE_REWEIGHT is not defined. When using GENIE v3+, we require the reweight product to be built and accessible via the environment variable GENIE_REWEIGHT")
  endif()
endif(GENIE_VERSION VERSION_GREATER 2.12.999)
################################################################################

LIST(APPEND GENIE_LIBS LHAPDF xml2 log4cpp)

LIST(APPEND GENIE_LINK_DIRS
  ${GENIE_LIB_DIR}
  ${LHAPDF_LIB}
  ${LOG4CPP_LIB})

if(GENIE_VERSION VERSION_GREATER 2.12.999)
  LIST(APPEND GENIE_INCLUDE_DIRS
    ${GENIE_INCLUDES_DIR}
    ${GENIE_REWEIGHT}/src
    ${GSL_INC}
    ${LHAPDF_INC}
    ${LIBXML2_INC}
    ${LOG4CPP_INC})

    LIST(APPEND GENIE_LINK_DIRS
      ${GENIE_REWEIGHT}/lib
      ${GSL_LIB}
    )

    LIST(APPEND GENIE_LIBS ${GSL_LIB_LIST})
else()
  LIST(APPEND GENIE_INCLUDE_DIRS
    ${GENIE_INCLUDES_DIR}
    ${LHAPDF_INC}
    ${LIBXML2_INC}
    ${LOG4CPP_INC})
endif(GENIE_VERSION VERSION_GREATER 2.12.999)

BuildFlagString(GENIE_CXX_FLAGS " " ${GENIE_CXX_FLAGS})
BuildFlagString(GENIE_LINK_DIRS "-L" ${GENIE_LINK_DIRS})

set(NEED_PYTHIA6 TRUE)
set(NEED_ROOTPYTHIA6 TRUE)

set(NEED_ROOTEVEGEN TRUE)

SET(USE_GENIE TRUE CACHE BOOL "Whether to enable GENIE (reweight) support. Requires external libraries. <FALSE>" FORCE)

cmessage(STATUS "GENIE")
cmessage(STATUS "     Version   : ${GENIE_VERSION}")
cmessage(STATUS "     Flags     : ${GENIE_CXX_FLAGS}")
cmessage(STATUS "     Includes  : ${GENIE_INCLUDE_DIRS}")
cmessage(STATUS "     Link Dirs : ${GENIE_LINK_DIRS}")
cmessage(STATUS "     Libs      : ${GENIE_LIBS}")
