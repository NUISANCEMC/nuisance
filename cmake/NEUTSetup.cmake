
if(NOT DEFINED ENV{NEUT_ROOT})

  cmessage(FATAL_ERROR "Environment variable NEUT_ROOT is not defined. "
    "This must be set to point to a prebuilt NEUT instance.")

endif()

set(NEUT_ROOT $ENV{NEUT_ROOT})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__NEUT_ENABLED__ ")

set(RWENGINE_INCLUDE_DIRECTORIES ${RWENGINE_INCLUDE_DIRECTORIES} ${NEUT_ROOT}/include ${NEUT_ROOT}/src/neutclass ${NEUT_ROOT}/src/reweight)

set(RWENGINE_LINKER_FLAGS "-L${NEUT_ROOT}/lib/${CMAKE_SYSTEM_NAME}")

