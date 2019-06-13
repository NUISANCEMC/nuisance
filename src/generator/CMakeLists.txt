add_subdirectory(input)
add_subdirectory(utility)

if(DEFINED USE_GENERATOR_REWEIGHT AND USE_GENERATOR_REWEIGHT)
  add_subdirectory(variation)
endif()

SET(input_implementation_files
  GeneratorManager.cxx)

SET(input_header_files
  GeneratorManager.hxx)

add_library(nuis_generator SHARED ${input_implementation_files})
install(TARGETS nuis_generator DESTINATION lib)
install(FILES ${input_header_files} DESTINATION include/generator)

LIST(APPEND GENERATOR_DEPENDENT_TARGETS nuis_generator)

SET(GENERATOR_DEPENDENT_TARGETS ${GENERATOR_DEPENDENT_TARGETS} PARENT_SCOPE)
