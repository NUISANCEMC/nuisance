// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NuFiX.
*
*    NuFiX is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NuFiX is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#ifndef GenericFlux_Tester_H_SEEN
#define GenericFlux_Tester_H_SEEN
#include "Measurement1D.h"

//******************************************************************** 
class GenericFlux_Tester : public Measurement1D {
//******************************************************************** 

public:

  GenericFlux_Tester(std::string name, std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile);
  virtual ~GenericFlux_Tester() {};
  
  //! Grab info from event 
  void FillEventVariables(FitEvent *event);

  //! Fill Custom Histograms
  void FillHistograms();
  
  //! ResetAll
  void ResetAll();

  //! Scale
  void ScaleEvents();

  //! Norm
  void ApplyNormScale(double norm);

  //! Define this samples signal 
  bool isSignal(FitEvent *nvect);

  //! Write Files
  void Write(std::string drawOpt);

  //! Get Chi2
  double GetChi2();

 private:

  // All Histograms
  TH1D* mcHist_1DQ2;
  TH1D* mcHist_1DTmu;
  TH1D* mcHist_1DCosTheta;
  TH1D* mcHist_1DEnu;
  TH1D* mcHist_1DErec;
  TH1D* mcHist_1DEdiff;

  TH2D* mcHist_2DTmuCosTheta;
  
  double Q2, Tmu, CosTheta;
  double Enu, Erec, Ediff;

  double binding_E;

  bool isCCQE, isCC0PI, isCC1PI, isCCNPI, isCCOTHER;
  bool isMEC, isCCQEMEC, isRES, isRES1PI, isRESNPI;
  int filltype;
  bool antinu;

   enum GenericFillTypes{
     kCCQEFill=0,
     kCCQEMECFill,
     kMECFill,
     kRESFill,
     kCC0PIFill
   };

};

#endif
