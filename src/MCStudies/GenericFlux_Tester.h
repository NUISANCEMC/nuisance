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
  void ApplyNormScale(float norm);

  //! Define this samples signal
  bool isSignal(FitEvent *nvect);

  //! Write Files
  void Write(std::string drawOpt);

  //! Get Chi2
  float GetChi2();

  //! Fill all signal flags we currently have
  void FillSignalFlags(FitEvent *event);

  void AddEventVariablesToTree();
  void AddSignalFlagsToTree();

 private:

  TTree* eventVariables;


  TLorentzVector *nu_4mom;
  TLorentzVector *pmu;
  TLorentzVector *ppip;
  TLorentzVector *ppim;
  TLorentzVector *ppi0;
  TLorentzVector *pprot;
  TLorentzVector *pneut;

  // Saved Variables
  float Enu_true;
  float Enu_QE;
  int PDGnu;

  float Q2_true;
  float Q2_QE;
  float W_nuc_rest;
  float bjorken_x;
  float bjorken_y;

  int Mode;

  int Nprotons;
  int Nneutrons;
  int Npiplus;
  int Npineg;
  int Npi0;

  int PDGLep;
  float TLep;
  float CosLep;
  float ELep;
  float PLep;
  float MLep;

  float PPr;  //!< Highest Mom Proton
  float CosPr; //!< Highest Mom Proton
  float EPr;
  float TPr;
  float MPr;


  float PNe;
  float CosNe;
  float ENe;
  float TNe;
  float MNe;

  float PPiP;
  float CosPiP;
  float EPiP;
  float TPiP;
  float MPiP;

  float PPiN;
  float CosPiN;
  float EPiN;
  float TPiN;
  float MPiN;

  float PPi0;
  float CosPi0;
  float EPi0;
  float TPi0;
  float MPi0;

  float CosPmuPpip;
  float CosPmuPpim;
  float CosPmuPpi0;
  float CosPmuPprot;
  float CosPmuPneut;
  float CosPpipPprot;
  float CosPpipPneut;
  float CosPpipPpim;
  float CosPpipPpi0;
  float CosPpimPprot;
  float CosPpimPneut;
  float CosPpimPpi0;
  float CosPi0Pprot;
  float CosPi0Pneut;
  float CosPprotPneut;

  float q0_true;
  float q3_true;

  int Nparticles;
  int Nleptons;

  float Erecoil_true;
  float Erecoil_charged;
  float Erecoil_minerva;

  float Weight;
  float RWWeight;
  float InputWeight;
  float FluxWeight;

  bool flagCCQE_full;
  bool flagCCQE_rest;
  bool flagCCQEBar_full;
  bool flagCCQEBar_rest;

  bool flagCC1pip_MiniBooNE;
  bool flagCC1pip_MINERvA_full;
  bool flagCC1pip_MINERvA_rest;
  bool flagCCNpip_MINERvA_full;
  bool flagCCNpip_MINERvA_rest;
  bool flagCC1pip_T2K_Michel;
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
