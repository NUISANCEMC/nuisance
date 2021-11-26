# Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

include(cmake/parseConfigApp.cmake)

find_program(T2KRWCONFIG NAMES t2kreweight-config)
SET(HAVET2KRWCONFIG FALSE)
# We are dealing with shiny NEUT
if(NOT "${T2KRWCONFIG}" STREQUAL "T2KRWCONFIG-NOTFOUND")
  SET(HAVET2KRWCONFIG TRUE)
  cmessage(STATUS "Found t2kreweight-config, using it to determine configuration.")
else()
  cmessage(STATUS "Failed to find t2kreweight-config, assuming older NEUT build.")
endif()

if(HAVET2KRWCONFIG)

	  execute_process (COMMAND t2kreweight-config
    --version OUTPUT_VARIABLE T2KRW_VER
             OUTPUT_STRIP_TRAILING_WHITESPACE)

  if(T2KRW_VER VERSION_GREATER_EQUAL 21.11)
  	GetLibDirs(CONFIG_APP t2kreweight-config ARGS --libs OUTPUT_VARIABLE T2KRW_LINK_DIRS)
		GetIncDirs(CONFIG_APP t2kreweight-config ARGS --cflags OUTPUT_VARIABLE T2KRW_INC_DIRS)
		GetLibs(CONFIG_APP t2kreweight-config ARGS --libs OUTPUT_VARIABLE T2KRW_LIBS)

    PrefixList(PREFIX "-I" ARGS ${T2KRW_INC_DIRS} OUTPUT_VARIABLE T2KRW_INC_DIRS)

		cmessage(STATUS "T2KReWeight:")
		cmessage(STATUS "       LINK DIRS: ${T2KRW_LINK_DIRS}")
		cmessage(STATUS "        INC DIRS: ${T2KRW_INC_DIRS}")
		cmessage(STATUS "            LIBS: ${T2KRW_LIBS}")

		LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES ${T2KRW_INC_DIRS})
		LIST(APPEND EXTRA_LINK_DIRS ${T2KRW_LINK_DIRS})
		LIST(APPEND EXTRA_LIBS ${T2KRW_LIBS})

  else()

		if(NOT DEFINED CMAKE_CXX_STANDARD OR "${CMAKE_CXX_STANDARD} " STREQUAL " ")
		  SET(CMAKE_CXX_STANDARD 11)
		endif()

		LIST(APPEND EXTRA_CXX_FLAGS -DT2KRW_OA2021_INTERFACE -D__T2KREW_ENABLED__)

		GetLibDirs(CONFIG_APP t2kreweight-config ARGS --linkflags OUTPUT_VARIABLE T2KRW_LINK_DIRS)
		GetIncDirs(CONFIG_APP t2kreweight-config ARGS --cflags OUTPUT_VARIABLE T2KRW_INC_DIRS)
		GetLibs(CONFIG_APP t2kreweight-config ARGS --linkflags OUTPUT_VARIABLE T2KRW_LIBS)

		cmessage(STATUS "T2KReWeight:")
		cmessage(STATUS "       LINK DIRS: ${T2KRW_LINK_DIRS}")
		cmessage(STATUS "        INC DIRS: ${T2KRW_INC_DIRS}")
		cmessage(STATUS "            LIBS: ${T2KRW_LIBS}")

		LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES ${T2KRW_INC_DIRS})
		LIST(APPEND EXTRA_LINK_DIRS ${T2KRW_LINK_DIRS})
		LIST(APPEND EXTRA_LIBS ${T2KRW_LIBS})


		execute_process (COMMAND t2kreweight-config
		  --has-feature NIWG RESULT_VARIABLE T2KRW_HAS_NIWG)

		if("${T2KRW_HAS_NIWG} " STREQUAL "0 ")

			GetLibDirs(CONFIG_APP t2kreweight-config ARGS --niwgflags OUTPUT_VARIABLE NIWG_LINK_DIRS)
			GetIncDirs(CONFIG_APP t2kreweight-config ARGS --niwgflags OUTPUT_VARIABLE NIWG_INC_DIRS)
			GetLibs(CONFIG_APP t2kreweight-config ARGS --niwgflags OUTPUT_VARIABLE NIWG_LIBS)

			cmessage(STATUS "NIWG:")
			cmessage(STATUS "       LINK DIRS: ${NIWG_LINK_DIRS}")
			cmessage(STATUS "        INC DIRS: ${NIWG_INC_DIRS}")
			cmessage(STATUS "            LIBS: ${NIWG_LIBS}")

			LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES ${NIWG_INC_DIRS})
			LIST(APPEND EXTRA_LINK_DIRS ${NIWG_LINK_DIRS})
			LIST(APPEND EXTRA_LIBS ${NIWG_LIBS})

		endif()

		execute_process (COMMAND t2kreweight-config
		  --has-feature NEUT RESULT_VARIABLE T2KRW_HAS_NEUT)

		if("${T2KRW_HAS_NEUT} " STREQUAL "0 ")

			LIST(APPEND EXTRA_CXX_FLAGS -DNEUT_EVENT_ENABLED)
			set(USE_NEUT_EVENT TRUE)
			set(USE_NEUT_EVENT TRUE CACHE BOOL "Whether to enable NEUT (event i/o) support. Requires external libraries. <FALSE>" FORCE)


			GetLibDirs(CONFIG_APP t2kreweight-config ARGS --neutflags OUTPUT_VARIABLE NEUT_LINK_DIRS)
			GetIncDirs(CONFIG_APP t2kreweight-config ARGS --neutflags OUTPUT_VARIABLE NEUT_INC_DIRS)
			GetLibs(CONFIG_APP t2kreweight-config ARGS --neutflags OUTPUT_VARIABLE NEUT_LIBS)

			cmessage(STATUS "NEUT:")
			cmessage(STATUS "       LINK DIRS: ${NEUT_LINK_DIRS}")
			cmessage(STATUS "        INC DIRS: ${NEUT_INC_DIRS}")
			cmessage(STATUS "            LIBS: ${NEUT_LIBS}")

			LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES ${NEUT_INC_DIRS})
			LIST(APPEND EXTRA_LINK_DIRS ${NEUT_LINK_DIRS})
			LIST(APPEND EXTRA_LIBS ${NEUT_LIBS})

		endif()

		execute_process (COMMAND t2kreweight-config
		  --has-feature GEANT RESULT_VARIABLE T2KRW_HAS_GEANT)

		if("${T2KRW_HAS_GEANT} " STREQUAL "0 ")

			GetLibDirs(CONFIG_APP t2kreweight-config ARGS --geantflags OUTPUT_VARIABLE GEANT_LINK_DIRS)
			GetIncDirs(CONFIG_APP t2kreweight-config ARGS --geantflags OUTPUT_VARIABLE GEANT_INC_DIRS)
			GetLibs(CONFIG_APP t2kreweight-config ARGS --geantflags OUTPUT_VARIABLE GEANT_LIBS)

			cmessage(STATUS "GEANT:")
			cmessage(STATUS "       LINK DIRS: ${GEANT_LINK_DIRS}")
			cmessage(STATUS "        INC DIRS: ${GEANT_INC_DIRS}")
			cmessage(STATUS "            LIBS: ${GEANT_LIBS}")

			LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES ${GEANT_INC_DIRS})
			LIST(APPEND EXTRA_LINK_DIRS ${GEANT_LINK_DIRS})
			LIST(APPEND EXTRA_LIBS ${GEANT_LIBS})

		endif()

		execute_process (COMMAND t2kreweight-config
		  --has-feature ND280 RESULT_VARIABLE T2KRW_HAS_ND280)

		if("${T2KRW_HAS_ND280} " STREQUAL "0 ")

			GetLibDirs(CONFIG_APP t2kreweight-config ARGS --nd280flags OUTPUT_VARIABLE ND280_LINK_DIRS)
			GetIncDirs(CONFIG_APP t2kreweight-config ARGS --nd280flags OUTPUT_VARIABLE ND280_INC_DIRS)
			GetLibs(CONFIG_APP t2kreweight-config ARGS --nd280flags OUTPUT_VARIABLE ND280_LIBS)

			cmessage(STATUS "ND280:")
			cmessage(STATUS "       LINK DIRS: ${ND280_LINK_DIRS}")
			cmessage(STATUS "        INC DIRS: ${ND280_INC_DIRS}")
			cmessage(STATUS "            LIBS: ${ND280_LIBS}")

			LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES ${ND280_INC_DIRS})
			LIST(APPEND EXTRA_LINK_DIRS ${ND280_LINK_DIRS})
			LIST(APPEND EXTRA_LIBS ${ND280_LIBS})

		endif()

	endif()

else()

	if(T2KREWEIGHT STREQUAL "")
	  cmessage(FATAL_ERROR "Variable T2KREWEIGHT is not defined. Either configure with -DT2KREWEIGHT or \"\$ export T2KREWEIGHT=/path/to/T2KReWeight\". This must be set to point to a prebuilt T2KReWeight instance.")
	endif()

	LIST(APPEND EXTRA_CXX_FLAGS -D__T2KREW_ENABLED__ )

	# First check the OAANALYSIS libs (need to grab some headers for T2KReWeight linking if compiled with oaAnalysisReader)
	IF(NOT $ENV{OAANALYSISREADERROOT} STREQUAL "")
	  cmessage(STATUS "Found OAANALYSISREADERROOT $ENV{OAANALYSISREADERROOT}, appending...")
	  LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES $ENV{OAANALYSISREADERROOT}/$ENV{OAANALYSISREADERCONFIG})
	  LIST(APPEND EXTRA_LINK_DIRS $ENV{OAANALYSISREADERROOT}/$ENV{OAANALYSISREADERCONFIG})
	  LIST(APPEND EXTRA_LIBS oaAnalysisReader)
	  # Don't have to append this; should be appeneded in ${T2KReWeight/src/T2KBuild.h}
	  #LIST(APPEND EXTRA_CXX_FLAGS -D__T2KRW_OAANALYSIS_ENABLED__)
	endif()

	LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES ${T2KREWEIGHT}/src/)
	LIST(APPEND EXTRA_LINK_DIRS ${T2KREWEIGHT}/lib)
	LIST(APPEND EXTRA_LIBS T2KReWeight)

endif()