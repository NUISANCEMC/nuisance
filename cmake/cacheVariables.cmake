# Copyright 2018 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

function(CheckAndSetDefaultEnv VARNAME DEFAULT CACHETYPE DOCSTRING ENVNAME)
  #cmessage(DEBUG "Trying to assign variable ${VARNAME} into the cache.")
  if(NOT DEFINED ${VARNAME})
    if(DEFINED ENV{${ENVNAME}} AND NOT $ENV{${ENVNAME}} STREQUAL "")
      set(${VARNAME} $ENV{${ENVNAME}} CACHE ${CACHETYPE} ${DOCSTRING})
      cmessage(DEBUG "    Read ${VARNAME} from ENVVAR ${ENVNAME} as $ENV{${ENVNAME}}.")
    else()
      set(${VARNAME} ${DEFAULT} CACHE ${CACHETYPE} ${DOCSTRING})
    endif()
  else()
    set(${VARNAME} ${${VARNAME}} CACHE ${CACHETYPE} ${DOCSTRING})
    unset(${VARNAME})
  endif()
  cmessage(CACHE "--Set cache variable: \"${VARNAME}\" to \"${${VARNAME}}\", in cache ${CACHETYPE}.")
endfunction()

function(CheckAndSetDefaultCache VARNAME DEFAULT CACHETYPE DOCSTRING)
#  cmessage(DEBUG "Trying to assign variable ${VARNAME} into the cache.")
  if(NOT DEFINED ${VARNAME})
    set(${VARNAME} ${DEFAULT} CACHE ${CACHETYPE} ${DOCSTRING})
  else()
    set(${VARNAME} ${${VARNAME}} CACHE ${CACHETYPE} ${DOCSTRING})
    unset(${VARNAME})
  endif()
  cmessage(CACHE "--Set cache variable: \"${VARNAME}\" to \"${${VARNAME}}\", in cache ${CACHETYPE}.")
endfunction()

function(CheckAndSetDefault VARNAME DEFAULT)
#  cmessage(DEBUG "Trying to assign variable ${VARNAME}.")
  if(NOT DEFINED ${VARNAME})
    set(${VARNAME} ${DEFAULT} PARENT_SCOPE)
    set(${VARNAME} ${DEFAULT})
  endif()
  cmessage(CACHE "--Set variable: \"${VARNAME}\" to \"${${VARNAME}}\".")
endfunction()

CheckAndSetDefaultCache(VERBOSE TRUE BOOL "Whether to configure loudly.")

set (CMAKE_SKIP_BUILD_RPATH TRUE)

#Changes default install path to be a subdirectory of the build dir.
#Can set build dir at configure time with -DCMAKE_INSTALL_PREFIX=/install/path
if(CMAKE_INSTALL_PREFIX STREQUAL "" OR CMAKE_INSTALL_PREFIX STREQUAL
  "/usr/local")
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}")
elseif(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE DEBUG)
elseif(NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE DEBUG)
endif()

# Misc
CheckAndSetDefaultCache(EXTRA_SETUP_SCRIPT "" PATH "The path to an extra script to inject into the NUISANCE setup script. <>")

CheckAndSetDefaultCache(USE_MINIMIZER TRUE INTERNAL "Whether we are using the ROOT minimization libraries. <TRUE>")

CheckAndSetDefaultCache(USE_GENERATOR_REWEIGHT TRUE BOOL "Whether we are using the generator reweighting libraries. <TRUE>")

CheckAndSetDefaultCache(USE_ROOT6 FALSE INTERNAL "Whether we are using the ROOT 6. <FALSE>")

# NuWro
CheckAndSetDefaultCache(USE_NuWro FALSE BOOL "Whether to enable NuWro support. <FALSE>")
CheckAndSetDefaultEnv(NUWRO "" PATH "Path to NuWro source tree root directory. Overrides environment variable \$NUWRO <>" NUWRO)
CheckAndSetDefaultEnv(NUWRO_INC "" PATH "Path to NuWro installed includes directory, needs to contain \"params_all.h\". Overrides environment variable \$NUWRO_INC <>" NUWRO_INC)

# NEUT
CheckAndSetDefaultCache(USE_NEUT FALSE BOOL "Whether to enable NEUT (reweight) support. Requires external libraries. <FALSE>")
CheckAndSetDefaultEnv(NEUT_VERSION FALSE STRING "NEUT version string, e.g. 5.4.0. <5.4.0>" NEUT_VERSION)
CheckAndSetDefaultEnv(NEUT_ROOT "" PATH "Path to NEUT source tree root directory. Overrides environment variable \$NEUT_ROOT <>" NEUT_ROOT)
CheckAndSetDefaultEnv(CERN "" PATH "Path to CERNLIB source tree root directory that NEUT was built against. Overrides environment variable \$CERN <>" CERN)
CheckAndSetDefaultEnv(CERN_LEVEL "" STRING "CERNLIB Library version. Overrides environment variable \$CERN_LEVEL <>" CERN_LEVEL)

# GENIE
CheckAndSetDefaultCache(USE_GENIE FALSE BOOL "Whether to enable GENIE (reweight) support. Requires external libraries. <FALSE>")
CheckAndSetDefaultCache(GENIE_VERSION "AUTO" STRING "GENIE Version <AUTO>")
CheckAndSetDefaultEnv(GENIE "" PATH "Path to GENIE source tree root directory. Overrides environment variable \$GENIE <>" GENIE)
CheckAndSetDefaultEnv(GENIE_REWEIGHT "" PATH "Path to GENIE ReWeight directory. Only relevant for GENIE v3+. Overrides environment variable \$GENIE_REWEIGHT <>" GENIE_REWEIGHT)
CheckAndSetDefaultEnv(LHAPDF_LIB "" PATH "Path to pre-built LHAPDF libraries. Overrides environment variable \$LHAPDF_LIB. <>" LHAPDF_LIB)
CheckAndSetDefaultEnv(LHAPDF_INC "" PATH "Path to installed LHAPDF headers. Overrides environment variable \$LHAPDF_INC. <>" LHAPDF_INC)
CheckAndSetDefaultEnv(LHAPATH "" PATH "Path to LHA PDF inputs. Overrides environment variable \$LHAPATH. <>" LHAPATH)
CheckAndSetDefaultEnv(LIBXML2_LIB "" PATH "Path to pre-built LIBXML2 libraries. Overrides environment variable \$LIBXML2_LIB. <>" LIBXML2_LIB)
CheckAndSetDefaultEnv(LIBXML2_INC "" PATH "Path to installed LIBXML2 headers. Overrides environment variable \$LIBXML2_INC. <>" LIBXML2_INC)
CheckAndSetDefaultEnv(LOG4CPP_LIB "" PATH "Path to pre-built LOG4CPP libraries. Overrides environment variable \$LOG4CPP_LIB. <>" LOG4CPP_LIB)
CheckAndSetDefaultEnv(LOG4CPP_INC "" PATH "Path to installed LOG4CPP headers. Overrides environment variable \$LOG4CPP_INC. <>" LOG4CPP_INC)
CheckAndSetDefaultEnv(GSL_LIB "" PATH "Path to pre-built gsl libraries. Overrides environment variable \$GSL_LIB. <>" GSL_LIB)
CheckAndSetDefaultEnv(GSL_INC "" PATH "Path to installed gsl headers. Overrides environment variable \$GSL_INC. <>" GSL_INC)

# Pythia
CheckAndSetDefaultEnv(PYTHIA6 "" PATH "Path to directory containing libPythia6.so. Overrides environment variable \$PYTHIA6 <>" PYTHIA6)
CheckAndSetDefault(NEED_PYTHIA6 FALSE)

CheckAndSetDefault(NEED_ROOTEVEGEN FALSE)
CheckAndSetDefault(NEED_ROOTPYTHIA6 FALSE)
