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



if(NOT DEFINED ENV{HEPMC})

    cmessage(FATAL_ERROR "Environment variable NUWRO is not defined. "
      "This must be set to point to a prebuilt NuWro instance.")

endif()

set(HEPMC $ENV{HEPMC})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__HEPMC_ENABLED__  ")

LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES  ${HEPMC}/include/HepMC/ ${HEPMC}/include/ ${HEPMC}/include/SimpleVector.h  )

set(RWENGINE_LINKER_FLAGS "${RWENGINE_LINKER_FLAGS} -L${HEPMC}/lib/ -lHepMC -lHepMCfio")

