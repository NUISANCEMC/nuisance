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

#ifndef HadronFlux_Vectors_H_SEEN
#define HadronFlux_Vectors_H_SEEN
#include "Measurement1D.h"
#include "FitEvent.h"

class HadronFlux_Vectors : public Measurement1D {

public:

  HadronFlux_Vectors(std::string name, std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile);
  virtual ~HadronFlux_Vectors() {};

  //! Grab info from event
  void FillEventVariables(FitEvent *event);

  void ResetVariables();

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

  TTree* eventVariables;
  std::vector<FitParticle*> partList;
  std::vector<FitParticle*> initList;
  std::vector<FitParticle*> vertList;

  bool SavePreFSI;

  int Mode;
  int in_pdg;
  int tgt;
  int tgta;
  int tgtz;

  float in_etrue;
  float in_ektrue;
  float in_ptrue;

  // Save outgoing particle vectors
  int nfsp;
  static const int kMAX = 200;
  float px[kMAX];
  float py[kMAX];
  float pz[kMAX];
  float E[kMAX];
  int pdg[kMAX];
  int pdg_rank[kMAX];

  // Save incoming particle info
  int ninitp;
  float px_init[kMAX];
  float py_init[kMAX];
  float pz_init[kMAX];
  float E_init[kMAX];
  int pdg_init[kMAX];

  // Save pre-FSI particle info
  int nvertp;
  float px_vert[kMAX];
  float py_vert[kMAX];
  float pz_vert[kMAX];
  float E_vert[kMAX];
  int pdg_vert[kMAX];

  // Basic event info
  float Weight;
  float InputWeight;
  float RWWeight;
  double fScaleFactor;

  // Custom weights
  float CustomWeight;

};

#endif
