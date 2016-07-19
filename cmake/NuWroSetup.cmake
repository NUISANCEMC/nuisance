if(NOT DEFINED ENV{NUWRO})

  cmessage(FATAL_ERROR "Environment variable NUWRO is not defined. "
    "This must be set to point to a prebuilt NuWro instance.")

endif()

set(NUWRO $ENV{NUWRO})

if(DEFINED USE_EXP AND USE_EXP)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNW_READHISTFROMINP")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__NUWRO_ENABLED__ -D__NUWRO_REWEIGHT_ENABLED__ ")

set(RWENGINE_INCLUDE_DIRECTORIES ${RWENGINE_INCLUDE_DIRECTORIES} ${NUWRO}/src ${NUWRO}/src/reweight ${NUWRO}/build/src)

set(RWENGINE_LINKER_FLAGS "${RWENGINE_LINKER_FLAGS} -L${NUWRO}/build/${CMAKE_SYSTEM_NAME}/lib -lreweight -levent")

set(NEED_PYTHIA6 TRUE)
set(NEED_ROOTPYTHIA6 TRUE)
