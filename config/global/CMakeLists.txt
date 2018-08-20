configure_file(nuis.global.config.fcl.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/nuis.global.config.fcl" @ONLY)

install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/nuis.global.config.fcl" DESTINATION
${CMAKE_INSTALL_PREFIX}/fcl)

configure_file(nuis.datacomparisons.fcl.in
"${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/nuis.datacomparisons.fcl" @ONLY)

install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/nuis.datacomparisons.fcl" DESTINATION
${CMAKE_INSTALL_PREFIX}/fcl)
