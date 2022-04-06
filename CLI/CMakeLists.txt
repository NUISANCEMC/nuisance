configure_file(nuis-config.in nuis-config @ONLY)

install(PROGRAMS
  nuis
  nuis-flux
  nuis-gen
  nuis-prep
  nuis-comp
  ${CMAKE_CURRENT_BINARY_DIR}/nuis-config
  DESTINATION bin)

if(NEUT_ENABLED)
  install(PROGRAMS
    nuis-gen-NEUT
    nuis-prep-NEUT
  DESTINATION bin)
endif()

if(GENIE_ENABLED)
  install(PROGRAMS
    nuis-gen-GENIE
    nuis-prep-GENIE
  DESTINATION bin)
endif()
