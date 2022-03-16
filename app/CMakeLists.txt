# Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

################################################################################
#    This file is part of NUISANCE.
#
#    NUISANCE is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    NUISANCE is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with NUISANCE.  If not, see <http://www.gnu.org/licenses/>.
################################################################################

if(MINIMIZER_ENABLED)
  add_executable(nuismin nuismin.cxx)
  target_link_libraries(nuismin CoreTargets GeneratorLinkDependencies)

#   add_executable(nuissplines nuissplines.cxx)
#   set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};nuissplines)
endif()


add_executable(nuis_flat_tree_combiner nuis_flat_tree_combiner.cxx)
target_link_libraries(nuis_flat_tree_combiner CoreTargets GeneratorLinkDependencies)

add_executable(nuiscomp nuiscomp.cxx)
target_link_libraries(nuiscomp CoreTargets GeneratorLinkDependencies)

add_executable(nuisflat nuisflat.cxx)
target_link_libraries(nuisflat CoreTargets GeneratorLinkDependencies)

# add_executable(nuissmear nuissmear.cxx)
# set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};nuissmear)

# add_executable(nuissyst nuissyst.cxx)
# set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};nuissyst)

# add_executable(nuisbayes nuisbayes.cxx)
# set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};nuisbayes)

if(GENIE_ENABLED)
  add_executable(PrepareGENIE PrepareGENIE.cxx)
	target_link_libraries(PrepareGENIE CoreTargets GeneratorLinkDependencies)
endif()

if(NEUT_ENABLED)
  add_executable(PrepareNEUT PrepareNEUT.cxx)
	target_link_libraries(PrepareNEUT CoreTargets GeneratorLinkDependencies)
endif()

# if(USE_GiBUU)
#   add_executable(PrepareGiBUU PrepareGiBUU.cxx)
#   set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};PrepareGiBUU)
# endif()

# # PREPARE NUWRO
# # Commented out for the time being until it is finished..
# if(USE_NuWro)
#   add_executable(PrepareNuwro PrepareNuwroEvents.cxx)
#   set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};PrepareNuwro)
# endif()

# add_executable(nuisbac nuisbac.cxx)
# set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};nuisbac)

# foreach(targ ${TARGETS_TO_BUILD})
#   if(NOT "${CMAKE_LINK_FLAGS} ${NUIS_EXE_FLAGS}" STREQUAL " ")
#     set_target_properties(${targ} PROPERTIES LINK_FLAGS "${CMAKE_LINK_FLAGS} ${NUIS_EXE_FLAGS}")
#   endif()
#   target_link_libraries(${targ} ${MODULETargets})
#   target_link_libraries(${targ} ${CMAKE_DEPENDLIB_FLAGS})
# endforeach()

# install(TARGETS ${TARGETS_TO_BUILD} DESTINATION bin)