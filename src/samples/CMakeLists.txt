set(samples_header_files
ISample.hxx)

install(FILES ${samples_header_files} DESTINATION include/samples)

add_subdirectory(MCTools)
