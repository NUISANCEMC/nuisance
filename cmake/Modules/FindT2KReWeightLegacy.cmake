if(NOT TARGET T2KReWeight::All)

  EnsureVarOrEnvSet(T2KREWEIGHT T2KREWEIGHT)

  if("${T2KREWEIGHT}" STREQUAL "T2KREWEIGHT-NOTFOUND")
    cmessage(STATUS "Environment variable: T2KREWEIGHT not set, assuming no T2KReWeight (Legacy) support required.")
    set(T2KReWeight_FOUND FALSE)
    set(T2KReWeight_ENABLED FALSE)
    return()
  endif()

  find_path(T2KReWeight_INCLUDE_DIR
    NAMES T2KBuild.h
    PATHS ${T2KREWEIGHT}/include
  )

  find_path(T2KReWeight_LIB_DIR
    NAMES libT2KReWeight.so
    PATHS ${T2KREWEIGHT}/lib
  )

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(T2KReWeight
      REQUIRED_VARS 
        T2KReWeight_INCLUDE_DIR
        T2KReWeight_LIB_DIR
  )

  if(T2KReWeight_FOUND)

    cmessage(STATUS "T2KReWeight Found (Legacy): ${T2KREWEIGHT}")
    cmessage(STATUS "    T2KReWeight_INCLUDE_DIR: ${T2KReWeight_INCLUDE_DIR}")
    cmessage(STATUS "    T2KReWeight_LIB_DIR: ${T2KReWeight_LIB_DIR}")
    cmessage(STATUS "    T2KReWeight_LIBS: T2KReWeight")

    if(NOT TARGET T2KReWeight::All)
        add_library(T2KReWeight::All INTERFACE IMPORTED)
        set_target_properties(T2KReWeight::All PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${T2KReWeight_INCLUDE_DIR}"
            INTERFACE_COMPILE_OPTIONS -DT2KReWeight_LEGACY_API_ENABLED
            INTERFACE_LINK_DIRECTORIES "${T2KReWeight_LIB_DIR}"
            INTERFACE_LINK_LIBRARIES T2KReWeight
        )
    endif()
  endif()
endif()