cmake_minimum_required (VERSION 2.6 FATAL_ERROR)

project(ExternalDataFitter)

enable_language(Fortran)

set (ExtFit_VERSION_MAJOR 1)
set (ExtFit_VERSION_MINOR 0) #The q+1'th letter of the alphabet
set (ExtFit_VERSION_REVISION 0)

set (ExtFit_VERSION_STRING "v${ExtFit_VERSION_MAJOR}r${ExtFit_VERSION_MINOR}")
if(${ExtFit_VERSION_REVISION} STRGREATER "0")
  set (ExtFit_VERSION_STRING "${ExtFit_VERSION_STRING}p${ExtFit_VERSION_REVISION}")
endif()

set (VERBOSE TRUE)

set (CMAKE_SKIP_BUILD_RPATH TRUE)

if(NOT DEFINED NOTEST OR NOT NOTEST)
  enable_testing()
endif()

include(${CMAKE_SOURCE_DIR}/cmake/cmessage.cmake)

if(NOT DEFINED USE_NEUT AND
  NOT DEFINED USE_NuWro AND
  NOT DEFINED USE_GENIE AND
  NOT DEFINED USE_T2K AND
  NOT DEFINED USE_NIWG AND
  NOT DEFINED USE_GiBUU AND
  NOT DEFINED USE_NUANCE)
  cmessage(FATAL_ERROR "No reweight engines requested. Configure with at least "
    "one of -DUSE_{NEUT,NuWro,GENIE,NIWG,T2K,GiBUU,NUANCE}.")
else()
  cmessage(STATUS "Generator Input Support:
  		  NEUT:${USE_NEUT},
  		  NuWro:${USE_NuWro},
		  GENIE:${USE_GENIE},
		  NIWG:${USE_NIWG},
  		  GiBUU:${USE_GiBUU},
		  NUANCE:${USE_NUANCE}")
endif()

#Changes default install path to be a subdirectory of the build dir.
#Can set build dir at configure time with -DCMAKE_INSTALL_PREFIX=/install/path
if(CMAKE_INSTALL_PREFIX STREQUAL "" OR CMAKE_INSTALL_PREFIX STREQUAL
  "/usr/local")
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}")
elseif(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}")
endif()

cmessage(STATUS "CMAKE_INSTALL_PREFIX: \"${CMAKE_INSTALL_PREFIX}\"")

if(CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE DEBUG)
elseif(NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE DEBUG)
endif()

cmessage(STATUS "CMAKE_BUILD_TYPE: \"${CMAKE_BUILD_TYPE}\"")

################################################################################
#                            Check Dependencies
################################################################################

##################################  ROOT  ######################################
include(${CMAKE_SOURCE_DIR}/cmake/ROOTSetup.cmake)

############################  Reweight Engines  ################################
include(${CMAKE_SOURCE_DIR}/cmake/ReweightEnginesSetup.cmake)

############################### GiBUU + NUANCE  ####################################

if(DEFINED USE_EXP AND USE_EXP)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DINMEMORYEVENTCLASS")
endif()

if(DEFINED USE_GiBUU AND USE_GiBUU)
  cmessage(STATUS "Included GiBUU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__GiBUU_ENABLED__")
  include(${CMAKE_SOURCE_DIR}/cmake/GiBUUSetup.cmake)
endif()

if(DEFINED USE_NUANCE AND USE_NUANCE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__NUANCE_ENABLED__")
endif()

#################################  Pythia6  ####################################
include(${CMAKE_SOURCE_DIR}/cmake/pythia6Setup.cmake)

################################## COMPILER ####################################
include(${CMAKE_SOURCE_DIR}/cmake/c++CompilerSetup.cmake)

################################################################################

################################# gperftools ###################################

include(${CMAKE_SOURCE_DIR}/cmake/gperfSetup.cmake)

################################### doxygen ###################################

include(${CMAKE_SOURCE_DIR}/cmake/docsSetup.cmake)

###############################################################################

set(MINCODE
  Routines
  FCN)

set(CORE
  MCStudies
  Genie
  FitBase
  InputHandler
  Splines
  Reweight
  Utils
  #Devel
  )


###############
# Allow compilation against single experiment folder
# Add later..
##############

set(EXPERIMENTS
  ANL
  ArgoNeuT
  BEBC
  BNL
  Electron
  FNAL
  GGM
  K2K
  MINERvA
  MiniBooNE
  SciBooNE
  T2K)

set(EXP_INCLUDE_DIRECTORIES)

foreach(edir ${EXPERIMENTS})
  set(EXP_INCLUDE_DIRECTORIES ${EXP_INCLUDE_DIRECTORIES};${CMAKE_SOURCE_DIR}/src/${edir})
endforeach()
cmessage(STATUS "Included experiments: ${EXP_INCLUDE_DIRECTORIES}")

foreach(mdir ${MINCODE})
  cmessage (STATUS "Configuring directory: src/${mdir}")
  add_subdirectory(src/${mdir})
endforeach()

foreach(edir ${EXPERIMENTS})
  cmessage (STATUS "Configuring directory: src/${edir}")
  add_subdirectory(src/${edir})
endforeach()

foreach(cdir ${CORE})
  cmessage (STATUS "Configuring directory: src/${cdir}")
  add_subdirectory(src/${cdir})
endforeach()

cmessage(STATUS "Module targets:  ${MODULETargets}")

add_subdirectory(app)
add_subdirectory(src/Tests)

configure_file(cmake/setup.sh.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/setup.sh" @ONLY)
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/setup.sh" DESTINATION
  ${CMAKE_INSTALL_PREFIX})


install(PROGRAMS
  "${PROJECT_SOURCE_DIR}/scripts/nuiscardgen" DESTINATION
  bin)

install(PROGRAMS
  "${PROJECT_SOURCE_DIR}/scripts/nuissamples" DESTINATION
  bin)
