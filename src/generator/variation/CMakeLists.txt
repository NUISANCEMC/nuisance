LIST(APPEND WEIGHT_ENGINES_LINK_LIBS nuis_event nuis_config nuis_params)

if(USE_NEUT)
  LIST(APPEND WEIGHT_ENGINES_IMPL NEUTWeightEngine.cxx)
  LIST(APPEND WEIGHT_ENGINES_LINK_LIBS nuis_generator_utility)
endif(USE_NEUT)

if(WEIGHT_ENGINES_IMPL)
  add_library(generator_weight_engines SHARED ${WEIGHT_ENGINES_IMPL})
  target_link_libraries(generator_weight_engines ${WEIGHT_ENGINES_LINK_LIBS})

  install(TARGETS generator_weight_engines DESTINATION plugins)
endif()
