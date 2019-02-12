if(USE_NUWRO)
  LIST(APPEND GENERATOR_UTILS_IMPL NuWroUtility.cxx)
  LIST(APPEND GENERATOR_UTILS_HDR NuWroUtility.hxx)
endif(USE_NUWRO)

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

  if(USE_GENIE)
    target_compile_options(nuis_generator_utility PUBLIC ${GENIE_CXX_FLAGS})
    include_directories(${GENIE_INCLUDE_DIRS})

    target_link_libraries(nuis_generator_utility ${GENIE_LINK_DIRS})
    target_link_libraries(nuis_generator_utility -Wl,--no-as-needed ${GENIE_LIBS})
  endif()

  if(USE_NEUT)
    target_compile_options(nuis_generator_utility PUBLIC ${NEUT_CXX_FLAGS})
    include_directories(${NEUT_INCLUDE_DIRS})

    target_link_libraries(nuis_generator_utility ${NEUT_LINK_DIRS})
    target_link_libraries(nuis_generator_utility -Wl,--no-as-needed ${NEUT_LIBS})
    target_link_libraries(nuis_generator_utility ${NEUT_IMPORTED_TARGETS})
  endif()

  if(USE_NUWRO)
    target_compile_options(nuis_generator_utility PUBLIC ${NUWRO_CXX_FLAGS})
    include_directories(${NUWRO_INCLUDE_DIRS})
    target_link_libraries(nuis_generator_utility -Wl,--no-as-needed ${NUWRO_IMPORTED_TARGETS})
  endif()

  if(${NEED_PYTHIA6})
      target_link_libraries(nuis_generator_utility -L${PYTHIA6})
  endif()
  target_link_libraries(nuis_generator_utility -L${ROOT_LIBDIR} ${ROOT_LIBS})

  install(TARGETS nuis_generator_utility DESTINATION plugins)
endif()

if(GENERATOR_UTILS_HDR)
  install(FILES ${GENERATOR_UTILS_HDR} DESTINATION include/generator/utility)
endif()
