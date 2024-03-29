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

#ifndef Smearceptance_Tester_H_SEEN
#define Smearceptance_Tester_H_SEEN

#include "Measurement1D.h"

#include "ISmearcepter.h"

#ifdef Prob3plusplus_ENABLED
#include "OscWeightEngine.h"
#endif

//********************************************************************
class Smearceptance_Tester : public Measurement1D {
  //********************************************************************

 public:
  Smearceptance_Tester(nuiskey samplekey);
  virtual ~Smearceptance_Tester(){};

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

  void AddEventVariablesToTree();

 private:
  ISmearcepter *smearceptor;

  TTree *eventVariables;

  float Omega_true;
  float Q2_true;
  int Mode_true;

  float EISLep_true;

  TLorentzVector HMFS_clep_true;
  TLorentzVector HMFS_pip_true;
  TLorentzVector HMFS_pim_true;
  TLorentzVector HMFS_cpi_true;
  TLorentzVector HMFS_pi0_true;
  TLorentzVector HMFS_cK_true;
  TLorentzVector HMFS_K0_true;
  TLorentzVector HMFS_p_true;

  float KEFSHad_cpip_true;
  float KEFSHad_cpim_true;
  float KEFSHad_cpi_true;
  float TEFSHad_pi0_true;
  float KEFSHad_cK_true;
  float KEFSHad_K0_true;
  float KEFSHad_p_true;
  float KEFSHad_n_true;

  float EFSHad_true;
  float EFSChargedEMHad_true;

  float EFSLep_true;
  float EFSgamma_true;

  int PDGISLep_true;
  int PDGFSLep_true;

  int Nprotons_true;
  int Nneutrons_true;
  int Ncpiplus_true;
  int Ncpiminus_true;
  int Ncpi_true;
  int Npi0_true;
  int NcK_true;
  int NK0_true;

  TLorentzVector HMFS_clep_rec;
  TLorentzVector HMFS_pip_rec;
  TLorentzVector HMFS_pim_rec;
  TLorentzVector HMFS_cpi_rec;
  TLorentzVector HMFS_pi0_rec;
  TLorentzVector HMFS_cK_rec;
  TLorentzVector HMFS_K0_rec;
  TLorentzVector HMFS_p_rec;

  float KEFSHad_cpip_rec;
  float KEFSHad_cpim_rec;
  float KEFSHad_cpi_rec;
  float TEFSHad_pi0_rec;
  float KEFSHad_cK_rec;
  float KEFSHad_K0_rec;
  float KEFSHad_p_rec;
  float KEFSHad_n_rec;

  float EFSHad_rec;
  float EFSLep_rec;

  float EFSVis_cpip;
  float EFSVis_cpim;
  float EFSVis_cpi;
  float EFSVis_pi0;
  float EFSVis_cK;
  float EFSVis_K0;
  float EFSVis_p;
  float EFSVis_n;
  float EFSVis_gamma;
  float EFSVis_other;
  float EFSVis;

  int FSCLep_seen;
  int Nprotons_seen;
  int Nneutrons_seen;
  int Ncpip_seen;
  int Ncpim_seen;
  int Ncpi_seen;
  int Npi0_seen;
  int NcK_seen;
  int NK0_seen;
  int Nothers_seen;

  float EISLep_QE_rec;
  float EISLep_LepHad_rec;
  float EISLep_LepHadVis_rec;

  int Nprotons_contributed;
  int Nneutrons_contributed;
  int Ncpip_contributed;
  int Ncpim_contributed;
  int Ncpi_contributed;
  int Npi0_contributed;
  int NcK_contributed;
  int NK0_contributed;
  int Ngamma_contributed;
  int Nothers_contibuted;

  float Weight;
  float RWWeight;
  float InputWeight;
  float FluxWeight;
  float EffWeight;
  float PredEvtRateWeight;

  float xsecScaling;

  bool flagCCINC_true;
  bool flagCC0K_true;
  bool flagCC0Pi_true;
  bool flagCC1Pi_true;

  bool flagCCINC_rec;
  bool flagCC0K_rec;
  bool flagCC0Pi_rec;
  bool flagCC1Pi_rec;

  bool OutputSummaryTree;

  int SVDTruncation;

  TH2D *RecoSmear;
  TH1D *ETrueDistrib;
  TH1D *ETrueDistrib_noweight;
  TH1D *ERecDistrib;

};

#endif
