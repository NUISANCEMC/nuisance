// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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
#ifndef T2K_CC0PIWITHPROTONS_XSEC_2018_MULTIDIF_0P_1P_NP_H_SEEN
#define T2K_CC0PIWITHPROTONS_XSEC_2018_MULTIDIF_0P_1P_NP_H_SEEN

#include "Measurement1D.h"

class T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p_Np : public Measurement1D {
public:

  /// Basic Constructor.
  T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p_Np(nuiskey samplekey);

  /// Virtual Destructor
  ~T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p_Np() {};

  /// Numu CC0PI Signal Definition
  ///
  /// /item
  bool isSignal(FitEvent *nvect);

  /// Read histograms in a special way because format is different.
  /// Read from FitPar::GetDataBase()+"/T2K/CC0pi/T2K_CC0PI_2DPmuCosmu_Data.root"
  void SetHistograms();

  /// Bin Tmu CosThetaMu
  void FillEventVariables(FitEvent* customEvent);

  // Fill Histograms
  void FillHistograms();

 private:

  bool useCC0pi0p;
  bool useCC0pi1p;
  bool useCC0piNp;

  bool only_allowed_particles;
  bool numu_event;
  double numu_energy;
  int particle_pdg;
  int fAnalysis;
  double fPP, fCosThetaP, fPMu, fCosThetaMu, fNp;

  bool fIsSystCov, fIsStatCov, fIsNormCov;

  TFile* fInputFile;
  TH2D* fMCHist_Fine2D;

  TH1D *fMCHist_CC0pi0pCosTheta;
  TH1D *fDataHist_CC0pi0pCosTheta;
  TH1D *fMCHist_CC0pi1pCosTheta;
  TH1D *fDataHist_CC0pi1pCosTheta;
  std::vector<TH1D*> fMCHist_Slices;
  std::vector<TH1D*> fDataHist_Slices;

  void FillMCSlice(int nProtonsAboveThresh, double pmu, double CosThetaMu, double pp, double CosThetaP, double w);
  int Get1DBin(int nProtonsAboveThresh, double pmu, double CosThetaMu, double pp, double CosThetaP);
  int GetCosThetaMuSlice(int nProtonsAboveThresh, double CosThetaMu);
  int GetCC0pi1p2DSlice(int nProtonsAboveThresh, double CosThetaMu, double CosThetaP);
  // void Write(std::string drawOpt);

};

#endif
