if(USE_HEPMCNUEVT)
# Get libdir from
  execute_process (COMMAND HepMC3-config --includedir
    OUTPUT_VARIABLE HEPMC_INCDIR OUTPUT_STRIP_TRAILING_WHITESPACE)

  execute_process (COMMAND HepMC3-config --libdir
    OUTPUT_VARIABLE HEPMC_LIBDIR OUTPUT_STRIP_TRAILING_WHITESPACE)

  execute_process (COMMAND HepMC3-config --prefix
    OUTPUT_VARIABLE HEPMCROOT OUTPUT_STRIP_TRAILING_WHITESPACE)

  include_directories($ENV{HEPMCNUEVTTOOLSROOT}/include)
  LIST(APPEND EXTRA_LINK_DIRS $ENV{HEPMCNUEVTTOOLSROOT}/lib)
  LIST(APPEND EXTRA_LIBS HepMCNuEvtTools)

  include_directories(${HEPMC_INCDIR})
  LIST(APPEND EXTRA_LINK_DIRS ${HEPMC_LIBDIR})
  LIST(APPEND EXTRA_LIBS HepMC3 HepMC3rootIO)

  set(HEPMCNUEVTTOOLSROOT $ENV{HEPMCNUEVTTOOLSROOT})
  set(USE_HEPMCNUEVT TRUE)

endif()