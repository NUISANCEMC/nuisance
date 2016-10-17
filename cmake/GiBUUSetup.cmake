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

if(DEFINED BUILD_GiBUU AND BUILD_GiBUU)
  include(ExternalProject)

  ExternalProject_Add(GiBUUTools
  PREFIX "${PROJECT_BINARY_DIR}/GiBUUTools"
  GIT_REPOSITORY https://github.com/luketpickering/GiBUU-t2k-dev.git
  CMAKE_ARGS
  -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
  -DUSE_GIBUU=1)

  cmessage(STATUS "Building GiBUU and GiBUUTools")
  set(BUILD_GiBUU 1)
else()
  set(BUILD_GiBUU 0)
endif()
