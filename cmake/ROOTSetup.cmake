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

if (NOT DEFINED ENV{ROOTSYS})
  cmessage (FATAL_ERROR "$ROOTSYS is not defined, please set up ROOT first.")
else()
  cmessage(STATUS "Using ROOT installed at $ENV{ROOTSYS}")
  set(CMAKE_ROOTSYS $ENV{ROOTSYS})
endif()

# Get cflags from ROOT
execute_process (COMMAND root-config
  --cflags OUTPUT_VARIABLE ROOT_CXX_FLAGS_RAW OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" ROOT_CXX_FLAGS "${ROOT_CXX_FLAGS_RAW}")
# Get libdir from ROOT
execute_process (COMMAND root-config
  --libdir OUTPUT_VARIABLE ROOT_LIBDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
# Get version from ROOT
execute_process (COMMAND root-config
  --version OUTPUT_VARIABLE ROOT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
# Get features from ROOT
execute_process (COMMAND root-config
  --features OUTPUT_VARIABLE ROOT_FEATURES OUTPUT_STRIP_TRAILING_WHITESPACE)

LIST(APPEND EXTRA_LINK_DIRS ${ROOT_LIBDIR})

LIST(APPEND ROOT_LIBS
  Core
  Cint
  RIO
  XMLIO
  Net
  Hist
  Graf
  Graf3d
  Gpad
  Tree
  Rint
  Postscript
  Matrix
  Physics
  MathCore
  Thread
  EG
  Geom
  GenVector)

if(USE_MINIMIZER)

  if("${ROOT_FEATURES}" MATCHES "minuit2")
    cmessage(STATUS "ROOT built with MINUIT2 support")
    LIST(APPEND EXTRA_CXX_FLAGS -D__MINUIT2_ENABLED__)
  else()
    cmessage(FATAL_ERROR "ROOT built without MINUIT2 support but minimizer functionality requested. Either configure with -DUSE_MINIMIZER=FALSE or use a version of ROOT with MINUIT2 support.")
  endif()

  string(REGEX MATCH "6.*" ROOTVERSIXMATCH ${ROOT_VERSION})
  if(ROOTVERSIXMATCH)
    cmessage(STATUS "Using ROOT6, We are essentially flying blind here.")
    LIST(REMOVE_ITEM ROOT_LIBS Cint)
    LIST(APPEND EXTRA_CXX_FLAGS -DROOT6_USE_FIT_FITTER_INTERFACE)
    set(USE_ROOT6 True)
  else()
    string(REGEX MATCH "5.34/([0-9]+)" ROOTVERSMATCH ${ROOT_VERSION})
    if(NOT ROOTVERSMATCH OR ${CMAKE_MATCH_1} LESS "19")
      cmessage(FATAL_ERROR "ROOT Version: ${ROOT_VERSION} has out of date minimizer interface, but minimizer functionality requested. Please configure with -DUSE_MINIMIZER=FALSE or update to 5.34/19 or greater to enable minimization features.")
    endif()
  endif()

endif()

if("${ROOT_FEATURES}" MATCHES "opengl")
  cmessage(STATUS "ROOT built with OpenGL support")
  LIST(APPEND ROOT_LIBS RGL)
endif()

if(DEFINED NEED_ROOTPYTHIA6 AND NEED_ROOTPYTHIA6)
  LIST(APPEND ROOT_LIBS EGPythia6 Pythia6)
endif()

cmessage ( STATUS "[ROOT]: root-config --version: ${ROOT_VERSION} ")
cmessage ( STATUS "[ROOT]: root-config --cflags : ${ROOT_CXX_FLAGS} ")
cmessage ( STATUS "[ROOT]: root-config --libs   : ${ROOT_LD_FLAGS} ")

LIST(APPEND EXTRA_CXX_FLAGS ${ROOT_CXX_FLAGS})

#Helper functions for building dictionaries
function(GenROOTDictionary OutputDictName Header LinkDef)

  get_directory_property(incdirs INCLUDE_DIRECTORIES)
  string(REPLACE ";" ";-I" LISTDIRINCLUDES "-I${incdirs}")
  string(REPLACE " " ";" LISTCPPFLAGS "${EXTRA_CXX_FLAGS}")

  #ROOT5 CINT cannot handle it.
  list(REMOVE_ITEM LISTCPPFLAGS "-std=c++11")

  message(STATUS "LISTCPPFLAGS: ${LISTCPPFLAGS}")
  message(STATUS "LISTINCLUDES: ${LISTDIRINCLUDES}")
  #Learn how to generate the Dict.cxx and Dict.hxx
  add_custom_command(
    OUTPUT "${OutputDictName}.cxx" "${OutputDictName}.h"
    COMMAND rootcint
    ARGS -f ${OutputDictName}.cxx -c
    -p ${LISTDIRINCLUDES} ${LISTCPPFLAGS} ${Header} ${LinkDef}
    DEPENDS ${Header};${LinkDef})
endfunction()


function(BuildROOTProject ProjectName InputFile CommaSeparatedClassesToDump LIBLINKMODE)

  string(REPLACE "," ";" HeadersToDump ${CommaSeparatedClassesToDump})
  set(OUTPUTFILES ${CMAKE_BINARY_DIR}/${ProjectName}/${ProjectName}ProjectSource.cxx
    ${CMAKE_BINARY_DIR}/${ProjectName}/${ProjectName}LinkDef.h
    ${CMAKE_BINARY_DIR}/${ProjectName}/${ProjectName}ProjectHeaders.h
    ${CMAKE_BINARY_DIR}/${ProjectName}/${ProjectName}ProjectInstances.h)

  cmessage(STATUS "As part of ROOT project: ${ProjectName}")
  foreach (header ${HeadersToDump})
    LIST(APPEND OUTPUTFILES "${CMAKE_BINARY_DIR}/${ProjectName}/${header}.h")
    cmessage(STATUS "Will generate: ${CMAKE_BINARY_DIR}/${ProjectName}/${header}.h")
  endforeach()

  add_custom_command(
    OUTPUT ${OUTPUTFILES}
    COMMAND ${CMAKE_BINARY_DIR}/src/Utils/DumpROOTClassesFromVector
    ARGS ${InputFile}
      ${CMAKE_BINARY_DIR}/${ProjectName}
      ${CommaSeparatedClassesToDump}
    VERBATIM
    DEPENDS DumpROOTClassesFromVector)

  add_custom_target(${ProjectName}_sources
    DEPENDS ${OUTPUTFILES})

  GenROOTDictionary(
    ${CMAKE_BINARY_DIR}/${ProjectName}/${ProjectName}ProjectDict
    ${CMAKE_BINARY_DIR}/${ProjectName}/${ProjectName}ProjectHeaders.h
    ${CMAKE_BINARY_DIR}/${ProjectName}/${ProjectName}LinkDef.h
    )

  add_custom_target(${ProjectName}ProjectDict
    DEPENDS
    ${CMAKE_BINARY_DIR}/${ProjectName}/${ProjectName}ProjectDict.cxx
    ${CMAKE_BINARY_DIR}/${ProjectName}/${ProjectName}ProjectDict.h )
  # add_dependencies(${ProjectName}ProjectDict ${ProjectName}_sources)

  #ProjectSource.cxx includes ProjectDict.cxx, so no need to add to compilation.
  set(ROAA_SOURCEFILES
    ${CMAKE_BINARY_DIR}/${ProjectName}/${ProjectName}ProjectSource.cxx)

  add_library(${ProjectName} ${LIBLINKMODE} ${ROAA_SOURCEFILES})
  add_dependencies(${ProjectName} ${ProjectName}ProjectDict)

endfunction()
