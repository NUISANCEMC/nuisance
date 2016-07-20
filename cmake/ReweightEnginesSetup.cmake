set(RWENGINE_INCLUDE_DIRECTORIES "")

##################################  NEUT  ######################################
if(DEFINED USE_NEUT AND USE_NEUT)
  include(${CMAKE_SOURCE_DIR}/cmake/NEUTSetup.cmake)
  cmessage(STATUS "Using NEUT Reweight engine.")
else()
  set(USE_NEUT 0)
endif()
#################################  NuWro  ######################################
if(DEFINED USE_NuWro AND USE_NuWro)
  include(${CMAKE_SOURCE_DIR}/cmake/NuWroSetup.cmake)
  cmessage(STATUS "Using NuWro Reweight engine.")
else()
  set(USE_NuWro 0)
endif()
##################################  GENIE  #####################################
if(DEFINED USE_GENIE AND USE_GENIE)
  include(${CMAKE_SOURCE_DIR}/cmake/GENIESetup.cmake)
  cmessage(STATUS "Using GENIE Reweight engine.")
else()
  set(USE_GENIE 0)
endif()
##################################  NIWG  ######################################
if(DEFINED USE_NIWG AND USE_NIWG)
  cmessage(FATAL_ERROR "Unfortunately NIWG is not enabled for CMake build at the moment. Fix it yourself or check back later!")
  include(${CMAKE_SOURCE_DIR}/cmake/NIWGSetup.cmake)
  cmessage(STATUS "Using NIWG Reweight engine.")
else()
  set(USE_NIWG 0)
endif()

cmessage(STATUS "Reweight engine include directories: ${RWENGINE_INCLUDE_DIRECTORIES}")
