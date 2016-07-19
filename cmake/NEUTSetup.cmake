
if(NOT DEFINED ENV{NEUT_ROOT} OR $ENV{NEUT_ROOT} STREQUAL "")

  cmessage(FATAL_ERROR "Environment variable NEUT_ROOT is not defined. "
    "This must be set to point to a prebuilt NEUT instance.")

endif()

if(NOT DEFINED ENV{CERN} OR $ENV{CERN} STREQUAL "")

  cmessage(FATAL_ERROR "Environment variable CERN is not defined. "
    "This must be set to point to a prebuilt CERNLIB instance.")

endif()

if(NOT DEFINED ENV{CERN_LEVEL} OR $ENV{CERN_LEVEL} STREQUAL "")

  cmessage(FATAL_ERROR "Environment variable CERN_LEVEL is not defined. "
    "This must be set correctly for a prebuilt CERNLIB instance.")

endif()

set(NEUT_ROOT $ENV{NEUT_ROOT})
set(NEUT_LIB_DIR ${NEUT_ROOT}/lib/Linux_pc)
set(CERN $ENV{CERN})
set(CERN_LEVEL $ENV{CERN_LEVEL})
set(NEUT_CLASS ${NEUT_ROOT}/src/neutclass)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__NEUT_ENABLED__ ")

set(RWENGINE_INCLUDE_DIRECTORIES ${RWENGINE_INCLUDE_DIRECTORIES} ${NEUT_ROOT}/include  ${NEUT_ROOT}/src/neutclass ${NEUT_ROOT}/src/reweight)

set(RWENGINE_LINKER_FLAGS "-L${NEUT_ROOT}/lib/${CMAKE_SYSTEM_NAME} -L${NEUT_ROOT}/lib/Linux_pc  -lNReWeight -L${CERN}/${CERN_LEVEL}/lib -ljetset74 -lpdflib804 -lmathlib -lpacklib -lpawlib ${NEUT_CLASS}/neutctrl.so ${NEUT_CLASS}/neutfsivert.so ${NEUT_CLASS}/neutnucfsivert.so ${NEUT_CLASS}/neutrootTreeSingleton.so ${NEUT_CLASS}/neutvtx.so ${NEUT_CLASS}/neutfsipart.so  ${NEUT_CLASS}/neutnucfsistep.so ${NEUT_CLASS}/neutpart.so ${NEUT_CLASS}/neutvect.so -L${NEUT_ROOT}/lib/Linux_pc  -lneutcore -lnuccorrspl -lnuceff -lpartnuck -lskmcsvc -ltauola -L${NEUT_ROOT}/src/reweight -lNReWeight")
