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

#ifndef GenericFlux_Tester_H_SEEN
#define GenericFlux_Tester_H_SEEN
#include "Measurement1D.h"

#ifndef __BAD__FLOAT__
#define __BAD_FLOAT__ -999.99
#endif

//********************************************************************
class GenericFlux_Tester : public Measurement1D {
//********************************************************************

public:

  GenericFlux_Tester(std::string name, std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile);
  virtual ~GenericFlux_Tester() {};

  //! Clear private variables
  inline void ResetVariables();

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

  // Lighter flat trees that don't include vectors
  bool liteMode;

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

  // Auxillairies
  float Q2_true;
  float Q2_QE;
  float W_nuc_rest;
  float bjorken_x;
  float bjorken_y;
  float q0_true;
  float q3_true;
  float Emiss;
  float Emiss_preFSI;
  TVector3 pmiss;
  TVector3 pmiss_preFSI;
  float Erecoil_true;
  float Erecoil_charged;
  float Erecoil_minerva;

  // Interaction mode
  int Mode;

  // Res ID (for GENIE only)
  int ResCode;

  // Particle counters
  int Nparticles;
  int Nleptons;
  int Nother;
  int Nprotons;
  int Nneutrons;
  int Npiplus;
  int Npineg;
  int Npi0;

  // Lepton variables
  int PDGLep;
  float TLep;
  float CosLep;
  float ELep;
  float PLep;
  float MLep;

  // Proton variables
  float PPr;  //!< Highest Mom Proton
  float CosPr; //!< Highest Mom Proton
  float EPr;
  float TPr;
  float MPr;

  // Neutron variables
  float PNe;
  float CosNe;
  float ENe;
  float TNe;
  float MNe;

  // Pi+ variables
  float PPiP;
  float CosPiP;
  float EPiP;
  float TPiP;
  float MPiP;

  // Pi- variables
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

  // Angular variables
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

  // Weights
  float Weight;
  float RWWeight;
  float InputWeight;
  float FluxWeight;

  // Generic signal flags
  bool flagCCINC;
  bool flagNCINC;
  bool flagCCQE;
  bool flagCC0pi;
  bool flagCCQELike;
  bool flagNCEL;
  bool flagNC0pi;
  bool flagCCcoh;
  bool flagNCcoh;
  bool flagCC1pip;
  bool flagNC1pip;
  bool flagCC1pim;
  bool flagNC1pim;
  bool flagCC1pi0;
  bool flagNC1pi0;

};

#endif
