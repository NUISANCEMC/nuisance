add_subdirectory(input)
add_subdirectory(utility)
add_subdirectory(variation)

SET(input_implementation_files
  GeneratorManager.cxx)

SET(input_header_files
  GeneratorManager.hxx)

add_library(nuis_generator SHARED ${input_implementation_files})
install(TARGETS nuis_generator DESTINATION lib)
install(FILES ${input_header_files} DESTINATION include/generator)

LIST(APPEND GENERATOR_DEPENDENT_TARGETS nuis_generator)

SET(GENERATOR_DEPENDENT_TARGETS ${GENERATOR_DEPENDENT_TARGETS} PARENT_SCOPE)
