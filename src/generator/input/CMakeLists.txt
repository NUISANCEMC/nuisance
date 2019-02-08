LIST(APPEND INPUT_HANDLERS_LINK_LIBS nuis_event nuis_config)

if(USE_NuWro)
  LIST(APPEND INPUT_HANDLERS_IMPL NuWroInputHandler.cxx)
  LIST(APPEND INPUT_HANDLERS_LINK_LIBS nuis_generator_utility)
endif(USE_NuWro)

if(USE_NEUT)
  LIST(APPEND INPUT_HANDLERS_IMPL NEUTInputHandler.cxx)
  LIST(APPEND INPUT_HANDLERS_LINK_LIBS nuis_generator_utility)
endif(USE_NEUT)

if(USE_GENIE)
  LIST(APPEND INPUT_HANDLERS_IMPL GENIEInputHandler.cxx)
  LIST(APPEND INPUT_HANDLERS_LINK_LIBS nuis_generator_utility)
endif(USE_GENIE)

if(INPUT_HANDLERS_IMPL)
  add_library(InputHandlers SHARED ${INPUT_HANDLERS_IMPL})
  target_link_libraries(InputHandlers ${INPUT_HANDLERS_LINK_LIBS})

  if(USE_GENIE)
    target_compile_options(InputHandlers PUBLIC ${GENIE_CXX_FLAGS})
    include_directories(${GENIE_INCLUDE_DIRS})

    target_link_libraries(InputHandlers ${GENIE_LINK_DIRS})
    target_link_libraries(InputHandlers ${GENIE_LIBS})
  endif()

  install(TARGETS InputHandlers DESTINATION plugins)
endif()
