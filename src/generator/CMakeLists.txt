
if(USE_NuWro)
  add_library(InputHandlers SHARED NuWroInputHandler.cxx)
  target_link_libraries(InputHandlers nuis_core nuis_config)

  install(TARGETS InputHandlers DESTINATION plugins)
endif(USE_NuWro)
