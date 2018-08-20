SET(input_implementation_files
  InputManager.cxx)

SET(input_header_files
  InputManager.hxx
  IInputHandler.hxx)

add_library(nuis_input SHARED ${input_implementation_files})
target_link_libraries(nuis_input)

install(TARGETS nuis_input DESTINATION lib)
install(FILES ${input_header_files} DESTINATION include/input)
