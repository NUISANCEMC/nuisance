if(USE_NUWRO)
  LIST(APPEND MC_TOOL_IMPL NuisToNuWro.cxx)
endif(USE_NUWRO)

LIST(APPEND MC_TOOL_IMPL VerboseEventSummary.cxx)

add_library(MCTools SHARED ${MC_TOOL_IMPL})
target_link_libraries(MCTools ${SAMPLES_MCTOOLS_LIBS})

if(USE_NUWRO)
  target_compile_options(MCTools PUBLIC ${NUWRO_CXX_FLAGS})
  include_directories(${NUWRO_INCLUDE_DIRS})
  target_link_libraries(MCTools -Wl,--no-as-needed ${NUWRO_IMPORTED_TARGETS})
endif()

install(TARGETS MCTools DESTINATION plugins)
