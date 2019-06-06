add_subdirectory(CC0Pi)
add_subdirectory(CC1Pi)

LIST(APPEND INuADataComparisons_FHiCL T2K.sample.config.fcl)
install(FILES T2K.sample.config.fcl DESTINATION fcl)

SET(INuADataComparisons ${INuADataComparisons} PARENT_SCOPE)
SET(INuADataComparisons_FHiCL ${INuADataComparisons_FHiCL} PARENT_SCOPE)
