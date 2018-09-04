set(utility_implementation_files
  FileSystemUtility.cxx
  FullEventUtility.cxx
  KinematicUtility.cxx
  PDGCodeUtility.cxx
  StringUtility.cxx)

set(utility_header_files
  FileSystemUtility.hxx
  FullEventUtility.hxx
  InteractionChannelUtility.hxx
  KinematicUtility.hxx
  PDGCodeUtility.hxx
  ROOTUtility.hxx
  HistogramUtility.hxx
  StringUtility.hxx
  TerminalUtility.hxx
  StatsUtility.hxx)

add_library(nuis_utility SHARED ${utility_implementation_files})
target_link_libraries(nuis_utility)

install(TARGETS nuis_utility DESTINATION lib)

install(FILES ${utility_header_files} DESTINATION include/utility)
