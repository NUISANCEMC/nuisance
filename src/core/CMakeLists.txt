set(core_implementation_files
FullEvent.cxx
InputManager.cxx
MinimalEvent.cxx
Particle.cxx)

set(core_header_files
FullEvent.hxx
InputManager.hxx
MinimalEvent.hxx
Particle.hxx
IInputHandler.hxx
types.hxx)

add_library(nuis_core SHARED ${core_implementation_files})
target_link_libraries(nuis_core)

install(TARGETS nuis_core DESTINATION lib)
install(FILES ${core_header_files} DESTINATION include/core)
