configure_file(nuis.global.config.fcl.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/nuis.global.config.fcl" @ONLY)

install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/nuis.global.config.fcl" DESTINATION
${CMAKE_INSTALL_PREFIX}/fcl)

SET(INuADataComparisons_FHiCL_Includes)
foreach(fcl ${INuADataComparisons_FHiCL})
  SET(INuADataComparisons_FHiCL_Includes "${INuADataComparisons_FHiCL_Includes}#include \"${fcl}\"\n")
endforeach()

configure_file(nuis.datacomparisons.fcl.in
"${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/nuis.datacomparisons.fcl" @ONLY)

install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/nuis.datacomparisons.fcl" DESTINATION
${CMAKE_INSTALL_PREFIX}/fcl)
