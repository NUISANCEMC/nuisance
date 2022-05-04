configure_file(nuis-config.in nuis-config @ONLY)

install(PROGRAMS
  nuis
  nuis-flux
  nuis-gen
  nuis-prep
  nuis-plot
  nuis-comp
  nuis-samples
  nuis-valid
  nuis-internal
  ${CMAKE_CURRENT_BINARY_DIR}/nuis-config
  DESTINATION bin)

# any internal nasty details can 
# be kept in scripts in here, these 
# will never form part of the public CLI
FILE(COPY
  internal
  DESTINATION var)

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

if(NuWro_ENABLED)
  install(PROGRAMS
    nuis-gen-NuWro
    nuis-prep-NuWro
  DESTINATION bin)
endif()
