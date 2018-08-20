SET(APPS nuissamples DumpEventInfo)

foreach(a ${APPS})
  add_executable(${a} ${a}.cxx)
  target_link_libraries(${a} nuis_event nuis_input nuis_utility nuis_config nuis_persistency)
  target_link_libraries(${a} ${CMAKE_DEPENDLIB_FLAGS})

  if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
    set_target_properties(${a} PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
  endif()

  install(TARGETS ${a} DESTINATION bin)
endforeach()
