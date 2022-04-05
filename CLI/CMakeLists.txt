configure_file(nuis-config.in nuis-config @ONLY)

install(PROGRAMS
  nuis
  nuis-flux
  nuis-gen
  ${CMAKE_CURRENT_BINARY_DIR}/nuis-config
  DESTINATION bin)

if(NEUT_ENABLED)
  install(PROGRAMS
    nuis-gen-NEUT
  DESTINATION bin)
endif()

if(GENIE_ENABLED)
  install(PROGRAMS
    nuis-gen-GENIE
  DESTINATION bin)
endif()
