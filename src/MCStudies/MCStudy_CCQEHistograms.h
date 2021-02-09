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

#ifndef MCStudy_CCQEHistograms_H_SEEN
#define MCStudy_CCQEHistograms_H_SEEN
#include "Measurement1D.h"

//********************************************************************
class MCStudy_CCQEHistograms : public Measurement1D {
//********************************************************************

public:

  MCStudy_CCQEHistograms(std::string name, std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile);
  virtual ~MCStudy_CCQEHistograms() {};

  //! Grab info from event
  void FillEventVariables(FitEvent *event);

  void ScaleEvents();
  void ResetAll();

  //! Define this samples signal
  bool isSignal(FitEvent *nvect);

  //! Write Files
  void Write(std::string drawOpt);

 private:
  
  double fEventScaleFactor;
  TTree* fEventTree;
  TH1D* hist_Enu;
  float Enu;
  TH1D* hist_TLep;
  float TLep  ;
  TH1D* hist_CosLep;
  float CosLep;
  TH1D* hist_Q2;
  float Q2    ;
  TH1D* hist_Q2QE;
  float Q2QE  ;
  TH1D* hist_EQE;
  float EQE   ;
  TH1D* hist_q0;
  float q0    ;
  TH1D* hist_q3;
  float q3    ;
  TH2D* hist_q0q3;

  TH1D* hist_Total;
  
  TH2D* hist_TLepCosLep;

  double LocalRWWeight;
  double LocalInputWeight;

};

#endif
