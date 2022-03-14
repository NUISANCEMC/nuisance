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
  MINERvAWeightCalcs.cxx
  weightRPA.h
)

if(GENIE_ENABLED)
  LIST(APPEND Reweight_Impl_Files GENIEWeightEngine.cxx)
endif()

if(NEUT_ENABLED)
  LIST(APPEND Reweight_Impl_Files NEUTWeightEngine.cxx)
endif()

if(NuWro_ENABLED)
  LIST(APPEND Reweight_Impl_Files NuWroWeightEngine.cxx)
endif()

if(T2KReWeight_ENABLED)
  LIST(APPEND Reweight_Impl_Files T2KWeightEngine.cxx)
endif()

if(NIWG_ENABLED)
  LIST(APPEND Reweight_Impl_Files NIWGWeightEngine.cxx)
endif()

if(NOvARwgt_ENABLED)
  LIST(APPEND Reweight_Impl_Files NOvARwgtEngine.cxx)
endif()

if(NuSystematics_ENABLED)
  LIST(APPEND Reweight_Impl_Files nusystematicsWeightEngine.cxx)
endif()

if(Prob3plusplus_ENABLED)
  LIST(APPEND Reweight_Impl_Files OscWeightEngine.cxx)
endif()

add_library(Reweight SHARED ${Reweight_Impl_Files})

install(TARGETS Reweight DESTINATION lib)
