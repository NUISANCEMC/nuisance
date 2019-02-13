SET(SAMPLES
  T2K_CC0Pi_H2O_xsec_2Dpcthetamu_numubar
  T2K_CC0PiNProt_CH_xsec_STV_nu)

foreach(S ${SAMPLES})
  LIST(APPEND INuADataComparisons ${S})
  LIST(APPEND SAMPLES_src ${S}.cxx)
endforeach()

add_library(T2KCC0PiDataComparisons SHARED ${SAMPLES_src})
target_link_libraries(T2KCC0PiDataComparisons
  nuis_event nuis_input nuis_config nuis_persistency)

install(TARGETS T2KCC0PiDataComparisons DESTINATION plugins)

install(FILES T2K.CC0Pi.sample.config.fcl DESTINATION fcl)

SET(INuADataComparisons ${INuADataComparisons} PARENT_SCOPE)
