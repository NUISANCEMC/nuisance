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

CheckAndSetDefaultCache(EXTRA_SETUP_SCRIPT "" PATH "The path to an extra script to inject into the NUISANCE setup script. <>")

CheckAndSetDefaultCache(USE_MINIMIZER TRUE BOOL "Whether we are using the ROOT minimization libraries. <TRUE>")

CheckAndSetDefaultCache(USE_REWEIGHT TRUE BOOL "Whether we are expect to be able to build the reweighting libraries of enabled generators. <TRUE>")

CheckAndSetDefaultCache(USE_ROOT6 FALSE INTERNAL "Whether we are using the ROOT 6. <FALSE>")

CheckAndSetDefaultCache(USE_HEPMCNUEVT FALSE BOOL "Whether to enable HepMC3 input support. <FALSE>")

CheckAndSetDefaultCache(USE_NUSYST FALSE BOOL "Whether to enable DUNE Reweight  support. <FALSE>")
CheckAndSetDefaultEnv(NUSYST_ROOT "" PATH "Path to nusystematics install directory <>" NUSYST_ROOT)
CheckAndSetDefaultEnv(SYSTTOOLS_ROOT "" PATH "Path to systematicstools install directory <>" SYSTTOOLS_ROOT)

CheckAndSetDefaultCache(USE_HEPMC FALSE BOOL "Whether to enable HepMC input support. <FALSE>")
CheckAndSetDefaultEnv(HEPMC "" PATH "Path to HepMC source tree root directory. Overrides environment variable \$HEPMC <>" HEPMC)
CheckAndSetDefaultCache(HEPMC_MOMUNIT "GEV" STRING "HepMC momentum units [MEV|GEV]. <GEV>")
CheckAndSetDefaultCache(HEPMC_LENUNIT "CM" STRING "HepMC momentum units [MM|CM]. <CM>")
CheckAndSetDefaultCache(HEPMC_USED_EP FALSE INTERNAL "Whether we built HepMC or not. <FALSE>")

CheckAndSetDefaultCache(USE_NEUT FALSE BOOL "Whether to enable NEUT (reweight) support. Requires external libraries. <FALSE>")
CheckAndSetDefaultEnv(NEUT_VERSION FALSE STRING "NEUT version string, e.g. 5.4.0. <5.4.0>" NEUT_VERSION)
CheckAndSetDefaultEnv(NEUT_ROOT "" PATH "Path to NEUT source tree root directory. Overrides environment variable \$NEUT_ROOT <>" NEUT_ROOT)
CheckAndSetDefaultEnv(CERN "" PATH "Path to CERNLIB source tree root directory that NEUT was built against. Overrides environment variable \$CERN <>" CERN)
CheckAndSetDefaultEnv(CERN_LEVEL "" STRING "CERNLIB Library version. Overrides environment variable \$CERN_LEVEL <>" CERN_LEVEL)

CheckAndSetDefaultCache(USE_NuWro FALSE BOOL "Whether to enable NuWro support. <FALSE>")
CheckAndSetDefaultEnv(NUWRO "" PATH "Path to NuWro source tree root directory. Overrides environment variable \$NUWRO <>" NUWRO)
CheckAndSetDefaultEnv(NUWRO_INC "" PATH "Path to NuWro installed includes directory, needs to contain \"params_all.h\". Overrides environment variable \$NUWRO_INC <>" NUWRO_INC)
CheckAndSetDefaultCache(NUWRO_INPUT_FILE "" FILEPATH "Path to an input NuWro event vector, which can be used to build NuWro i/o libraries. <>")
CheckAndSetDefaultCache(NUWRO_BUILT_FROM_FILE FALSE INTERNAL "Whether the NuWro libraries were built by NUISANCE. <FALSE>")
CheckAndSetDefaultCache(USE_NuWro_RW FALSE BOOL "Whether to try and build support for NuWro reweighting. <FALSE>")
CheckAndSetDefaultCache(USE_NuWro_SRW_Event FALSE BOOL "Whether to use cut down NuWro reweight event format. Requires NuWro reweight. <FALSE>")

CheckAndSetDefaultCache(USE_GENIE FALSE BOOL "Whether to enable GENIE support. Requires external libraries. <FALSE>")
CheckAndSetDefaultCache(GENIE_VERSION "AUTO" STRING "GENIE Version <AUTO>")
CheckAndSetDefaultEnv(GENIE "" PATH "Path to GENIE source tree root directory. Overrides environment variable \$GENIE <>" GENIE)
CheckAndSetDefaultEnv(GENIE_REWEIGHT "" PATH "Path to GENIE ReWeight directory. Only relevant for GENIE v3+. Overrides environment variable \$GENIE_REWEIGHT <>" GENIE_REWEIGHT)
CheckAndSetDefaultCache(GENIE_EMPMEC_REWEIGHT FALSE BOOL "Whether to use GENIE EMP MEC reweight (requires custom GENIE) <FALSE>")
CheckAndSetDefaultCache(USE_GENIE_XSECMEC FALSE BOOL "Whether to use GENIE MEC reweight (requires custom GENIE) <FALSE>")
CheckAndSetDefaultEnv(LHAPDF_LIB "" PATH "Path to pre-built LHAPDF libraries. Overrides environment variable \$LHAPDF_LIB. <>" LHAPDF_LIB)
CheckAndSetDefaultEnv(LHAPDF_INC "" PATH "Path to installed LHAPDF headers. Overrides environment variable \$LHAPDF_INC. <>" LHAPDF_INC)
CheckAndSetDefaultEnv(LHAPATH "" PATH "Path to LHA PDF inputs. Overrides environment variable \$LHAPATH. <>" LHAPATH)
CheckAndSetDefaultEnv(LIBXML2_LIB "" PATH "Path to pre-built LIBXML2 libraries. Overrides environment variable \$LIBXML2_LIB. <>" LIBXML2_LIB)
CheckAndSetDefaultEnv(LIBXML2_INC "" PATH "Path to installed LIBXML2 headers. Overrides environment variable \$LIBXML2_INC. <>" LIBXML2_INC)
CheckAndSetDefaultEnv(LOG4CPP_LIB "" PATH "Path to pre-built LOG4CPP libraries. Overrides environment variable \$LOG4CPP_LIB. <>" LOG4CPP_LIB)
CheckAndSetDefaultEnv(LOG4CPP_INC "" PATH "Path to installed LOG4CPP headers. Overrides environment variable \$LOG4CPP_INC. <>" LOG4CPP_INC)
CheckAndSetDefaultEnv(GSL_LIB "" PATH "Path to pre-built gsl libraries. Overrides environment variable \$GSL_LIB. <>" GSL_LIB)
CheckAndSetDefaultEnv(GSL_INC "" PATH "Path to installed gsl headers. Overrides environment variable \$GSL_INC. <>" GSL_INC)

CheckAndSetDefaultCache(USE_T2K FALSE BOOL "Whether to enable T2KReWeight support. Requires external libraries. <FALSE>")
CheckAndSetDefaultEnv(T2KREWEIGHT "" PATH "Path to installed T2KReWeight. Overrides environment variable \$T2KREWEIGHT. <>" T2KREWEIGHT)

CheckAndSetDefaultCache(USE_NIWG FALSE BOOL "Whether to enable (T2K) NIWG ReWeight support. Requires external libraries. <FALSE>")
CheckAndSetDefaultEnv(NIWG_ROOT "" PATH "Path to installed NIWGReWeight. Overrides environment variable \$NIWG. <>" NIWG)

CheckAndSetDefaultCache(USE_MINERvA_RW FALSE BOOL "Whether to enable MINERvA ReWeight support. <FALSE>")

CheckAndSetDefaultCache(USE_NOvARwgt FALSE BOOL "Whether to enable NOvA ReWeight support. <FALSE>")

CheckAndSetDefaultEnv(NOVARWGT "" PATH "Path to directory containing libPythia6.so. Overrides environment variable \$NOVARWGT <>" NOVARWGT)


CheckAndSetDefaultEnv(PYTHIA6 "" PATH "Path to directory containing libPythia6.so. Overrides environment variable \$PYTHIA6 <>" PYTHIA6)

CheckAndSetDefaultEnv(PYTHIA8 "" PATH "Path to directory containing libPythia8.so. Overrides environment variable \$PYTHIA8 <>" PYTHIA8)
CheckAndSetDefaultCache(USE_PYTHIA8 FALSE BOOL "Whether to enable PYTHIA8 event support. <FALSE>")

CheckAndSetDefaultCache(USE_GiBUU TRUE BOOL "Whether to enable GiBUU event support. <TRUE>")
CheckAndSetDefaultCache(BUILD_GiBUU FALSE BOOL "Whether to build supporting GiBUU event tools along with a patched version of GiBUU. <FALSE>")

CheckAndSetDefaultCache(USE_NUANCE TRUE BOOL "Whether to enable NUANCE event support. <TRUE>")

CheckAndSetDefaultCache(USE_PROB3PP FALSE BOOL "Whether to download and compile in Prob3++ support. <FALSE>")

CheckAndSetDefaultCache(NO_EXTERNAL_UPDATE FALSE BOOL "Whether to perform the update target for external dependencies. Note this may produce errors for CMake < 3.8 where a bug was fixed for the feature that this option invokes. <FALSE>")

CheckAndSetDefaultCache(USE_GPERFTOOLS FALSE BOOL "Whether to compile in google performance tools. <FALSE>")

CheckAndSetDefault(NEED_PYTHIA6 FALSE)
CheckAndSetDefault(NEED_PYTHIA8 FALSE)

CheckAndSetDefault(NEED_ROOTEVEGEN FALSE)
CheckAndSetDefault(NEED_ROOTPYTHIA6 FALSE)

CheckAndSetDefaultCache(USE_OMP FALSE BOOL "Whether to enable multicore features (there currently are none...). <FALSE>")

CheckAndSetDefaultCache(USE_DYNSAMPLES TRUE BOOL "Whether to enable the dynamic sample loader. <TRUE>")

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
  NEUT_VERSION
  NEUT_ROOT
  CERN
  CERN_LEVEL
  USE_NuWro
  NUWRO
  NUWRO_INC
  NUWRO_INPUT_FILE
  NUWRO_BUILT_FROM_FILE
  USE_GENIE
  GENIE_VERSION
  GENIE
  GENIE_REWEIGHT
  LHAPDF_LIB
  LHAPDF_INC
  LHAPATH
  LIBXML2_LIB
  LIBXML2_INC
  LOG4CPP_LIB
  LOG4CPP_INC
  GSL_LIB
  GSL_INC
  PYTHIA6
  PYTHIA8
  USE_PYTHIA8
  USE_T2K
  T2KREWEIGHT
  USE_NIWG
  NIWG_ROOT
  USE_MINERvA_RW
  USE_NOvARwgt
  NOVARWGT
  USE_GiBUU
  BUILD_GiBUU
  USE_NUANCE
  USE_PROB3PP
  NO_EXTERNAL_UPDATE
  USE_GPERFTOOLS
  NO_EXPERIMENTS
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
