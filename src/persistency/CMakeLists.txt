set(persistency_implementation_files
ROOTOutput.cxx)

set(persistency_header_files
ROOTOutput.hxx)

add_library(nuis_persistency SHARED ${persistency_implementation_files})
add_dependencies(nuis_persistency nuis_config fhiclcpp-simple)

install(TARGETS nuis_persistency DESTINATION lib)
install(FILES ${persistency_header_files} DESTINATION include/persistency)
