// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NUISANCE.
*
*    NUISANCE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NUISANCE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NUISANCE.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#ifndef HEPDATA_Loader_H_SEEN
#define HEPDATA_Loader_H_SEEN

#include "Measurement1D.h"
#include "nuiscling_ftypes.h"

class HEPDATA_Loader : public Measurement1D {
public:

  HEPDATA_Loader(nuiskey samplekey);
  virtual ~HEPDATA_Loader() {}; 
  
  /// \brief Fill Q2QE Event Information
  void FillEventVariables(FitEvent *event);       

  /// \brief Selection only true CCQE
  bool isSignal(FitEvent *event);    

  /// \brief Fill main histograms and correction histograms             
  void FillHistograms();

  /// \brief scale normal MC and corrected MC
  void ScaleEvents();

  void Write(std::string drawOpt);
  
  private:

  int fHEPDATA_NDIM;
  nuiscling_ftypes::filter filter_func;
  std::vector<nuiscling_ftypes::project> projection_funcs;

  TH2Poly* fDataHist_TH2Poly;
  TH2Poly* fMCHist_TH2Poly;


};
  
#endif
