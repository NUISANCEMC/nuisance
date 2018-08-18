if(USE_NuWro)
  LIST(APPEND INPUT_HANDLERS_IMPL NuWroInputHandler.cxx)
endif(USE_NuWro)

if(USE_NEUT)
  LIST(APPEND INPUT_HANDLERS_IMPL NEUTInputHandler.cxx)
endif(USE_NEUT)

add_library(InputHandlers SHARED ${INPUT_HANDLERS_IMPL})
target_link_libraries(InputHandlers nuis_core nuis_config)

install(TARGETS InputHandlers DESTINATION plugins)
