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

# Pythia
CheckAndSetDefaultEnv(PYTHIA6 "" PATH "Path to directory containing libPythia6.so. Overrides environment variable \$PYTHIA6 <>" PYTHIA6)
CheckAndSetDefault(NEED_PYTHIA6 FALSE)

CheckAndSetDefault(NEED_ROOTEVEGEN FALSE)
CheckAndSetDefault(NEED_ROOTPYTHIA6 FALSE)
