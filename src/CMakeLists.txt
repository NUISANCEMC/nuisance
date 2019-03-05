add_subdirectory(config)
add_subdirectory(event)
add_subdirectory(exception)
add_subdirectory(generator)
add_subdirectory(input)
add_subdirectory(parameters)
add_subdirectory(persistency)
add_subdirectory(plugins)
add_subdirectory(samples)
add_subdirectory(utility)
add_subdirectory(variation)

SET(INuADataComparisons_List ${INuADataComparisons_List} PARENT_SCOPE)
SET(INuADataComparisons_FHiCL ${INuADataComparisons_FHiCL} PARENT_SCOPE)

SET(GENERATOR_DEPENDENT_TARGETS ${GENERATOR_DEPENDENT_TARGETS} PARENT_SCOPE)

add_subdirectory(app)
