SET(variation_implementation_files
  IWeightProvider.cxx
  WeightManager.cxx)

SET(variation_header_files
  WeightManager.hxx
  IVariationProvider.hxx
  IWeightProvider.hxx)

add_library(nuis_variation SHARED ${variation_implementation_files})
target_link_libraries(nuis_variation nuis_event nuis_plugins)

install(TARGETS nuis_variation DESTINATION lib)
install(FILES ${variation_header_files} DESTINATION include/variation)
