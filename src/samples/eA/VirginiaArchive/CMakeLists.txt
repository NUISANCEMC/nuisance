SET(SAMPLES
  VirginiaQEArchive)

foreach(S ${SAMPLES})
  LIST(APPEND IeADataComparisons ${S})
  LIST(APPEND SAMPLES_src ${S}.cxx)
endforeach()

LIST(APPEND IeADataComparisons_FHiCL VirginiaQEArchive.sample.config.fcl)
add_library(VirginiaQEArchiveDataComparisons SHARED ${SAMPLES_src})
target_link_libraries(VirginiaQEArchiveDataComparisons
  nuis_event nuis_input nuis_config nuis_persistency)

add_executable(DumpSampleFHiCL DumpSampleFHiCL.cxx)
target_link_libraries(DumpSampleFHiCL -Wl,--no-as-needed nuis_event nuis_input nuis_generator_utility nuis_utility_experimental nuis_utility nuis_config nuis_persistency nuis_plugins nuis_params nuis_variation)
target_link_libraries(DumpSampleFHiCL ${GENERATOR_DEPENDENT_TARGETS} -Wl,--as-needed)
target_link_libraries(DumpSampleFHiCL ${NUISANCE_LINK_DIRS})
target_link_libraries(DumpSampleFHiCL ${NUISANCE_DEPEND_LIBS})
if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
  set_target_properties(DumpSampleFHiCL PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
endif()

install(TARGETS VirginiaQEArchiveDataComparisons DESTINATION plugins)

install(FILES VirginiaQEArchive.sample.config.fcl DESTINATION fcl)

SET(IeADataComparisons ${IeADataComparisons} PARENT_SCOPE)
SET(IeADataComparisons_FHiCL ${IeADataComparisons_FHiCL} PARENT_SCOPE)
