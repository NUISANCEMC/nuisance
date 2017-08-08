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

################################################################################
#                            Check Dependencies
################################################################################

#################################  GENIE  ######################################
if(GENIE STREQUAL "")
  cmessage(FATAL_ERROR "Variable GENIE is not defined. "
    "The location of a pre-built GENIE install must be defined either as"
    " $ cmake -DGENIE=/path/to/GENIE or as and environment vairable"
    " $ export GENIE=/path/to/GENIE")
endif()

if (BUILD_GEVGEN)
  cmessage(STATUS "Building custom gevgen")
  LIST(APPEND EXTRA_CXX_FLAGS -D__GEVGEN_ENABLED__)
endif()

# Extract GENIE VERSION
execute_process (COMMAND ${CMAKE_SOURCE_DIR}/cmake/getgenieversion.sh ${GENIE}
  OUTPUT_VARIABLE GENIE_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process (COMMAND genie-config
  --libs OUTPUT_VARIABLE GENIE_LD_FLAGS_STR OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process (COMMAND genie-config
  --topsrcdir OUTPUT_VARIABLE GENIE_INCLUDES_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)

string(REGEX MATCH "-L\([^ ]+\) \(.*\)$" PARSE_GENIE_LIBS_MATCH ${GENIE_LD_FLAGS_STR})

cmessage(DEBUG "genie-config --libs: ${GENIE_LD_FLAGS_STR}")

if(NOT PARSE_GENIE_LIBS_MATCH)
  cmessage(FATAL_ERROR "Expected to be able to parse the result of genie-config --libs to a lib directory and a list of libraries to include, but got: \"${GENIE_LD_FLAGS_STR}\"")
endif()

set(GENIE_LIB_DIR ${CMAKE_MATCH_1})
set(GENIE_LIBS_RAW ${CMAKE_MATCH_2})
string(REPLACE "-l" "" GENIE_LIBS_STRIPED "${GENIE_LIBS_RAW}")

cmessage(STATUS "GENIE version : ${GENIE_VERSION}")
cmessage(STATUS "GENIE libdir  : ${GENIE_LIB_DIR}")
cmessage(STATUS "GENIE libs    : ${GENIE_LIBS_STRIPED}")

string(REGEX MATCH "ReinSeghal" WASMATCHED ${GENIE_LIBS_STRIPED})
if(WASMATCHED AND GENIE_VERSION STREQUAL "210")
  set(GENIE_SEHGAL ${GENIE_LIBS_STRIPED})
  STRING(REPLACE "ReinSeghal" "ReinSehgal" GENIE_LIBS_STRIPED ${GENIE_SEHGAL})
  cmessage(DEBUG "Fixed inconsistency in library naming: ${GENIE_LIBS_STRIPED}")
endif()

string(REGEX MATCH "ReWeight" WASMATCHED ${GENIE_LIBS_STRIPED})
if(NOT WASMATCHED)
  set(GENIE_LIBS_STRIPED "GReWeight ${GENIE_LIBS_STRIPED}")
  cmessage(DEBUG "Force added ReWeight library: ${GENIE_LIBS_STRIPED}")
endif()

string(REPLACE " " ";" GENIE_LIBS_LIST "${GENIE_LIBS_STRIPED}")
cmessage(DEBUG "genie-config --libs -- MATCH1: ${CMAKE_MATCH_1}")
cmessage(DEBUG "genie-config --libs -- MATCH2: ${CMAKE_MATCH_2}")
cmessage(DEBUG "genie-config --libs -- libs stripped: ${GENIE_LIBS_STRIPED}")
cmessage(DEBUG "genie-config --libs -- libs list: ${GENIE_LIBS_LIST}")

################################  LHAPDF  ######################################
if(LHAPDF_LIB STREQUAL "")
  cmessage(FATAL_ERROR "Variable LHAPDF_LIB is not defined. The location of a pre-built lhapdf install must be defined either as $ cmake -DLHAPDF_LIB=/path/to/LHAPDF_libraries or as and environment vairable $ export LHAPDF_LIB=/path/to/LHAPDF_libraries")
endif()

if(LHAPDF_INC STREQUAL "")
  cmessage(FATAL_ERROR "Variable LHAPDF_INC is not defined. The location of a pre-built lhapdf install must be defined either as $ cmake -DLHAPDF_INC=/path/to/LHAPDF_includes or as and environment vairable $ export LHAPDF_INC=/path/to/LHAPDF_includes")
endif()

if(LHAPATH STREQUAL "")
  cmessage(FATAL_ERROR "Variable LHAPATH is not defined. The location of a the LHAPATH directory must be defined either as $ cmake -DLHAPATH=/path/to/LHAPATH or as and environment variable $ export LHAPATH=/path/to/LHAPATH")
endif()

################################  LIBXML  ######################################
if(LIBXML2_LIB STREQUAL "")
  cmessage(FATAL_ERROR "Variable LIBXML2_LIB is not defined. The location of a pre-built libxml2 install must be defined either as $ cmake -DLIBXML2_LIB=/path/to/LIBXML2_libraries or as and environment vairable $ export LIBXML2_LIB=/path/to/LIBXML2_libraries")
endif()

if(LIBXML2_INC STREQUAL "")
  cmessage(FATAL_ERROR "Variable LIBXML2_INC is not defined. The location of a pre-built libxml2 install must be defined either as $ cmake -DLIBXML2_INC=/path/to/LIBXML2_includes or as and environment vairable $ export LIBXML2_INC=/path/to/LIBXML2_includes")
endif()
###############################  log4cpp  ######################################
if(LOG4CPP_LIB STREQUAL "")
  cmessage(FATAL_ERROR "Variable LOG4CPP_LIB is not defined. The location of a pre-built log4cpp install must be defined either as $ cmake -DLOG4CPP_LIB=/path/to/LOG4CPP_libraries or as and environment vairable $ export LOG4CPP_LIB=/path/to/LOG4CPP_libraries")
endif()

if(LOG4CPP_INC  STREQUAL "")
  cmessage(FATAL_ERROR "Variable LOG4CPP_INC is not defined. The location of a pre-built log4cpp install must be defined either as $ cmake -DGENIE_LOG4CPP_INC=/path/to/LOG4CPP_includes or as and environment vairable $ export LOG4CPP_INC=/path/to/LOG4CPP_includes")
endif()
################################################################################

LIST(APPEND EXTRA_CXX_FLAGS -D__GENIE_ENABLED__ -D__GENIE_VERSION__=${GENIE_VERSION})

LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES
  ${GENIE_INCLUDES_DIR}
  ${GENIE_INCLUDES_DIR}/GHEP
  ${GENIE_INCLUDES_DIR}/Ntuple
  ${GENIE_INCLUDES_DIR}/ReWeight
  ${GENIE_INCLUDES_DIR}/Apps
  ${GENIE_INCLUDES_DIR}/FluxDrivers
  ${GENIE_INCLUDES_DIR}/EVGDrivers
  ${LHAPDF_INC}
  ${LIBXML2_INC}
  ${LOG4CPP_INC})

SAYVARS()

LIST(APPEND EXTRA_LINK_DIRS
  ${GENIE_LIB_DIR}
  ${LHAPDF_LIB}
  ${LIBXML2_LIB}
  ${LOG4CPP_LIB})

#LIST(REVERSE EXTRA_LIBS)
#LIST(REVERSE GENIE_LIBS_LIST)
LIST(APPEND EXTRA_LIBS ${GENIE_LIBS_LIST})
#LIST(REVERSE EXTRA_LIBS)

LIST(APPEND EXTRA_LIBS LHAPDF xml2 log4cpp)

if(USE_PYTHIA8)
  set(NEED_PYTHIA8 TRUE)
  set(NEED_ROOTPYTHIA8 TRUE)
else()
  set(NEED_PYTHIA6 TRUE)
  set(NEED_ROOTPYTHIA6 TRUE)
endif()
set(NEED_ROOTEVEGEN TRUE)

SET(USE_GENIE TRUE CACHE BOOL "Whether to enable GENIE (reweight) support. Requires external libraries. <FALSE>" FORCE)
