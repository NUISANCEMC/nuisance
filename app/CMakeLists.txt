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
  LIST(APPEND TARGETS_TO_BUILD 
    nuismin)
endif()

LIST(APPEND TARGETS_TO_BUILD 
  nuis_flat_tree_combiner
  nuiscomp
  nuisflat
  nuissmear
  nuissyst
  nuisbayes
  nuisbac
  nuisplot
  PrepareGiBUU)

if(GENIE_ENABLED)
  LIST(APPEND TARGETS_TO_BUILD PrepareGENIE)
endif()

if(NEUT_ENABLED)
    LIST(APPEND TARGETS_TO_BUILD PrepareNEUT)
endif()

if(NuWro_ENABLED)
    LIST(APPEND TARGETS_TO_BUILD PrepareNuWroEvents)
endif()

foreach(targ ${TARGETS_TO_BUILD})
  add_executable(${targ} ${targ}.cxx)
  target_link_libraries(${targ} CoreTargets GeneratorLinkDependencies)
endforeach()

install(TARGETS ${TARGETS_TO_BUILD} DESTINATION bin)

add_executable(nuishistrange nuishistrange.cxx)
target_link_libraries(nuishistrange ROOT::ROOT)
install(TARGETS nuishistrange DESTINATION bin)

add_executable(nuishistdewidth nuishistdewidth.cxx)
target_link_libraries(nuishistdewidth ROOT::ROOT)
install(TARGETS nuishistdewidth DESTINATION bin)
