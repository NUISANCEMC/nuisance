add_subdirectory(CCInclusive)

LIST(APPEND INuADataComparisons_FHiCL MicroBooNE.sample.config.fcl)
install(FILES MicroBooNE.sample.config.fcl DESTINATION fcl)

SET(INuADataComparisons ${INuADataComparisons} PARENT_SCOPE)
SET(INuADataComparisons_FHiCL ${INuADataComparisons_FHiCL} PARENT_SCOPE)
