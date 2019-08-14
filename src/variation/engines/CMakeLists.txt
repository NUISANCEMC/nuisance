if(USE_T2K)
  include(${CMAKE_SOURCE_DIR}/cmake/T2KReWeightSetup.cmake)
  LIST(APPEND WEIGHT_ENGINES_IMPL T2KReWeight.cxx)
  LIST(APPEND WEIGHT_ENGINES_LINK_LIBS nuis_generator_utility)
endif(USE_T2K)

if(WEIGHT_ENGINES_IMPL)
  add_library(variation_engines SHARED ${WEIGHT_ENGINES_IMPL})
  target_link_libraries(variation_engines ${WEIGHT_ENGINES_LINK_LIBS})

  if(USE_NEUT)
    target_compile_options(variation_engines PUBLIC ${NEUT_CXX_FLAGS})
    include_directories(${NEUT_INCLUDE_DIRS})

    target_link_libraries(variation_engines ${NEUT_LINK_DIRS})
    target_link_libraries(variation_engines -Wl,--no-as-needed ${NEUT_LIBS})
    target_link_libraries(variation_engines ${NEUT_IMPORTED_TARGETS})
  endif()

  if(USE_T2K)
    include_directories(${T2K_INCLUDE_DIRS})

    target_link_libraries(variation_engines ${T2K_LINK_DIRS})
    target_link_libraries(variation_engines ${T2K_LIBS})
  endif()

  if(USE_NUWRO)
    target_compile_options(variation_engines PUBLIC ${NUWRO_CXX_FLAGS})
    include_directories(${NUWRO_INCLUDE_DIRS})
    target_link_libraries(variation_engines -Wl,--no-as-needed ${NUWRO_IMPORTED_TARGETS})
  endif()

  if(${NEED_PYTHIA6})
    target_link_libraries(variation_engines -L${PYTHIA6})
  endif()
  target_link_libraries(variation_engines -L${ROOT_LIBDIR} ${ROOT_LIBS})

  install(TARGETS variation_engines DESTINATION plugins)
endif()
