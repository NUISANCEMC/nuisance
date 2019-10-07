SET(SAMPLES
  MINERvA_CCInclusive_CH_xsec_EAvailq3_nu)

foreach(S ${SAMPLES})
  LIST(APPEND INuADataComparisons ${S})
  LIST(APPEND SAMPLES_src ${S}.cxx)
endforeach()

add_library(MINERvADataComparisons SHARED ${SAMPLES_src})
target_link_libraries(MINERvADataComparisons
  nuis_event nuis_input nuis_config nuis_persistency)

install(TARGETS MINERvADataComparisons DESTINATION plugins)

install(FILES MINERvA.CCInclusive.sample.config.fcl DESTINATION fcl)

SET(INuADataComparisons ${INuADataComparisons} PARENT_SCOPE)
