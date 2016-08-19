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

if(NOT DEFINED SET_UP_PYTHIA6)

  if(DEFINED NEED_PYTHIA6 AND NEED_PYTHIA6)
    if(NOT DEFINED ENV{PYTHIA6} AND NOT DEFINED ENV{PYTHIA6_LIB})

      cmessage(FATAL_ERROR "Environment variable PYTHIA6/PYTHIA6_LIB is not defined. "
        "This must be set to point to a prebuilt NuWro instance.")

    endif()

    if(DEFINED ENV{PYTHIA6})
      set(PYTHIA6 $ENV{PYTHIA6})
    elseif(DEFINED ENV{PYTHIA6_LIB})
      set(PYTHIA6 $ENV{PYTHIA6_LIB})
    endif()
    set(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS} -L${PYTHIA6} -lPythia6 -lgfortran")

  else()
    set(NEED_PYTHIA6 0)
  endif()

endif()
set(SET_UP_PYTHIA6 TRUE)
