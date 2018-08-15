add_executable(DumpEventInfo DumpEventInfo.cxx)
target_link_libraries(DumpEventInfo nuis_core nuis_config)
target_link_libraries(DumpEventInfo ${CMAKE_DEPENDLIB_FLAGS})

if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
  set_target_properties(DumpEventInfo PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
endif()

install(TARGETS DumpEventInfo DESTINATION bin)
