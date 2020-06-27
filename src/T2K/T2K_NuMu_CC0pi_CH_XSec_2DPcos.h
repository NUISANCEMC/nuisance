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
#ifndef T2K_NUMU_CC0PI_CH_XSEC_2DPCOS_H_SEEN
#define T2K_NUMU_CC0PI_CH_XSEC_2DPCOS_H_SEEN

#include "Measurement1D.h"
#include "TH2Poly.h"
#include "MeasurementVariableBox2D.h"

class T2K_NuMu_CC0pi_CH_XSec_2DPcos : public Measurement1D {
public:

  T2K_NuMu_CC0pi_CH_XSec_2DPcos(nuiskey samplekey);

  /// Virtual Destructor
  ~T2K_NuMu_CC0pi_CH_XSec_2DPcos() {};

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

  void ConvertEventRates();

  void Write(std::string drawopt);

  /// \brief Create Q2 Box to save correction info
  inline MeasurementVariableBox* CreateBox(){ return new MeasurementVariableBox2D(); };

 private:

  bool fIsSystCov, fIsStatCov, fIsNormCov;
  bool fMaskMomOverflow;

  TH2D* fMCHist_Fine2D;

  std::vector<TH1D*> fMCHist_Slices;
  std::vector<TH1D*> fDataHist_Slices;

  void FillMCSlice(double x, double y, double w);
  void MaskMomOverflow();

};

#endif
