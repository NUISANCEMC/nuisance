set(IMPLFILES
minimizerFCN.cxx
SampleList.cxx
)

set(HEADERFILES
minimizerFCN.h
SampleList.h
)

set(LIBNAME FCN)

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  add_library(${LIBNAME} STATIC ${IMPLFILES})
else(CMAKE_BUILD_TYPE MATCHES RELEASE)
  add_library(${LIBNAME} SHARED ${IMPLFILES})
endif()

include_directories(${RWENGINE_INCLUDE_DIRECTORIES})
include_directories(${EXP_INCLUDE_DIRECTORIES})

include_directories(${CMAKE_SOURCE_DIR}/src/Utils)
include_directories(${CMAKE_SOURCE_DIR}/src/FitBase)

include_directories(${CMAKE_SOURCE_DIR}/src/ANL)
include_directories(${CMAKE_SOURCE_DIR}/src/BNL)
include_directories(${CMAKE_SOURCE_DIR}/src/FNAL)
include_directories(${CMAKE_SOURCE_DIR}/src/GGM)
include_directories(${CMAKE_SOURCE_DIR}/src/MINERvA)
include_directories(${CMAKE_SOURCE_DIR}/src/MiniBooNE)
include_directories(${CMAKE_SOURCE_DIR}/src/T2K)

include_directories(${CMAKE_SOURCE_DIR}/src/MCStudies)
include_directories(${CMAKE_SOURCE_DIR}/src/FCN)


set_target_properties(${LIBNAME} PROPERTIES VERSION
  "${ExtFit_VERSION_MAJOR}.${ExtFit_VERSION_MINOR}.${ExtFit_VERSION_REVISION}")
set_target_properties(${LIBNAME} PROPERTIES LINK_FLAGS ${ROOT_LD_FLAGS})

install(TARGETS ${LIBNAME} DESTINATION lib)
#Can uncomment this to install the headers... but is it really neccessary?
#install(FILES ${HEADERFILES} DESTINATION include)

set(MODULETargets ${MODULETargets} ${LIBNAME} PARENT_SCOPE)
