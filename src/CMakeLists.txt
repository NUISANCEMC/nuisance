add_subdirectory(config)
add_subdirectory(event)
add_subdirectory(input)
add_subdirectory(plugins)
add_subdirectory(utility)
add_subdirectory(generator)
add_subdirectory(persistency)

add_subdirectory(samples)
SET(INuADataComparisons_List ${INuADataComparisons_List} PARENT_SCOPE)
SET(INuADataComparisons_FHiCL ${INuADataComparisons_FHiCL} PARENT_SCOPE)

add_subdirectory(app)
