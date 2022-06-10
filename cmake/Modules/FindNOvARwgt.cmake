SET(NOvARwgt_FOUND FALSE)

if(NOT TARGET NOvARwgt::All)

EnsureVarOrEnvSet(NOVARWGT NOVARWGT)

if("${NOVARWGT}" STREQUAL "NOVARWGT-NOTFOUND")
  cmessage(STATUS "Environment variable: NOVARWGT not set, assuming no NOVARWGT support required.")
  SET(NOvARwgt_FOUND FALSE)
  SET(NOvARwgt_ENABLED FALSE)
  return()
endif()

find_path(NOvARwgt_INCLUDE_DIR
  NAMES NOvARwgt/interfaces/GenieInterface.h
  PATHS ${NOVARWGT}/inc
)

find_path(NOvARwgt_LIB_DIR
  NAMES libNOvARwgt.so
  PATHS ${NOVARWGT}/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NOvARwgt
    REQUIRED_VARS 
      NOvARwgt_INCLUDE_DIR
      NOvARwgt_LIB_DIR
)

  if(NOvARwgt_FOUND)

    cmessage(STATUS "NOvARwgt Found: ${NOvARwgt}")
    cmessage(STATUS "    NOvARwgt_INCLUDE_DIR: ${NOvARwgt_INCLUDE_DIR}")
    cmessage(STATUS "    NOvARwgt_LIB_DIR: ${NOvARwgt_LIB_DIR}")
    cmessage(STATUS "    NOvARwgt_LIBS: libNOvARwgt.so")

    if(NOT TARGET NOvARwgt::All)
        add_library(NOvARwgt::All INTERFACE IMPORTED)
        set_target_properties(NOvARwgt::All PROPERTIES
            INTERFACE_COMPILE_OPTIONS "-DNOvARwgt_ENABLED"
            INTERFACE_INCLUDE_DIRECTORIES "${NOvARwgt_INCLUDE_DIR}"
            INTERFACE_LINK_DIRECTORIES "${NOvARwgt_LIB_DIR}"
            INTERFACE_LINK_LIBRARIES NOvARwgt
        )
    endif()
  endif()
endif()

if(NOvARwgt_FOUND)
  target_link_libraries(GeneratorCompileDependencies INTERFACE NOvARwgt::All)
endif()