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

set(Reweight_Impl_Files
  GlobalDialList.cxx
  FitWeight.cxx
  WeightEngineBase.cxx
  WeightUtils.cxx
  SampleNormEngine.cxx
  LikelihoodWeightEngine.cxx
  SplineWeightEngine.cxx
  NUISANCESyst.cxx
  NUISANCEWeightEngine.cxx
  NUISANCEWeightCalcs.cxx
)

set(Reweight_Hdr_Files
  GlobalDialList.h
  FitWeight.h
  WeightEngineBase.h
  WeightUtils.h
  SampleNormEngine.h
  LikelihoodWeightEngine.h
  SplineWeightEngine.h
  NUISANCESyst.h
  NUISANCEWeightEngine.h
  NUISANCEWeightCalcs.h
  weightRPA.h
)

if(GENIEReWeight_ENABLED)
  LIST(APPEND Reweight_Impl_Files 
    MINERvAWeightCalcs.cxx)
  LIST(APPEND Reweight_Hdr_Files 
    MINERvAWeightCalcs.h
    GENIEWeightEngine.h)
  if(GENIE3_API_ENABLED)
    LIST(APPEND Reweight_Impl_Files 
      GENIE3WeightEngine.cxx)
  else()
    LIST(APPEND Reweight_Impl_Files 
      GENIE2WeightEngine.cxx)
  endif()
endif()

if(NEUTReWeight_ENABLED)
  if(NEUTReWeight_LEGACY_API_ENABLED)
    LIST(APPEND Reweight_Impl_Files 
      NEUTWeightEngine_legacy.cxx)
  else()
    LIST(APPEND Reweight_Impl_Files 
      NEUTWeightEngine.cxx)
  endif()
  LIST(APPEND Reweight_Hdr_Files 
    NEUTWeightEngine.h)
endif()

if(T2KReWeight_ENABLED)
  if(T2KReWeight_LEGACY_API_ENABLED)
    LIST(APPEND Reweight_Impl_Files 
      T2KWeightEngine_legacy.cxx)
  else()
    LIST(APPEND Reweight_Impl_Files 
      T2KWeightEngine.cxx)
  endif()
  LIST(APPEND Reweight_Hdr_Files 
    T2KWeightEngine.h)
endif()

if(NIWGLegacy_ENABLED)
  LIST(APPEND Reweight_Impl_Files NIWGWeightEngine_legacy.cxx)
  LIST(APPEND Reweight_Hdr_Files NIWGWeightEngine_legacy.h)
endif()

if(NOvARwgt_ENABLED)
  LIST(APPEND Reweight_Impl_Files NOvARwgtEngine.cxx)
  LIST(APPEND Reweight_Hdr_Files NOvARwgtEngine.h)
endif()

if(nusystematics_ENABLED)
  LIST(APPEND Reweight_Impl_Files nusystematicsWeightEngine.cxx)
  LIST(APPEND Reweight_Hdr_Files nusystematicsWeightEngine.h)
endif()

if(Prob3plusplus_ENABLED)
  LIST(APPEND Reweight_Impl_Files OscWeightEngine.cxx)
  LIST(APPEND Reweight_Hdr_Files OscWeightEngine.h)
endif()

add_library(Reweight SHARED ${Reweight_Impl_Files})
target_link_libraries(Reweight GeneratorCompileDependencies CoreIncludes ROOT::ROOT)
set_target_properties(Reweight PROPERTIES PUBLIC_HEADER "${Reweight_Hdr_Files}")

install(TARGETS Reweight
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include)
