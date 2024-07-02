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

set(MCStudies_Impl_Files
  ExpMultDist_CCQE_XSec_1DVar_FakeStudy.cxx
  ExpMultDist_CCQE_XSec_2DVar_FakeStudy.cxx
  GenericFlux_Tester.cxx
  GenericFlux_Vectors.cxx
  HadronFlux_Vectors.cxx
  #MCStudy_KaonPreSelection.cxx
  MCStudy_MuonValidation.cxx
  ElectronFlux_FlatTree.cxx
  T2K2017_FakeData.cxx
  MCStudy_CCQEHistograms.cxx
  OfficialNIWGPlots.cxx
  Smearceptance_Tester.cxx
  
  SigmaEnuHists.cxx
)

if(Prob3plusplus_ENABLED)
  LIST(APPEND MCStudies_Impl_Files 
    Simple_Osc.cxx
    Smear_SVDUnfold_Propagation_Osc.cxx)
endif()

add_library(MCStudies SHARED ${MCStudies_Impl_Files})
target_link_libraries(MCStudies Experiments CoreIncludes ROOT::ROOT)

install(TARGETS MCStudies
    EXPORT nuisance-targets
    LIBRARY DESTINATION lib/
    PUBLIC_HEADER DESTINATION include)

add_library(NUIS::MCStudies ALIAS MCStudies)