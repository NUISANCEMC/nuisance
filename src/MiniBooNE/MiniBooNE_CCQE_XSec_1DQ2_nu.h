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

#ifndef MINIBOONE_CCQE_XSEC_1DQ2_NU_H_SEEN
#define MINIBOONE_CCQE_XSEC_1DQ2_NU_H_SEEN

#include "Measurement1D.h"

//********************************************************************
class MiniBooNE_CCQE_XSec_1DQ2_nu : public Measurement1D {
//********************************************************************

public:

  MiniBooNE_CCQE_XSec_1DQ2_nu(nuiskey samplekey);
  virtual ~MiniBooNE_CCQE_XSec_1DQ2_nu() {};

  // Multiple Constrictuors.
  void Setup_MiniBooNE_CCQE_XSec_1DQ2_nu();
  void Setup_MiniBooNE_CCQELike_XSec_1DQ2_nu();

  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);
  void FillExtraHistograms(MeasurementVariableBox* vars, double weight = 1.0);
  
private:
  double q2qe; ///<! X_Variable
  bool bad_particle; ///<! Used in CCQELike mode to tag events without nucleons, muons or photons.
  bool ccqelike; ///<! Flag for running in CCQELike mode
  TH1D* fDataHist_CCQELIKE; ///<! CCQELike data contribution
  TrueModeStack* fMCHist_CCQELIKE;
};

#endif
