set(config_implementation_files
GlobalConfiguration.cxx)

set(config_header_files
GlobalConfiguration.hxx)

add_library(nuis_config SHARED ${config_implementation_files})
add_dependencies(nuis_config fhiclcpp-simple)

install(TARGETS nuis_config DESTINATION lib)
install(FILES ${config_header_files} DESTINATION include/config)
