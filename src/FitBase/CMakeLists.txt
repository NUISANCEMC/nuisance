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

set(FitBase_Hdr_Files
  ParamPull.h
  EventManager.h
  Measurement1D.h
  Measurement2D.h
  JointMeas1D.h
  MeasurementBase.h
  TemplateMeas1D.h
  SampleSettings.h
  MeasurementVariableBox.h
  MeasurementVariableBox2D.h
  MeasurementVariableBox1D.h
  StandardStacks.h
  StackBase.h
)

if(ROOT_VERSION VERSION_GREATER 6)
  LIST(APPEND FitBase_Impl_Files nuiscling.cxx)
  LIST(APPEND FitBase_Hdr_Files nuiscling.h)
endif()

add_library(FitBase SHARED ${FitBase_Impl_Files})
target_link_libraries(FitBase CoreIncludes ROOT::ROOT)
set_target_properties(FitBase PROPERTIES PUBLIC_HEADER "${FitBase_Hdr_Files}")

if(ROOT_VERSION VERSION_GREATER 6)
  if(GENIE_ENABLED)
    target_compile_definitions(FitBase PRIVATE GENIE_INC_DIR=\"${GENIE_INC_DIR}\")
  endif()

  if(NEUT_ENABLED)
    target_compile_definitions(FitBase PRIVATE NEUT_INC_DIR=\"${NEUT_INCLUDE_DIR}\")
  endif()

  if(NuWro_ENABLED)
    target_compile_definitions(FitBase PRIVATE NuWro_INC_DIR=\"${NuWro_INC_DIR}\")
  endif()
endif()

install(TARGETS FitBase
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include)
