if(DEFINED USE_GPERFTOOLS AND USE_GPERFTOOLS)
  include(ExternalProject)

  ExternalProject_Add(libunwind
    PREFIX "${CMAKE_BINARY_DIR}/Ext"
    GIT_REPOSITORY "git://git.sv.gnu.org/libunwind.git"
    CONFIGURE_COMMAND ${CMAKE_BINARY_DIR}/Ext/src/libunwind/autogen.sh --prefix=${CMAKE_INSTALL_PREFIX}
    UPDATE_COMMAND ""
    BUILD_COMMAND make -j6
    INSTALL_COMMAND make install
    )

  ExternalProject_Add(gperftools
    PREFIX "${CMAKE_BINARY_DIR}/Ext"
    GIT_REPOSITORY "https://github.com/gperftools/gperftools.git"
    GIT_TAG "gperftools-2.5"
    CONFIGURE_COMMAND ./autogen.sh && ./configure --prefix=${CMAKE_INSTALL_PREFIX} CPPFLAGS=-I${CMAKE_INSTALL_PREFIX}/include LDFLAGS=-L${CMAKE_INSTALL_PREFIX}/lib
    BUILD_IN_SOURCE 1
    UPDATE_COMMAND ""
    BUILD_COMMAND make -j6
    INSTALL_COMMAND make install
    )

  add_dependencies(gperftools libunwind)

  set(CMAKE_CXX_FLAGS "-fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free ${CMAKE_CXX_FLAGS}")
  set(CMAKE_LINK_FLAGS "-L${CMAKE_INSTALL_PREFIX}/lib -ltcmalloc_and_profiler ${CMAKE_LINK_FLAGS}")

  cmessage(STATUS "Using google performance libraries")
endif()
