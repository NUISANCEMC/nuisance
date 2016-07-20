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

  TTree* eventVariables;

  // Saved Variables
  double Enu_true;
  double Enu_QE;
  int PDGnu;

  double Q2_true;
  double Q2_QE;
  
  int PDGLep;
  double TLep;
  double CosLep;
  double ELep;
  double PLep;
  double MLep;

  double PPr;  //!< Highest Mom Proton
  double CosPr; //!< Highest Mom Proton
  double EPr;
  double TPr;
  double MPr;

  double FluxWeight; //!< For Flux Shape Unfolding
  
};

#endif
