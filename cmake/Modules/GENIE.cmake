find_program(GENIECONFIG NAMES genie-config)

include(CMessage)

if("${GENIECONFIG}x" STREQUAL "GENIECONFIG-NOTFOUNDx")
  cmessage(STATUS "Could not find genie-config, assuming no GENIE build")
  SET(GENIE_FOUND FALSE)
else()

	execute_process (COMMAND genie-config
    	--version OUTPUT_VARIABLE GENIE_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE)

	if(GENIE_VERSION VERSION_GREATER 2.99.99)
		include(FindGENIE3)
	else()
		include(FindGENIE2)
	endif()

endif()