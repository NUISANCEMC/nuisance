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

  //! Fill all signal flags we currently have
  void FillSignalFlags(FitEvent *event);

  void AddEventVariablesToTree();
  void AddSignalFlagsToTree();
  
 private:

  TTree* eventVariables;

  // Saved Variables
  double Enu_true;
  double Enu_QE;
  int PDGnu;

  double Q2_true;
  double Q2_QE;

  int Nprotons;
  int Nneutrons;
  int Npiplus;
  int Npineg;
  int Npi0;
  
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

  
  double PNe;
  double CosNe;
  double ENe;
  double TNe;
  double MNe;

  double PPiP;
  double CosPiP;
  double EPiP;
  double TPiP;
  double MPiP;

  double PPiN;
  double CosPiN;
  double EPiN;
  double TPiN;
  double MPiN;

  double PPi0;
  double CosPi0;
  double EPi0;
  double TPi0;
  double MPi0;
  
  double q0_true;
  double q3_true;

  int Nparticles;
  int Nleptons;

  double Erecoil_true;
  double Erecoil_charged;
  double Erecoil_minerva;

  double Weight;
  double RWWeight;
  double InputWeight;
  double FluxWeight;
  
  bool flagCCQE_full;
  bool flagCCQE_rest;
  bool flagCCQEBar_full;
  bool flagCCQEBar_rest;
  bool flagCC1pip_MiniBooNE;
  bool flagCC1pip_MINERvA_full;
  bool flagCC1pip_MINERvA_rest;
  bool flagCCNpip_MINERvA_full;
  bool flagCCNpip_MINERva_rest;
  bool flagCC1pip_T2K;
  bool flagCC1pi0_MiniBooNE;
  bool flagCC1pi0Bar_MINERvA;
  bool flagNC1pi0_MiniBooNE;
  bool flagNC1pi0Bar_MiniBooNE;
  bool flagCCcoh_MINERvA;
  bool flagCCcohBar_MINERvA;

  bool flagCCQEnumu_MINERvA_full;
  bool flagCCQEnumubar_MINERvA_full;
  bool flagCCQEnumu_MINERvA_rest;
  bool flagCCQEnumubar_MINERvA_rest;

  bool flagCCincLowRecoil_MINERvA;
  bool flagCCincLowRecoil_MINERvA_reqhad;
  bool flagCCQELike_MiniBooNE;
  bool flagCCQE_MiniBooNE;
  bool flagCCQEBar_MiniBooNE;

};

#endif
