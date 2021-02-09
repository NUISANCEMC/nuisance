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

if(NEED_PYTHIA6)

  if(DEFINED ENV{PYTHIA6_LIBRARY} AND PYTHIA6 STREQUAL "")
    SET(PYTHIA6 $ENV{PYTHIA6_LIBRARY} CACHE PATH 
      "Path to directory containing libPythia6.so. Overrides environment variable \$PYTHIA6 <>" 
      FORCE)
  endif()

  if(PYTHIA6 STREQUAL "")
    cmessage(FATAL_ERROR "Variable PYTHIA6 is not defined. This must be set to point to a prebuilt PYTHIA6 instance, please set the \$PYTHIA6 environment variable or configure with -DPYTHIA6=/path/to/pythia6.")
  endif()

  LIST(APPEND EXTRA_LINK_DIRS ${PYTHIA6})

  LIST(APPEND EXTRA_LIBS Pythia6 gfortran)

endif()
