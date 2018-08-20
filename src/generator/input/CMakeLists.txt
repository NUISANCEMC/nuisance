LIST(APPEND INPUT_HANDLERS_LINK_LIBS nuis_event nuis_config)

if(USE_NuWro)
  LIST(APPEND INPUT_HANDLERS_IMPL NuWroInputHandler.cxx)
endif(USE_NuWro)

if(USE_NEUT)
  LIST(APPEND INPUT_HANDLERS_IMPL NEUTInputHandler.cxx)
  LIST(APPEND INPUT_HANDLERS_LINK_LIBS nuis_generator_utility)
endif(USE_NEUT)

if(INPUT_HANDLERS_IMPL)
  add_library(InputHandlers SHARED ${INPUT_HANDLERS_IMPL})
  target_link_libraries(InputHandlers ${INPUT_HANDLERS_LINK_LIBS})

  install(TARGETS InputHandlers DESTINATION plugins)
endif()
