set(utility_experimental_implementation_files
  T2KUtility.cxx
  MINERvAUtility.cxx
  VirginiaQEArchiveUtility.cxx
  )

set(utility_experimental_header_files
  T2KUtility.hxx
  MINERvAUtility.hxx
  VirginiaQEArchiveUtility.hxx
  )

add_library(nuis_utility_experimental SHARED ${utility_experimental_implementation_files})
target_link_libraries(nuis_utility_experimental)

install(TARGETS nuis_utility_experimental DESTINATION lib)

install(FILES ${utility_experimental_header_files} DESTINATION include/utility/experimental)
