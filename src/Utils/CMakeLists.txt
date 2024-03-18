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

set(Utils_Impl_Files
  FitUtils.cxx
  GeneralUtils.cxx
  PlotUtils.cxx
  SignalDef.cxx
  BeamUtils.cxx
  TargetUtils.cxx
  ParserUtils.cxx
)

set(Utils_Hdr_Files
  FitUtils.h
  GeneralUtils.h
  PlotUtils.h
  SignalDef.h
  BeamUtils.h
  TargetUtils.h
  ParserUtils.h
  PhysConst.h
)

add_library(Utils SHARED ${Utils_Impl_Files})
target_link_libraries(Utils CoreIncludes ROOT::ROOT)
set_target_properties(Utils PROPERTIES PUBLIC_HEADER "${Utils_Hdr_Files}")

install(TARGETS Utils
    EXPORT nuisance-targets
    LIBRARY DESTINATION lib/
    PUBLIC_HEADER DESTINATION include)

add_library(NUIS::Utils ALIAS Utils)