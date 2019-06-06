add_subdirectory(CC0Pi)

LIST(APPEND INuADataComparisons_FHiCL MINERvA.sample.config.fcl)
install(FILES MINERvA.sample.config.fcl DESTINATION fcl)

SET(INuADataComparisons ${INuADataComparisons} PARENT_SCOPE)
SET(INuADataComparisons_FHiCL ${INuADataComparisons_FHiCL} PARENT_SCOPE)
