
if(NOT DEFINED ENV{NEUT_ROOT})

  cmessage(FATAL_ERROR "Environment variable NEUT_ROOT is not defined. "
    "This must be set to point to a prebuilt NEUT instance.")

endif()

set(NEUT_ROOT $ENV{NEUT_ROOT})
set(NEUT_MACHINE $ENV{Machine})
set(NEUT_LIB_DIR ${NEUT_ROOT}/lib/Linux_pc)
set(CERN_ROOT $ENV{CERN_ROOT})
set(NEUT_CLASS ${NEUT_ROOT}/src/neutclass)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__NEUT_ENABLED__ ")

set(RWENGINE_INCLUDE_DIRECTORIES ${RWENGINE_INCLUDE_DIRECTORIES} ${NEUT_ROOT}/include ${NEUT_ROOT}/src/neutclass ${NEUT_ROOT}/src/reweight ${NEUT_ROOT}/src/neutcore $(CERN_ROOT)/inc $(NEUT_ROOT)/src/reweight)

set(RWENGINE_LINKER_FLAGS "-L${NEUT_ROOT}/lib/${CMAKE_SYSTEM_NAME} -L${NEUT_ROOT}/lib/Linux_pc  -lNReWeight -L${CERN_ROOT}/lib -ljetset74 -lpdflib804 -lmathlib -lpacklib -lpawlib ${NEUT_CLASS}/neutctrl.so ${NEUT_CLASS}/neutfsivert.so ${NEUT_CLASS}/neutnucfsivert.so ${NEUT_CLASS}/neutrootTreeSingleton.so ${NEUT_CLASS}/neutvtx.so ${NEUT_CLASS}/neutfsipart.so  ${NEUT_CLASS}/neutnucfsistep.so ${NEUT_CLASS}/neutpart.so ${NEUT_CLASS}/neutvect.so -L${NEUT_ROOT}/lib/Linux_pc  -lneutcore -lnuccorrspl -lnuceff -lpartnuck -lskmcsvc -ltauola -L${NEUT_ROOT}/src/reweight -lNReWeight")



#-L${NEUT_ROOT}/src/neutclass neutfsivert neutnucfsivert neutrootTreeSingleton -lneutvtx -lneutfsipart -lneutnucfsistep -lneutpart -lneutvect")

