###### FHICL set up
include(ExternalProject)

ExternalProject_Add(fhiclcpp-simple
  PREFIX "${PROJECT_BINARY_DIR}/fhiclcpp-simple"
  GIT_REPOSITORY https://github.com/luketpickering/fhiclcpp-simple.git
  GIT_TAG stable
  UPDATE_DISCONNECTED 1
  CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
  -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DSETUPFILENAME=fhiclcpp.setup.sh)

include_directories(${CMAKE_INSTALL_PREFIX}/include)
