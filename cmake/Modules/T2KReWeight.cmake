find_program(T2KRWCONFIG NAMES t2kreweight-config)

SET(T2KReWeight_ENABLED FALSE)
SET(T2KReWeight_LEGACY_API_ENABLED FALSE)

if(NOT "${T2KRWCONFIG}x" STREQUAL "T2KRWCONFIG-NOTFOUNDx")
  cmessage(STATUS "Found t2kreweight-config, using it to determine configuration.")

  #this stops T2KReWeight modules colliding with ones produced here
  SET(OLD_CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}")
  UNSET(CMAKE_MODULE_PATH)
  find_package(T2KReWeight)
  SET(CMAKE_MODULE_PATH "${OLD_CMAKE_MODULE_PATH}")

  #If we are using the latest T2KReWeight, we don't want to link directly to NIWG
  if(NIWGLegacy_REQUIRED)
    cmessage(FATAL_ERROR "NIWGLegacy_ENABLED was set but we are building against current T2KReWeight, please use NIWGReWeight parameters via T2KReWeight rather than requesting direct linking.")
  endif()
  SET(NIWGLegacy_ENABLED FALSE)

else()
  include(FindT2KReWeightLegacy)

  if(T2KReWeight_FOUND)
    SET(T2KReWeight_LEGACY_API_ENABLED FALSE)
  endif()
endif()

if(T2KReWeight_FOUND)
    target_link_libraries(GeneratorCompileDependencies INTERFACE T2KReWeight::All)
endif()