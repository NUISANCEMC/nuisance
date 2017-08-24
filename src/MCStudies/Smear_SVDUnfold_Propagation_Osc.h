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

#ifndef Smear_SVDUnfold_Propagation_Osc_H_SEEN
#define Smear_SVDUnfold_Propagation_Osc_H_SEEN

#include "Measurement1D.h"

class Smear_SVDUnfold_Propagation_Osc : public Measurement1D {
 public:
  Smear_SVDUnfold_Propagation_Osc(nuiskey samplekey);

  virtual ~Smear_SVDUnfold_Propagation_Osc(){};

  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);

  void ConvertEventRates(void);

  TH1D *NDDataHist;
  TH1D *FDDataHist;

  TH2D *NDToSpectrumSmearingMatrix;

  TMatrixD NDToSpectrumResponseMatrix;
  TMatrixD SpectrumToFDResponseMatrix;

  Int_t TruncateUpTo;
};

#endif
