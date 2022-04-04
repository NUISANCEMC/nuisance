configure_file(nuis-config.in nuis-config @ONLY)

install(PROGRAMS
  nuis
  nuis-flux
  nuis-gen
  ${CMAKE_CURRENT_BINARY_DIR}/nuis-config
  DESTINATION bin)