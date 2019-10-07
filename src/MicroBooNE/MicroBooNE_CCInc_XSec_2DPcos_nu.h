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
#ifndef MICROBOONE_CCINC_2DPCOS_NU_H_SEEN
#define MICROBOONE_CCINC_2DPCOS_NU_H_SEEN

#include "Measurement1D.h"
#include "MeasurementVariableBox2D.h"
#include "TH2Poly.h"

class MicroBooNE_CCInc_XSec_2DPcos_nu : public Measurement1D {
public:
  /// Basic Constructor.
  MicroBooNE_CCInc_XSec_2DPcos_nu(nuiskey samplekey);

  /// Virtual Destructor
  ~MicroBooNE_CCInc_XSec_2DPcos_nu(){};

  /// Numu CCinc Signal Definition
  bool isSignal(FitEvent *nvect);

  /// Read histograms in a special way because format is different.
  void SetHistograms();

  /// Bin Pmu CosThetaMu
  void FillEventVariables(FitEvent *customEvent);

  /// Smear and build 1D MC histogram from slices
  void ConvertEventRates();

  // Fill Histograms
  void FillHistograms();

private:
  TFile *fInputFile;
  TH2D *fMCHist_Fine2D;

  TMatrixDSym *fSmearingMatrix;

  std::map<int, std::pair<size_t, size_t> > fPolyBinMap;

  std::vector<TH1D *> fMCHist_Slices;
  std::vector<TH1D *> fDataHist_Slices;

  void FillMCSlice(double x, double y, double w);

};

#endif
