if(USE_NEUT)
  LIST(APPEND WEIGHT_ENGINES_IMPL NEUTWeightEngine.cxx)
  LIST(APPEND WEIGHT_ENGINES_LINK_LIBS nuis_generator_utility)
endif(USE_NEUT)

if(WEIGHT_ENGINES_IMPL)
  add_library(generator_weight_engines SHARED ${WEIGHT_ENGINES_IMPL})
  target_link_libraries(generator_weight_engines ${WEIGHT_ENGINES_LINK_LIBS})

  if(USE_NEUT)
    target_compile_options(generator_weight_engines PUBLIC ${NEUT_CXX_FLAGS})
    include_directories(${NEUT_INCLUDE_DIRS})

    target_link_libraries(generator_weight_engines ${NEUT_LINK_DIRS})
    target_link_libraries(generator_weight_engines -Wl,--no-as-needed ${NEUT_LIBS})
    target_link_libraries(generator_weight_engines ${NEUT_IMPORTED_TARGETS})
  endif()

  if(USE_NUWRO)
    target_compile_options(generator_weight_engines PUBLIC ${NUWRO_CXX_FLAGS})
    include_directories(${NUWRO_INCLUDE_DIRS})
    target_link_libraries(generator_weight_engines -Wl,--no-as-needed ${NUWRO_IMPORTED_TARGETS})
  endif()

  if(${NEED_PYTHIA6})
    target_link_libraries(generator_weight_engines -L${PYTHIA6})
  endif()
  target_link_libraries(generator_weight_engines -L${ROOT_LIBDIR} ${ROOT_LIBS})

  install(TARGETS generator_weight_engines DESTINATION plugins)
endif()
