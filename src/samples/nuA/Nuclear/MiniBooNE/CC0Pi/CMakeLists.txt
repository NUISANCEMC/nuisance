SET(SAMPLES
  MiniBooNE_CCQE_CH2_xsec_nu)

foreach(S ${SAMPLES})
  LIST(APPEND INuADataComparisons ${S})
  LIST(APPEND SAMPLES_src ${S}.cxx)
endforeach()

add_library(MiniBooNEDataComparisons SHARED ${SAMPLES_src})
target_link_libraries(MiniBooNEDataComparisons
  nuis_event nuis_input nuis_config nuis_persistency)

install(TARGETS MiniBooNEDataComparisons DESTINATION plugins)

install(FILES MiniBooNE.CC0Pi.sample.config.fcl DESTINATION fcl)

SET(INuADataComparisons ${INuADataComparisons} PARENT_SCOPE)
