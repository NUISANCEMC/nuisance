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
  SET(NIWGLegacy_ENABLED FALSE)

else()
  include(FindT2KReWeightLegacy)

  if(T2KReWeight_FOUND)
    SET(T2KReWeight_LEGACY_API_ENABLED FALSE)
  endif()
endif()

if(NOT T2KReWeight_FOUND)
  if(T2KReWeight_REQUIRED)
    cmessage(FATAL_ERROR "T2KReWeight was explicitly enabled but cannot be found.")
  endif()
  SET(T2KReWeight_ENABLED FALSE)
else()
    target_link_libraries(GeneratorCompileDependencies INTERFACE T2KReWeight::All)
endif()