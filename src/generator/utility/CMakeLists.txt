if(USE_NuWro)
  LIST(APPEND GENERATOR_UTILS_IMPL NuWroUtility.cxx)
  LIST(APPEND GENERATOR_UTILS_HDR NuWroUtility.hxx)
endif(USE_NuWro)

if(USE_NEUT)
  LIST(APPEND GENERATOR_UTILS_IMPL NEUTUtility.cxx)
  LIST(APPEND GENERATOR_UTILS_HDR NEUTUtility.hxx)
endif(USE_NEUT)

if(USE_GENIE)
  LIST(APPEND GENERATOR_UTILS_IMPL GENIEUtility.cxx)
  LIST(APPEND GENERATOR_UTILS_HDR GENIEUtility.hxx)
endif(USE_GENIE)


if(GENERATOR_UTILS_IMPL)
  add_library(nuis_generator_utility SHARED ${GENERATOR_UTILS_IMPL})
  target_link_libraries(nuis_generator_utility nuis_event)

  if(USE_GENIE)
    target_compile_options(nuis_generator_utility PUBLIC ${GENIE_CXX_FLAGS})
    include_directories(${GENIE_INCLUDE_DIRS})

    target_link_libraries(nuis_generator_utility ${GENIE_LINK_DIRS})
    target_link_libraries(nuis_generator_utility ${GENIE_LIBS})
  endif()

  install(TARGETS nuis_generator_utility DESTINATION plugins)
endif()

if(GENERATOR_UTILS_HDR)
  install(FILES ${GENERATOR_UTILS_HDR} DESTINATION include/generator/utility)
endif()
