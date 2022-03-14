find_package(nusystematics REQUIRED)

LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES
  ${nusystematics_INCLUDE_DIR}
  ${systematicstools_INCLUDE_DIR})

LIST(APPEND EXTRA_LINK_DIRS ${nusystematics_CMAKE_DIR}/../lib ${systematicstools_CMAKE_DIR}/../lib)
LIST(APPEND EXTRA_LIBS nusystematics::all)