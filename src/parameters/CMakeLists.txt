SET(parameters_implementation_files
ParameterManager.cxx)

SET(parameters_header_files
  ParameterManager.hxx)

add_library(nuis_params SHARED ${parameters_implementation_files})
target_link_libraries(nuis_params)

install(TARGETS nuis_params DESTINATION lib)
install(FILES ${parameters_header_files} DESTINATION include/parameters)
