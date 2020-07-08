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
#ifndef T2K_CC0PI_2DPCOS_H2O_ANU_H_SEEN
#define T2K_CC0PI_2DPCOS_H2O_ANU_H_SEEN

#include "Measurement1D.h"
#include "TH2Poly.h"
#include "MeasurementVariableBox2D.h"


class T2K_CC0pi_XSec_H2O_2DPcos_anu : public Measurement1D {
public:

  // Basic Constructor.
  T2K_CC0pi_XSec_H2O_2DPcos_anu(nuiskey samplekey);

  // Virtual Destructor
  ~T2K_CC0pi_XSec_H2O_2DPcos_anu() {};

  // Signal Definition
  bool isSignal(FitEvent *nvect);

  // Read histograms 
  void SetHistograms();

  // Bin Pmu CosThetaMu
  void FillEventVariables(FitEvent* customEvent);

  // Fill Histograms
  void FillHistograms();

  /// Event scaling 
  void ConvertEventRates();

 private:

  TFile* fInputFile;
  std::vector<TH1D*> fMCHist_Slices;
  std::vector<TH1D*> fDataHist_Slices;
  double pmu, CosThetaMu;

  void FillMCSlice(double x, double y, double w);
  
};
  
#endif
