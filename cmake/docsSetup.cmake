# Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

# add a target to generate API documentation with Doxygen
find_package(Doxygen)

if(DOXYGEN_FOUND)

  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc)

  configure_file(${CMAKE_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile @ONLY)
  add_custom_target(doc_generate
  ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/doc/Doxyfile
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/doc
  COMMENT "Generating documentation with Doxygen... (this will take a while)" VERBATIM
  )

  add_custom_target(docs
  make
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/doc/latex
  COMMENT "Building latex documentation with Doxygen... (this will also take a while)" VERBATIM
  )
  add_dependencies(docs doc_generate)
  install(FILES ${CMAKE_BINARY_DIR}/doc/latex/refman.pdf
    DESTINATION ${CMAKE_BINARY_DIR}/doc
    RENAME ExtFit_${NUISANCE_VERSION_STRING}.pdf OPTIONAL)
endif(DOXYGEN_FOUND)
