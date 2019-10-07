SET(SAMPLES
MicroBooNE_CCInclusive_Ar40_xsec_reco_pmu_thetamu_nu)

foreach(S ${SAMPLES})
  LIST(APPEND INuADataComparisons ${S})
  LIST(APPEND SAMPLES_src ${S}.cxx)
endforeach()

add_library(MicroBooNEDataComparisons SHARED ${SAMPLES_src})
target_link_libraries(MicroBooNEDataComparisons
  nuis_event nuis_input nuis_config nuis_persistency)

install(TARGETS MicroBooNEDataComparisons DESTINATION plugins)

install(FILES MicroBooNE.CCInclusive.sample.config.fcl DESTINATION fcl)

SET(INuADataComparisons ${INuADataComparisons} PARENT_SCOPE)
