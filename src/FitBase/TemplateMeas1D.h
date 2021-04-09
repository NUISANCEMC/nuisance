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

#ifndef TEMPLATEMEAS1D_H_SEEN
#define TEMPLATEMEAS1D_H_SEEN

#include "Measurement1D.h"

enum TemplateMeas1D_AnalysisTypes{
  kTemplateMeas1D_CC0pi_Tmu = 0,
  kTemplateMeas1D_CCQE_Q2 = 1
};

//******************************************************************** 
class TemplateMeas1D : public Measurement1D {
//******************************************************************** 

public:

  /// REQUIRED by all ssample classes
  TemplateMeas1D(std::string name, std::string inputfile, FitWeight *rw, std::string type);

  /// Destructor
  virtual ~TemplateMeas1D() {};

  /// REQUIRED by all sample classes
  void FillEventVariables(FitEvent *event);

  /// REQUIRED by all sample classes
  bool isSignal(FitEvent *event);
  
  /// Optional
  void FillHistograms();
  void Write(std::string drawOpt);
  void ScaleEvents();
  void ApplyNormScale(double norm);
  void ResetAll();


 private:

  /// OPTIONAL
  TH1D* fMCHist_ExtraQ2Plot_PDG[61];
  TH1D* fMCHist_ExtraQ2Plot;
  
  double q2qe; ///<! X_Variable
  bool bad_particle; ///<! Used in CCQELike mode to tag events without nucleons, muons or photons.
  bool ccqelike; ///<! Flag for running in CCQELike mode
  TH1D* fDataHist_CCQELIKE; ///<! CCQELike data contribution
  int fAnalysis;
};
  
#endif
