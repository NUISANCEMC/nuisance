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

function(CheckAndSetDefaultEnv VARNAME DEFAULT CACHETYPE DOCSTRING ENVNAME)
  cmessage(DEBUG "Trying to assign variable ${VARNAME} into the cache.")
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
  cmessage(DEBUG "--Set cache variable: \"${VARNAME}\" to \"${${VARNAME}}\", in cache ${CACHETYPE}.")
endfunction()

function(CheckAndSetDefaultCache VARNAME DEFAULT CACHETYPE DOCSTRING)
  cmessage(DEBUG "Trying to assign variable ${VARNAME} into the cache.")
  if(NOT DEFINED ${VARNAME})
    set(${VARNAME} ${DEFAULT} CACHE ${CACHETYPE} ${DOCSTRING})
  else()
    set(${VARNAME} ${${VARNAME}} CACHE ${CACHETYPE} ${DOCSTRING})
    unset(${VARNAME})
  endif()
  cmessage(DEBUG "--Set cache variable: \"${VARNAME}\" to \"${${VARNAME}}\", in cache ${CACHETYPE}.")
endfunction()

function(CheckAndSetDefault VARNAME DEFAULT)
  cmessage(DEBUG "Trying to assign variable ${VARNAME}.")
  if(NOT DEFINED ${VARNAME})
    set(${VARNAME} ${DEFAULT} PARENT_SCOPE)
    set(${VARNAME} ${DEFAULT})
  endif()
  cmessage(DEBUG "--Set variable: \"${VARNAME}\" to \"${${VARNAME}}\".")
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

CheckAndSetDefaultCache(USE_HEPMC FALSE BOOL "Whether to enable HepMC input support. <FALSE>")
CheckAndSetDefaultEnv(HEPMC "" PATH "Path to HepMC source tree root directory. Overrides environment variable \$HEPMC <>" HEPMC)
CheckAndSetDefaultCache(HEPMC_MOMUNIT "GEV" STRING "HepMC momentum units [MEV|GEV]. <GEV>")
CheckAndSetDefaultCache(HEPMC_LENUNIT "CM" STRING "HepMC momentum units [MM|CM]. <CM>")
CheckAndSetDefaultCache(HEPMC_USED_EP FALSE INTERNAL "Whether the we built HepMC or not. <FALSE>")

CheckAndSetDefaultCache(USE_NEUT FALSE BOOL "Whether to enable NEUT (reweight) support. Requires external libraries. <FALSE>")
CheckAndSetDefaultEnv(NEUT_ROOT "" PATH "Path to NEUT source tree root directory. Overrides environment variable \$NEUT_ROOT <>" NEUT_ROOT)
CheckAndSetDefaultEnv(NEUT_CERN "" PATH "Path to CERNLIB source tree root directory that NEUT was built against. Overrides environment variable \$CERN <>" CERN)
CheckAndSetDefaultEnv(NEUT_CERN_LEVEL "" STRING "CERNLIB Library version. Overrides environment variable \$CERN_LEVEL <>" CERN_LEVEL)

CheckAndSetDefaultCache(USE_NuWro FALSE BOOL "Whether to enable NuWro support. <FALSE>")
CheckAndSetDefaultEnv(NUWRO_ROOT "" PATH "Path to NuWro source tree root directory. Overrides environment variable \$NUWRO <>" NUWRO)
CheckAndSetDefaultEnv(NUWRO_INCLUDES "" PATH "Path to NuWro installed includes directory, needs to contain \"params_all.h\". Overrides environment variable \$NUWRO_INC <>" NUWRO_INC)
CheckAndSetDefaultCache(NUWRO_INPUT_FILE "" FILEPATH "Path to an input NuWro event vector, which can be used to build NuWro i/o libraries. <>")
CheckAndSetDefaultCache(NUWRO_BUILT_FROM_FILE FALSE INTERNAL "Whether the NuWro libraries were built by NUISANCE. <FALSE>")
CheckAndSetDefaultCache(USE_NuWro_RW FALSE BOOL "Whether to try and build support for NuWro reweighting. <FALSE>")

CheckAndSetDefaultCache(USE_GENIE FALSE BOOL "Whether to enable GENIE (reweight) support. Requires external libraries. <FALSE>")
CheckAndSetDefaultEnv(GENIE_ROOT "" PATH "Path to GENIE source tree root directory. Overrides environment variable \$GENIE <>" GENIE)
CheckAndSetDefaultEnv(GENIE_LHAPDF_LIB "" PATH "Path to pre-built LHAPDF libraries. Overrides environment variable \$LHAPDF_LIB. <>" LHAPDF_LIB)
CheckAndSetDefaultEnv(GENIE_LHAPDF_INC "" PATH "Path to installed LHAPDF headers. Overrides environment variable \$LHAPDF_INC. <>" LHAPDF_INC)
CheckAndSetDefaultEnv(GENIE_LHAPATH "" PATH "Path to LHA PDF inputs. Overrides environment variable \$LHAPATH. <>" LHAPATH)
CheckAndSetDefaultEnv(GENIE_LIBXML2_LIB "" PATH "Path to pre-built LIBXML2 libraries. Overrides environment variable \$LIBXML2_LIB. <>" LIBXML2_LIB)
CheckAndSetDefaultEnv(GENIE_LIBXML2_INC "" PATH "Path to installed LIBXML2 headers. Overrides environment variable \$LIBXML2_INC. <>" LIBXML2_INC)
CheckAndSetDefaultEnv(GENIE_LOG4CPP_LIB "" PATH "Path to pre-built LOG4CPP libraries. Overrides environment variable \$LOG4CPP_LIB. <>" LOG4CPP_LIB)
CheckAndSetDefaultEnv(GENIE_LOG4CPP_INC "" PATH "Path to installed LOG4CPP headers. Overrides environment variable \$LOG4CPP_INC. <>" LOG4CPP_INC)

CheckAndSetDefaultCache(BUILD_GEVGEN FALSE BOOL "Whether to build nuisance_gevgen app.")

CheckAndSetDefaultCache(USE_T2K FALSE BOOL "Whether to enable T2KReWeight support. Requires external libraries. <FALSE>")
CheckAndSetDefaultEnv(T2KREWEIGHT_ROOT "" PATH "Path to installed T2KREWEIGHTReWeight. Overrides environment variable \$T2KREWEIGHT. <>" T2KREWEIGHT)

CheckAndSetDefaultCache(USE_NIWG FALSE BOOL "Whether to enable (T2K) NIWG ReWeight support. Requires external libraries. <FALSE>")
CheckAndSetDefaultEnv(NIWG_ROOT "" PATH "Path to installed NIWGReWeight. Overrides environment variable \$NIWG. <>" NIWG)

CheckAndSetDefaultEnv(PYTHIA6 "" PATH "Path to directory containing libPythia6.so. Overrides environment variable \$PYTHIA6 <>" PYTHIA6)

CheckAndSetDefaultCache(USE_GiBUU TRUE BOOL "Whether to enable GiBUU event support. <TRUE>")
CheckAndSetDefaultCache(BUILD_GiBUU FALSE BOOL "Whether to build supporting GiBUU event tools along with a patched version of GiBUU. <FALSE>")

CheckAndSetDefaultCache(USE_NUANCE TRUE BOOL "Whether to enable NUANCE event support. <TRUE>")

CheckAndSetDefaultCache(NO_EXTERNAL_UPDATE TRUE BOOL "Whether to perform the update target for external dependencies. <TRUE>")

CheckAndSetDefaultCache(USE_GPERFTOOLS FALSE BOOL "Whether to compile in google performance tools. <TRUE>")

CheckAndSetDefault(NEED_PYTHIA6 FALSE)

CheckAndSetDefault(NEED_ROOTEVEGEN FALSE)
CheckAndSetDefault(NEED_ROOTPYTHIA6 FALSE)

CheckAndSetDefaultCache(USE_OMP FALSE BOOL "Whether to enable multicore features (there currently are none...). <FALSE>")

CheckAndSetDefault(NO_EXPERIMENTS FALSE)

cmessage(STATUS "NO_EXPERIMENTS: ${NO_EXPERIMENTS}")

CheckAndSetDefaultCache(NO_ANL ${NO_EXPERIMENTS} BOOL "Whether to *NOT* build ANL samples. <-DNO_EXPERIMENTS=FALSE>")
CheckAndSetDefaultCache(NO_ArgoNeuT ${NO_EXPERIMENTS} BOOL "Whether to *NOT* build ArgoNeuT samples. <-DNO_EXPERIMENTS=FALSE>")
CheckAndSetDefaultCache(NO_BEBC ${NO_EXPERIMENTS} BOOL "Whether to *NOT* build BEBC samples. <-DNO_EXPERIMENTS=FALSE>")
CheckAndSetDefaultCache(NO_BNL ${NO_EXPERIMENTS} BOOL "Whether to *NOT* build BNL samples. <-DNO_EXPERIMENTS=FALSE>")
CheckAndSetDefaultCache(NO_FNAL ${NO_EXPERIMENTS} BOOL "Whether to *NOT* build FNAL samples. <-DNO_EXPERIMENTS=FALSE>")
CheckAndSetDefaultCache(NO_GGM ${NO_EXPERIMENTS} BOOL "Whether to *NOT* build GGM samples. <-DNO_EXPERIMENTS=FALSE>")
CheckAndSetDefaultCache(NO_K2K ${NO_EXPERIMENTS} BOOL "Whether to *NOT* build K2K samples. <-DNO_EXPERIMENTS=FALSE>")
CheckAndSetDefaultCache(NO_MINERvA ${NO_EXPERIMENTS} BOOL "Whether to *NOT* build MINERvA samples. <-DNO_EXPERIMENTS=FALSE>")
CheckAndSetDefaultCache(NO_MiniBooNE ${NO_EXPERIMENTS} BOOL "Whether to *NOT* build MiniBooNE samples. <-DNO_EXPERIMENTS=FALSE>")
CheckAndSetDefaultCache(NO_T2K ${NO_EXPERIMENTS} BOOL "Whether to *NOT* build T2K samples. <-DNO_EXPERIMENTS=FALSE>")
CheckAndSetDefaultCache(NO_SciBooNE ${NO_EXPERIMENTS} BOOL "Whether to *NOT* build SciBooNE samples. <-DNO_EXPERIMENTS=FALSE>")


function(SAYVARS)

LIST(APPEND VARS
  USE_HEPMC
  HEPMC
  HEPMC_MOMUNIT
  HEPMC_LENUNIT
  HEPMC_USED_EP
  USE_NEUT
  NEUT_ROOT
  NEUT_CERN
  NEUT_CERN_LEVEL
  USE_NuWro
  NUWRO_ROOT
  NUWRO_INCLUDES
  NUWRO_INPUT_FILE
  NUWRO_BUILT_FROM_FILE
  USE_GENIE
  GENIE_ROOT
  GENIE_LHAPDF_LIB
  GENIE_LHAPDF_INC
  GENIE_LIBXML2_LIB
  GENIE_LIBXML2_INC
  GENIE_LOG4CPP_LIB
  GENIE_LOG4CPP_INC
  BUILD_GEVGEN
  USE_T2K
  USE_NIWG
  USE_GiBUU
  BUILD_GiBUU
  USE_NUANCE
  NO_EXTERNAL_UPDATE
  USE_GPERFTOOLS
  NO_ANL
  NO_ArgoNeuT
  NO_BEBC
  NO_BNL
  NO_FNAL
  NO_GGM
  NO_K2K
  NO_MINERvA
  NO_MiniBooNE
  NO_T2K
  NO_SciBooNE)

  foreach(v ${VARS})
    if(DEFINED ${v})
      cmessage(DEBUG "VARIABLE: \"${v}\" = \"${${v}}\"")
    endif()
  endforeach(v)

endfunction()
