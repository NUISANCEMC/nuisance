if(NOT TARGET NuWro::All)

EnsureVarOrEnvSet(NUWRO NUWRO)

if("${NUWRO}" STREQUAL "NUWRO-NOTFOUND")
  cmessage(STATUS "NUWRO environment variable is not defined, assuming no NuWro build")
endif()

include(FindPackageHandleStandardArgs)

find_path(NuWro_INC_DIR
  NAMES dis/dis_cc.h
  PATHS ${NUWRO}/src)

find_path(NuWro_LIB_DIR
  NAMES event1.so
  PATHS ${NUWRO}/bin)

find_package_handle_standard_args(NuWro
  REQUIRED_VARS 
    NUWRO 
    NuWro_INC_DIR 
    NuWro_LIB_DIR
)

if(NuWro_FOUND)

  cmessage(STATUS "NuWro found: ${NUWRO}")
  cmessage(STATUS "       NuWro_INC_DIR: ${NuWro_INC_DIR}")
  cmessage(STATUS "       NuWro_LIB_DIR: ${NuWro_LIB_DIR}")

  if(NOT TARGET NuWro::event1)
    add_library(NuWro::event1 UNKNOWN IMPORTED)
    set_target_properties(NuWro::event1 PROPERTIES
      IMPORTED_NO_SONAME ON
      IMPORTED_LOCATION ${NUWRO}/bin/event1.so
      )
  endif()

  if(NOT TARGET NuWro::All)
    add_library(NuWro::All INTERFACE IMPORTED)
    set_target_properties(NuWro::All PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${NuWro_INC_DIR}"
        INTERFACE_COMPILE_OPTIONS "-DNuWro_ENABLED"
        INTERFACE_LINK_LIBRARIES NuWro::event1
    )
  endif()

endif()

endif()

if(NOT NuWro_FOUND)
  if(NuWro_REQUIRED)
    cmessage(FATAL_ERROR "NuWro was explicitly enabled but cannot be found.")
  endif()
  SET(NuWro_ENABLED FALSE)
else()
  target_link_libraries(GeneratorCompileDependencies INTERFACE NuWro::All)
endif()
