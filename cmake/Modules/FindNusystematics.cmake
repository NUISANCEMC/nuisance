if(NOT TARGET nusystematics::All)

  SET(nusystematics_FOUND FALSE)

  message(STATUS "[JSKIMDEBUG] HERE 0")

  EnsureVarOrEnvSet(NUSYSTEMATICS_LIB NUSYSTEMATICS_LIB)
  EnsureVarOrEnvSet(NUSYSTEMATICS_INC NUSYSTEMATICS_INC)

  message(STATUS "[JSKIMDEBUG] HERE 1")

  include(FindPackageHandleStandardArgs)

  message(STATUS "[JSKIMDEBUG] HERE 2")

  find_package_handle_standard_args(nusystematics
    REQUIRED_VARS 
    NUSYSTEMATICS_INC
    NUSYSTEMATICS_LIB
  )

  message(STATUS "[JSKIMDEBUG] HERE 3, nusystematics_FOUND = ${nusystematics_FOUND}")

  if(nusystematics_FOUND)

    SET(NUSYSTEMATICS_LIBS "nusystematics_systproviders;systematicstools_interface;systematicstools_interpreters;systematicstools_systproviders;systematicstools_utility")

    cmessage(STATUS "NUSYSTEMATICS INC_DIRS: ${NUSYSTEMATICS_INC}")
    cmessage(STATUS "NUSYSTEMATICS LIB_DIRS: ${NUSYSTEMATICS_LIB}")
    cmessage(STATUS "NUSYSTEMATICS LIBS: ${NUSYSTEMATICS_LIBS}")

    add_library(nusystematics::All INTERFACE IMPORTED)
    set_target_properties(nusystematics::All PROPERTIES
      INTERFACE_COMPILE_OPTIONS "-DNO_ART"
      INTERFACE_INCLUDE_DIRECTORIES "${NUSYSTEMATICS_INC}"
      INTERFACE_LINK_DIRECTORIES "${NUSYSTEMATICS_LIB}"
      INTERFACE_LINK_LIBRARIES "${NUSYSTEMATICS_LIBS}"
      )

  endif()

endif()
