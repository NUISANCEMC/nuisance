set(samples_header_files
  IEventProcessor.hxx
  IDataComparison.hxx
  SimpleDataComparison.hxx
  MultiDataComparison.hxx
  SimpleMCStudy.hxx)

install(FILES ${samples_header_files} DESTINATION include/samples)

add_subdirectory(MCTools)

# add_subdirectory(nuA)
# add_subdirectory(eA)

cmessage(DEBUG "INuADataComparisons: ${INuADataComparisons}")

SET(INuADataComparisons_List)
if(NOT INuADataComparisons STREQUAL "")
  string(REPLACE ";" ", " INuADataComparisons_List "${INuADataComparisons}")
endif()
cmessage(DEBUG "INuADataComparisons_List: ${INuADataComparisons_List}")


SET(INuADataComparisons_List ${INuADataComparisons_List} PARENT_SCOPE)
SET(INuADataComparisons_FHiCL ${INuADataComparisons_FHiCL} PARENT_SCOPE)

cmessage(DEBUG "IeADataComparisons: ${IeADataComparisons}")

SET(IeADataComparisons_List)
if(NOT IeADataComparisons STREQUAL "")
  string(REPLACE ";" ", " IeADataComparisons_List "${IeADataComparisons}")
endif()
cmessage(DEBUG "IeADataComparisons_List: ${IeADataComparisons_List}")


SET(IeADataComparisons_List ${IeADataComparisons_List} PARENT_SCOPE)
SET(IeADataComparisons_FHiCL ${IeADataComparisons_FHiCL} PARENT_SCOPE)
