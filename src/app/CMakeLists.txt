SET(APPS nuissamples nuiscomp nuisevsum nuisstudy)

foreach(a ${APPS})
  add_executable(${a} ${a}.cxx)
  target_link_libraries(${a} -Wl,--no-as-needed nuis_event nuis_input nuis_utility nuis_config nuis_persistency nuis_plugins nuis_params nuis_variation nuis_generator -Wl,--as-needed)
  target_link_libraries(${a} ${NUISANCE_LINK_DIRS})
  target_link_libraries(${a} ${NUISANCE_DEPEND_LIBS})

  if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
    set_target_properties(${a} PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
  endif()

  install(TARGETS ${a} DESTINATION bin)
endforeach()
