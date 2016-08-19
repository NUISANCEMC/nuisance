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
if(NOT DEFINED GENIE AND NOT DEFINED ENV{GENIE})

  cmessage(FATAL_ERROR "Variable GENIE is not defined. "
    "The location of a pre-built GENIE install must be defined either as"
    " $ cmake -DGENIE=/path/to/GENIE or as and environment vairable"
    " $ export GENIE=/path/to/GENIE")

endif()

if(NOT DEFINED GENIE AND DEFINED ENV{GENIE})
  set(GENIE $ENV{GENIE})
endif()

execute_process (COMMAND genie-config
  --libs OUTPUT_VARIABLE GENIE_LD_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process (COMMAND genie-config
  --topsrcdir OUTPUT_VARIABLE GENIE_INCLUDES OUTPUT_STRIP_TRAILING_WHITESPACE)

################################  LHAPDF  ######################################
if(NOT DEFINED LHAPDF_LIB AND NOT DEFINED ENV{LHAPDF_LIB})

  cmessage(FATAL_ERROR "Variable LHAPDF_LIB is not defined. "
    "The location of a pre-built lhapdf install must be defined either as"
    " $ cmake -DLHAPDF_LIB=/path/to/LHAPDF_libraries or as and environment vairable"
    " $ export LHAPDF_LIB=/path/to/LHAPDF_libraries")

endif()

if(NOT DEFINED LHAPDF_LIB AND DEFINED ENV{LHAPDF_LIB})
  set(LHAPDF_LIB $ENV{LHAPDF_LIB})
endif()

if(NOT DEFINED LHAPDF_INC AND NOT DEFINED ENV{LHAPDF_INC})

  cmessage(FATAL_ERROR "Variable LHAPDF_INC is not defined. "
    "The location of a pre-built lhapdf install must be defined either as"
    " $ cmake -DLHAPDF_INC=/path/to/LHAPDF_includes or as and environment vairable"
    " $ export LHAPDF_INC=/path/to/LHAPDF_includes")

endif()

if(NOT DEFINED LHAPDF_INC AND DEFINED ENV{LHAPDF_INC})
  set(LHAPDF_INC $ENV{LHAPDF_INC})
endif()


################################  LIBXML  ######################################
if(NOT DEFINED LIBXML2_LIB AND NOT DEFINED ENV{LIBXML2_LIB})

  cmessage(FATAL_ERROR "Variable LIBXML2_LIB is not defined. "
    "The location of a pre-built libxml2 install must be defined either as"
    " $ cmake -DLIBXML2_LIB=/path/to/LIBXML2_libraries or as and environment vairable"
    " $ export LIBXML2_LIB=/path/to/LIBXML2_libraries")

endif()

if(NOT DEFINED LIBXML2_LIB AND DEFINED ENV{LIBXML2_LIB})
  set(LIBXML2_LIB $ENV{LIBXML2_LIB})
endif()

if(NOT DEFINED LIBXML2_INC AND NOT DEFINED ENV{LIBXML2_INC})

  cmessage(FATAL_ERROR "Variable LIBXML2_INC is not defined. "
    "The location of a pre-built libxml2 install must be defined either as"
    " $ cmake -DLIBXML2_INC=/path/to/LIBXML2_includes or as and environment vairable"
    " $ export LIBXML2_INC=/path/to/LIBXML2_includes")

endif()

if(NOT DEFINED LIBXML2_INC AND DEFINED ENV{LIBXML2_INC})
  set(LIBXML2_INC $ENV{LIBXML2_INC})
endif()


###############################  log4cpp  ######################################
if(NOT DEFINED LOG4CPP_LIB AND NOT DEFINED ENV{LOG4CPP_LIB})

  cmessage(FATAL_ERROR "Variable LOG4CPP_LIB is not defined. "
    "The location of a pre-built log4cpp install must be defined either as"
    " $ cmake -DLOG4CPP_LIB=/path/to/LOG4CPP_libraries or as and environment vairable"
    " $ export LOG4CPP_LIB=/path/to/LOG4CPP_libraries")

endif()

if(NOT DEFINED LOG4CPP_LIB AND DEFINED ENV{LOG4CPP_LIB})
  set(LOG4CPP_LIB $ENV{LOG4CPP_LIB})
endif()

if(NOT DEFINED LOG4CPP_INC AND NOT DEFINED ENV{LOG4CPP_INC})

  cmessage(FATAL_ERROR "Variable LOG4CPP_INC is not defined. "
    "The location of a pre-built log4cpp install must be defined either as"
    " $ cmake -DLOG4CPP_INC=/path/to/LOG4CPP_includes or as and environment vairable"
    " $ export LOG4CPP_INC=/path/to/LOG4CPP_includes")

endif()

if(NOT DEFINED LOG4CPP_INC AND DEFINED ENV{LOG4CPP_INC})
  set(LOG4CPP_INC $ENV{LOG4CPP_INC})
endif()
################################################################################

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__GENIE_ENABLED__ ")

set(RWENGINE_INCLUDE_DIRECTORIES ${RWENGINE_INCLUDE_DIRECTORIES}
  ${GENIE_INCLUDES}
  ${GENIE_INCLUDES}/GHEP
  ${GENIE_INCLUDES}/Ntuple
  ${GENIE_INCLUDES}/ReWeight
  ${LHAPDF_INC}
  ${LIBXML2_INC}
  ${LOG4CPP_INC})

set(RWENGINE_LINKER_FLAGS "${RWENGINE_LINKER_FLAGS} ${GENIE_LD_FLAGS} -L${LHAPDF_LIB} -lLHAPDF -L${LIBXML2_LIB} -lxml2 -L${LOG4CPP_LIB} -llog4cpp")

set(NEED_PYTHIA6 TRUE)
set(NEED_ROOTPYTHIA6 TRUE)

