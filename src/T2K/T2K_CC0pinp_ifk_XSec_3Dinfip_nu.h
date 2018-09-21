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
#ifndef T2K_CC0PINP_IFK_XSEC_3DINFIP_NU_H_SEEN
#define T2K_CC0PINP_IFK_XSEC_3DINFIP_NU_H_SEEN

#include "Measurement1D.h"

class T2K_CC0pinp_ifk_XSec_3Dinfip_nu : public Measurement1D {
public:

  /// Basic Constructor.
  T2K_CC0pinp_ifk_XSec_3Dinfip_nu(nuiskey samplekey);

  /// Virtual Destructor
  ~T2K_CC0pinp_ifk_XSec_3Dinfip_nu() {};

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

  /// Have to do a weird event scaling for analysis 1
  void ConvertEventRates();

 private:

  bool only_allowed_particles;
  bool numu_event;
  double numu_energy;
  int particle_pdg;
  double pmu, CosThetaMu;
  int fAnalysis;

  double outOfBoundsMC;

  bool fIsSystCov, fIsStatCov, fIsNormCov;

  TFile* fInputFile;

  std::vector<TH1D*> fMCHist_Slices;
  std::vector<TH1D*> fDataHist_Slices;

  void FillMCSlice(double x, double y, double z, double w);
  
};
  
#endif
