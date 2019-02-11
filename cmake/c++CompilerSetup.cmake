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

set(CXX_WARNINGS -Wall -Wextra)

LIST(APPEND EXTRA_CXX_FLAGS ${CXX_WARNINGS} -Werror -Wno-delete-non-virtual-dtor -Wno-unused "-D__FILENAME__=\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"")

BuildFlagString(NUISANCE_LINK_DIRS "-L" ${EXTRA_LINK_DIRS})

LIST(APPEND EXTRA_LIBS dl)
BuildLibraryFlagString(STR_EXTRA_LIBS ${EXTRA_LIBS})
BuildFlagString(STR_EXTRA_SHAREDOBJS " " ${EXTRA_SHAREDOBJS})

#This ends up holding all of the libraries and search paths for extenal dependencies
CatStringsIfNotEmpty(NUISANCE_DEPEND_LIBS
  ${STR_EXTRA_SHAREDOBJS}
  ${STR_EXTRA_LIBS})

BuildFlagString(STR_EXTRA_LINK_FLAGS " " ${EXTRA_LINK_FLAGS})

CatStringsIfNotEmpty(CMAKE_LINK_FLAGS
  ${CMAKE_LINK_FLAGS}
  ${STR_EXTRA_LINK_FLAGS})

get_directory_property(NUISANCE_INCLUDE_DIRS INCLUDE_DIRECTORIES)

BuildFlagString(NUISANCE_CXX_FLAGS " " ${EXTRA_CXX_FLAGS})
CatStringsIfNotEmpty(NUISANCE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${NUISANCE_CXX_FLAGS} )

if (VERBOSE)
  cmessage (STATUS "C++ Compiler      : ${CXX_COMPILER_NAME}")
  cmessage (STATUS "    Flags         : ${NUISANCE_CXX_FLAGS}")
  cmessage (STATUS "    Release Flags : ${CMAKE_CXX_FLAGS_RELEASE}")
  cmessage (STATUS "    Debug Flags   : ${CMAKE_CXX_FLAGS_DEBUG}")
  cmessage (STATUS "    Include Dirs  : ${NUISANCE_INCLUDE_DIRS}")
  cmessage (STATUS "    Linker Flags  : ${CMAKE_LINK_FLAGS}")
  cmessage (STATUS "    Link Dirs     : ${NUISANCE_LINK_DIRS}")
  cmessage (STATUS "    Lib Flags     : ${NUISANCE_DEPEND_LIBS}")
endif()

add_compile_options(${EXTRA_CXX_FLAGS})
