# Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

################################################################################
#    This file is part of NuFiX.
#
#    NuFiX is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    NuFiX is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
################################################################################

set(TARGETS_TO_BUILD)

if(USE_MINIMIZER)
  add_executable(ExtFit_minimizer ExtFit_minimizer.cxx)

  set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};ExtFit_minimizer)

  target_link_libraries(ExtFit_minimizer ${MODULETargets})

  target_link_libraries(ExtFit_minimizer ${CMAKE_DEPENDLIB_FLAGS})
  target_link_libraries(ExtFit_minimizer ${ROOT_LIBS})

  if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
    set_target_properties(ExtFit_minimizer PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
  endif()
endif()



include_directories(${RWENGINE_INCLUDE_DIRECTORIES})

include_directories(${CMAKE_SOURCE_DIR}/src/Minimizer)
include_directories(${CMAKE_SOURCE_DIR}/src/FitBase)
include_directories(${CMAKE_SOURCE_DIR}/src/Utils)
include_directories(${CMAKE_SOURCE_DIR}/src/FCN)
include_directories(${CMAKE_SOURCE_DIR}/src/MCStudies)
include_directories(${EXP_INCLUDE_DIRECTORIES})

add_executable(DumpGiBUUEvents DumpGiBUUEvents.cxx)
set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};DumpGiBUUEvents)
target_link_libraries(DumpGiBUUEvents ${MODULETargets})
target_link_libraries(DumpGiBUUEvents ${CMAKE_DEPENDLIB_FLAGS})
target_link_libraries(DumpGiBUUEvents ${ROOT_LIBS})
include_directories(${CMAKE_SOURCE_DIR}/src/FitBase)
if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
  set_target_properties(DumpGiBUUEvents PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
endif()


add_executable(ExtFit_comparison ExtFit_comparison.cxx)
set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};ExtFit_comparison)
target_link_libraries(ExtFit_comparison ${MODULETargets})
target_link_libraries(ExtFit_comparison ${CMAKE_DEPENDLIB_FLAGS})
target_link_libraries(ExtFit_comparison ${ROOT_LIBS})
if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
  set_target_properties(ExtFit_comparison PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
endif()

if(USE_NuWro)
  add_executable(ExtFit_PrepareNuwro ExtFit_PrepareNuwro.cxx)
  set(TARGETS_TO_BUILD ${TARGETS_TO_BUILD};ExtFit_PrepareNuwro)
  target_link_libraries(ExtFit_PrepareNuwro ${MODULETargets})
  target_link_libraries(ExtFit_PrepareNuwro ${CMAKE_DEPENDLIB_FLAGS})
  target_link_libraries(ExtFit_PrepareNuwro ${ROOT_LIBS})
  if(NOT "${CMAKE_LINK_FLAGS}" STREQUAL "")
    set_target_properties(ExtFit_PrepareNuwro PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
  endif()
endif()

install(TARGETS ${TARGETS_TO_BUILD} DESTINATION bin)
