function(GETFIRSTMATCHINGDELMIMMEDDIR DELIM CONFIGAPP ARG DIR_OUT FAILURE_IS_NOT_ERROR)

  cmessage(DEBUG "CONFIGAPP: ${CONFIGAPP}, ARG: ${ARG} DIR_OUT: ${DIR_OUT}, FAILURE_IS_NOT_ERROR: ${FAILURE_IS_NOT_ERROR}")

  if(DELIM STREQUAL "")
    cmessage(FATAL_ERROR "GETFIRSTMATCHINGDELMIMMEDDIR Passed no delimiter. This is a build configuration bug in NUISANCE, please report to the developers.")
  endif()

  if(CONFIGAPP STREQUAL "")
    cmessage(FATAL_ERROR "GETFIRSTMATCHINGDELMIMMEDDIR Passed no configuration application. This is a build configuration bug in NUISANCE, please report to the developers.")
  endif()

  SET(CONFIGAPP_LOCATION "CONFIGAPP_LOCATION-NOTFOUND")
  find_program(CONFIGAPP_LOCATION ${CONFIGAPP})
  if(NOT CONFIGAPP_LOCATION STREQUAL "CONFIGAPP_LOCATION-NOTFOUND")
    execute_process (COMMAND ${CONFIGAPP_LOCATION}
    ${ARG} OUTPUT_VARIABLE CONFIGAPP_RESPONSE_RAW OUTPUT_STRIP_TRAILING_WHITESPACE)

    cmessage(DEBUG "${CONFIGAPP_LOCATION} ${ARG} responded with: \"${CONFIGAPP_RESPONSE_RAW}\"")

    if(CONFIGAPP_RESPONSE_RAW STREQUAL "")
      if(FAILURE_IS_NOT_ERROR)
        cmessage(DEBUG "\"${CONFIGAPP_LOCATION} ${ARG}\" produced no output and was expected to.")
        set(${DIR_OUT} "" PARENT_SCOPE)
      else()
        cmessage(FATAL_ERROR "\"${CONFIGAPP_LOCATION} ${ARG}\" produced no output and was required to.")
      endif()
    else()
      string(REGEX MATCH "${DELIM}\([^ ]+\)" PARSE_CONFIGAPP_RESPONSE_MATCH ${CONFIGAPP_RESPONSE_RAW})

      if(NOT PARSE_CONFIGAPP_RESPONSE_MATCH)
        if(FAILURE_IS_NOT_ERROR)
          cmessage(DEBUG "Couldn't find ${DELIM} flag, found: \"${CONFIGAPP_RESPONSE_RAW}\"")
          set(${CMAKE_MATCH_1} "")
        else()
          cmessage(FATAL_ERROR "Expected to be able to parse the result of ${CONFIGAPP} ${ARG} to a lib directory, but couldn't find a ${DELIM} flag, found: \"${CONFIGAPP_RESPONSE_RAW}\"")
        endif()
      endif()

      set(${DIR_OUT} ${CMAKE_MATCH_1} PARENT_SCOPE)
    endif()
  else()
    cmessage(FATAL_ERROR "[ERROR]: Failed to find dependency configuration application: \"${CONFIGAPP}\"")
  endif()
endfunction()

#Uselike GETLIBDIR(gsl-config --libs GSL_LIB_DIR)
function(GETLIBDIR CONFIGAPP ARG LIBDIR_OUT)
  if(ARGN)
    set(FAILURE_IS_NOT_ERROR TRUE)
  else()
    set(FAILURE_IS_NOT_ERROR FALSE)
  endif()
  GETFIRSTMATCHINGDELMIMMEDDIR(
    "-L"
    ${CONFIGAPP}
    ${ARG}
    MATCHING_DIR
    ${FAILURE_IS_NOT_ERROR})
  set(${LIBDIR_OUT} ${MATCHING_DIR} PARENT_SCOPE)
endfunction()

#Uselike GETINCDIR(gsl-config --cflags GSL_INC_DIR)
function(GETINCDIR CONFIGAPP ARG INCDIR_OUT)
  if(ARGN)
    set(FAILURE_IS_NOT_ERROR TRUE)
  else()
    set(FAILURE_IS_NOT_ERROR FALSE)
  endif()
  GETFIRSTMATCHINGDELMIMMEDDIR(
    "-I"
    ${CONFIGAPP}
    ${ARG}
    MATCHING_DIR
    ${FAILURE_IS_NOT_ERROR})
  set(${INCDIR_OUT} ${MATCHING_DIR} PARENT_SCOPE)
endfunction()

function(GETALLMATCHINGDELMIMMEDDIR DELIM CONFIGAPP ARG LIST_OUT)
  if(DELIM STREQUAL "")
    cmessage(FATAL_ERROR "GETALLMATCHINGDELMIMMEDDIR Passed no delimiter. This is a build configuration bug in NUISANCE, please report to the developers.")
  endif()

  if(CONFIGAPP STREQUAL "")
    cmessage(FATAL_ERROR "GETALLMATCHINGDELMIMMEDDIR Passed no configuration application. This is a build configuration bug in NUISANCE, please report to the developers.")
  endif()

  #cmessage(WARNING "DELIM: ${DELIM}, CONFIGAPP: ${CONFIGAPP}, ARG: ${ARG}, LIST_OUT: ${LIST_OUT}")

  SET(CONFIGAPP_LOCATION "CONFIGAPP_LOCATION-NOTFOUND")
  find_program(CONFIGAPP_LOCATION ${CONFIGAPP})
  if(NOT CONFIGAPP_LOCATION STREQUAL "CONFIGAPP_LOCATION-NOTFOUND")
    execute_process (COMMAND ${CONFIGAPP_LOCATION}
    ${ARG} OUTPUT_VARIABLE CONFIGAPP_RESPONSE_RAW OUTPUT_STRIP_TRAILING_WHITESPACE)

    string(REPLACE " " ";" CONFIGAPP_RESPONSE_LIST "${CONFIGAPP_RESPONSE_RAW}")

   # cmessage(WARNING "CONFIGAPP_RESPONSE_RAW: ${CONFIGAPP_RESPONSE_RAW}, CONFIGAPP_RESPONSE_LIST: ${CONFIGAPP_RESPONSE_LIST}")

    set(LIST_BUILD)

    foreach(I ${CONFIGAPP_RESPONSE_LIST})
      if(I)
        string(REGEX MATCH "^${DELIM}" WASMATCHED ${I})
        if(WASMATCHED)
          string(REPLACE "${DELIM}" "" I_STRIPPED "${I}")
          LIST(APPEND LIST_BUILD ${I_STRIPPED})
        endif()
      endif()
    endforeach()

    set(${LIST_OUT} ${LIST_BUILD} PARENT_SCOPE)
  else()
    cmessage(FATAL_ERROR "[ERROR]: Failed to find dependency configuration application: \"${CONFIGAPP}\"")
  endif()
endfunction()

#Uselike GETLIBDIRS(gsl-config --libs GSL_LIB_DIR)
function(GETLIBDIRS CONFIGAPP ARG LIBDIR_OUT)
  GETALLMATCHINGDELMIMMEDDIR(
    "-L"
    ${CONFIGAPP}
    ${ARG}
    MATCHING_DIR)
  set(${LIBDIR_OUT} ${MATCHING_DIR} PARENT_SCOPE)
endfunction()

#Uselike GETINCDIRS(gsl-config --cflags GSL_INC_DIR)
function(GETINCDIRS CONFIGAPP ARG INCDIR_OUT)
  GETALLMATCHINGDELMIMMEDDIR(
    "-I"
    ${CONFIGAPP}
    ${ARG}
    MATCHING_DIR)
  set(${INCDIR_OUT} ${MATCHING_DIR} PARENT_SCOPE)
endfunction()

#Uselike GETLIBS(gsl-config --libs GSL_LIB_DIR)
function(GETLIBS CONFIGAPP ARG LIBLIST_OUT)
  #cmessage(WARNING "LIBLIST_OUT: ${LIBLIST_OUT}")
  GETALLMATCHINGDELMIMMEDDIR(
    "-l"
    ${CONFIGAPP}
    ${ARG}
    MATCHING_ITEMS)
  set(${LIBLIST_OUT} ${MATCHING_ITEMS} PARENT_SCOPE)
  #cmessage(WARNING "LIBLIST_OUT: ${LIBLIST_OUT}: ${${LIBLIST_OUT}}")
endfunction()
