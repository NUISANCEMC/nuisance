if(USE_NuWro)
  LIST(APPEND MC_TOOL_IMPL NuisToNuWro.cxx)
endif(USE_NuWro)

add_library(MCTools SHARED ${MC_TOOL_IMPL})
target_link_libraries(MCTools nuis_core nuis_config)

install(TARGETS MCTools DESTINATION plugins)
