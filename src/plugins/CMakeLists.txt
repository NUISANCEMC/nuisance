SET(plugins_implementation_files
  LoadedPlugins.cxx)

set(plugins_header_files
  Instantiate.hxx
  NamedSO.hxx
  traits.hxx)

add_library(nuis_plugins SHARED ${plugins_implementation_files})
target_link_libraries(nuis_plugins)

install(TARGETS nuis_plugins DESTINATION lib)
install(FILES ${plugins_header_files} DESTINATION include/plugins)
