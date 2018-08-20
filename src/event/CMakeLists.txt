SET(event_implementation_files
  FullEvent.cxx
  MinimalEvent.cxx
  Particle.cxx)

SET(event_header_files
  FullEvent.hxx
  MinimalEvent.hxx
  Particle.hxx
  types.hxx)

add_library(nuis_event SHARED ${event_implementation_files})

install(TARGETS nuis_event DESTINATION lib)
install(FILES ${event_header_files} DESTINATION include/event)
