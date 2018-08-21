set(samples_header_files
  ISample.hxx
  IDataComparison.hxx
  SimpleDataComparison.hxx)

install(FILES ${samples_header_files} DESTINATION include/samples)

add_subdirectory(MCTools)

add_subdirectory(nuA)

cmessage(DEBUG "INuADataComparisons: ${INuADataComparisons}")

SET(INuADataComparisons_List)
if(NOT IDataComparisons STREQUAL "")
  string(REPLACE ";" "," INuADataComparisons_List ${INuADataComparisons})
endif()

SET(INuADataComparisons_List ${INuADataComparisons_List} PARENT_SCOPE)
SET(INuADataComparisons_FHiCL ${INuADataComparisons_FHiCL} PARENT_SCOPE)
