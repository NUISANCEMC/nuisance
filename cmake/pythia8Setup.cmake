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

if(NEED_PYTHIA8)
  if(PYTHIA8 STREQUAL "")
    cmessage(FATAL_ERROR "Variable PYTHIA8 is not defined. This must be set to point to a prebuilt PYTHIA8 instance, please set the \$PYTHIA8 environment variable or configure with -DPYTHIA8=/path/to/pythia8.")
  endif()

  LIST(APPEND EXTRA_LINK_DIRS ${PYTHIA8})

  LIST(REVERSE EXTRA_LIBS)
  LIST(APPEND EXTRA_LIBS Pythia8 gfortran)
  LIST(REVERSE EXTRA_LIBS)
endif()
