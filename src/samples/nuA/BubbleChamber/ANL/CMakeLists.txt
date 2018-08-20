SET(SAMPLES ANL_CCQE_Evt_1DQ2_nu)

LIST(APPEND INuADataComparisons ANL_CCQE_Evt_1DQ2_nu)

foreach(S ${SAMPLES})
  LIST(APPEND SAMPLES_src ${S}.cxx)
endforeach()

add_library(ANLDataComparisons SHARED ${SAMPLES_src})
target_link_libraries(ANLDataComparisons nuis_event nuis_input nuis_config nuis_persistency)

install(TARGETS ANLDataComparisons DESTINATION plugins)

SET(INuADataComparisons ${INuADataComparisons} PARENT_SCOPE)
