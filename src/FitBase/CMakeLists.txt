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

set(FitBase_Impl_Files
  ParamPull.cxx
  EventManager.cxx
  Measurement1D.cxx
  Measurement2D.cxx
  JointMeas1D.cxx
  MeasurementBase.cxx
  TemplateMeas1D.cxx
  SampleSettings.cxx
  MeasurementVariableBox.cxx
  MeasurementVariableBox2D.cxx
  MeasurementVariableBox1D.cxx
  StandardStacks.cxx
  StackBase.cxx
)

add_library(FitBase SHARED ${FitBase_Impl_Files})
target_link_libraries(FitBase CoreIncludes ROOT::ROOT)

install(TARGETS FitBase DESTINATION lib)
