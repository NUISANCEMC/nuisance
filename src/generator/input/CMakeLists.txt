if(USE_NUWRO)
  LIST(APPEND INPUT_HANDLERS_IMPL NuWroInputHandler.cxx)
endif(USE_NUWRO)

if(USE_NEUT)
  LIST(APPEND INPUT_HANDLERS_IMPL NEUTInputHandler.cxx)
endif(USE_NEUT)

if(USE_GENIE)
  LIST(APPEND INPUT_HANDLERS_IMPL GENIEInputHandler.cxx)
endif(USE_GENIE)

if(INPUT_HANDLERS_IMPL)
  add_library(InputHandlers SHARED ${INPUT_HANDLERS_IMPL})
  target_link_libraries(InputHandlers ${INPUT_HANDLERS_LINK_LIBS})

  if(USE_GENIE)
    target_compile_options(InputHandlers PUBLIC ${GENIE_CXX_FLAGS})
    include_directories(${GENIE_INCLUDE_DIRS})

    target_link_libraries(InputHandlers ${GENIE_LINK_DIRS})
    target_link_libraries(InputHandlers -Wl,--no-as-needed ${GENIE_LIBS})
  endif()

  if(USE_NEUT)
    target_compile_options(InputHandlers PUBLIC ${NEUT_CXX_FLAGS})
    include_directories(${NEUT_INCLUDE_DIRS})

    target_link_libraries(InputHandlers ${NEUT_LINK_DIRS})
    target_link_libraries(InputHandlers -Wl,--no-as-needed ${NEUT_LIBS})
    target_link_libraries(InputHandlers ${NEUT_IMPORTED_TARGETS})
  endif()

  if(USE_NUWRO)
    target_compile_options(InputHandlers PUBLIC ${NUWRO_CXX_FLAGS})
    include_directories(${NUWRO_INCLUDE_DIRS})
    target_link_libraries(InputHandlers -Wl,--no-as-needed ${NUWRO_IMPORTED_TARGETS})
  endif()

  if(${NEED_PYTHIA6})
    target_link_libraries(InputHandlers -L${PYTHIA6})
  endif()
  target_link_libraries(InputHandlers -L${ROOT_LIBDIR} ${ROOT_LIBS})

  install(TARGETS InputHandlers DESTINATION plugins)
endif()
