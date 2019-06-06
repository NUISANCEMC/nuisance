SET(SAMPLES
T2K_CC1Pip_CH_xsec_muproj_nu
T2K_CC1Pip_CH_xsec_piproj_nu)

foreach(S ${SAMPLES})
  LIST(APPEND INuADataComparisons ${S})
  LIST(APPEND SAMPLES_src ${S}.cxx)
endforeach()

add_library(T2KCC1PiDataComparisons SHARED ${SAMPLES_src})
target_link_libraries(T2KCC1PiDataComparisons
  nuis_event nuis_input nuis_config nuis_persistency)

install(TARGETS T2KCC1PiDataComparisons DESTINATION plugins)

install(FILES T2K.CC1Pi.sample.config.fcl DESTINATION fcl)

SET(INuADataComparisons ${INuADataComparisons} PARENT_SCOPE)
