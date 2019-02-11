configure_file(setup.sh.in
  ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/setup.sh @ONLY)
configure_file(nuisbuild.in
  ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/nuisbuild @ONLY)
configure_file(nuiseventprocgen.in
  ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/nuiseventprocgen @ONLY)

install(
  FILES
    ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/setup.sh
  DESTINATION
    ${CMAKE_INSTALL_PREFIX}
  )

install(
  FILES
    SimpleDataComparison.cxx.in SimpleMCStudy.cxx.in
  DESTINATION
    file_stubs
  )

install(
  PROGRAMS
    ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/nuisbuild
    ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/nuiseventprocgen
  DESTINATION
    bin
  )
