SET(TESTS InputHandlerCheck)

foreach(a ${TESTS})
  add_executable(${a} ${a}.cxx)
  target_link_libraries(${a} -Wl,--no-as-needed nuis_event nuis_input nuis_utility_experimental nuis_utility nuis_config nuis_persistency nuis_plugins nuis_params nuis_variation InputHandlers)
  target_link_libraries(${a} ${GENERATOR_DEPENDENT_TARGETS} -Wl,--as-needed)
  target_link_libraries(${a} ${NUISANCE_LINK_DIRS})
  target_link_libraries(${a} ${NUISANCE_DEPEND_LIBS})

  if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
    set_target_properties(${a} PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
  endif()

    if(USE_GENIE)
      target_compile_options(${a} PUBLIC ${GENIE_CXX_FLAGS})
      include_directories(${GENIE_INCLUDE_DIRS})
    endif()

    if(USE_NEUT)
      target_compile_options(${a} PUBLIC ${NEUT_CXX_FLAGS})
      include_directories(${NEUT_INCLUDE_DIRS})
    endif()

    if(USE_NUWRO)
      target_compile_options(${a} PUBLIC ${NUWRO_CXX_FLAGS})
      include_directories(${NUWRO_INCLUDE_DIRS})
    endif()

  install(TARGETS ${a} DESTINATION test)
endforeach()
