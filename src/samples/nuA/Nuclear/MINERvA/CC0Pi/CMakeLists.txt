SET(SAMPLES
  MINERvA_CC0PiNProt_CH_xsec_STV_nu)

foreach(S ${SAMPLES})
  LIST(APPEND INuADataComparisons ${S})
  LIST(APPEND SAMPLES_src ${S}.cxx)
endforeach()

add_library(MINERvACC0PiDataComparisons SHARED ${SAMPLES_src})
target_link_libraries(MINERvACC0PiDataComparisons
  nuis_event nuis_input nuis_config nuis_persistency)

install(TARGETS MINERvACC0PiDataComparisons DESTINATION plugins)

install(FILES MINERvA.CC0Pi.sample.config.fcl DESTINATION fcl)

SET(INuADataComparisons ${INuADataComparisons} PARENT_SCOPE)
