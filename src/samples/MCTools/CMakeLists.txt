LIST(APPEND SAMPLES_MCTOOLS_LIBS nuis_event nuis_input nuis_config)

if(USE_NuWro)
  LIST(APPEND MC_TOOL_IMPL NuisToNuWro.cxx)
  LIST(APPEND SAMPLES_MCTOOLS_LIBS nuis_generator_utility)
endif(USE_NuWro)

LIST(APPEND MC_TOOL_IMPL VerboseEventSummary.cxx)

add_library(MCTools SHARED ${MC_TOOL_IMPL})
target_link_libraries(MCTools ${SAMPLES_MCTOOLS_LIBS})

install(TARGETS MCTools DESTINATION plugins)
