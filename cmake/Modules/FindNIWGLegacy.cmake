if(NOT TARGET NIWGLegacy::All)

EnsureVarOrEnvSet(NIWG NIWG)

if("${NIWG}" STREQUAL "NIWG-NOTFOUND")
  cmessage(STATUS "Environment variable: NIWG not set, assuming no NIWG (Legacy) support required.")
  set(NIWGLegacy_FOUND FALSE)
  return()
endif()

find_path(NIWGLegacy_INCLUDE_DIR
  NAMES NIWGSyst.h
  PATHS ${NIWG}
)

find_path(NIWGLegacy_LIB_DIR
  NAMES libNIWGReWeight.so
  PATHS ${NIWG}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NIWGLegacy
    REQUIRED_VARS 
      NIWGLegacy_INCLUDE_DIR
      NIWGLegacy_LIB_DIR
)

  if(NIWGLegacy_FOUND)

    cmessage(STATUS "NIWG Found (Legacy): ${NIWGLegacy}")
    cmessage(STATUS "    NIWGLegacy_INCLUDE_DIR: ${NIWGLegacy_INCLUDE_DIR}")
    cmessage(STATUS "    NIWGLegacy_LIB_DIR: ${NIWGLegacy_LIB_DIR}")
    cmessage(STATUS "    NIWGLegacy_LIBS: libNIWGReWeight.so")

    if(NOT TARGET NIWGLegacy::All)
        add_library(NIWGLegacy::All INTERFACE IMPORTED)
        set_target_properties(NIWGLegacy::All PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${NIWGLegacy_INCLUDE_DIR}"
            INTERFACE_LINK_DIRECTORIES "${NIWGLegacy_LIB_DIR}"
            INTERFACE_LINK_LIBRARIES NIWGReWeight
        )
    endif()
  endif()
endif()

if(NOT NIWGLegacy_FOUND)
  if(NIWGLegacy_REQUIRED)
    cmessage(FATAL_ERROR "NIWGLegacy was explicitly enabled but cannot be found.")
  endif()
  SET(NIWGLegacy_ENABLED FALSE)
else()
  target_link_libraries(GeneratorCompileDependencies INTERFACE NIWGLegacy::All)
endif()