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

#ifndef MINERVA_1DQ2_nu_proton_H_SEEN
#define MINERVA_1DQ2_nu_proton_H_SEEN
#include "Measurement1D.h"

///\brief MINERvA CC0pi1p Analysis : Q2 Distribution
///
///\n Ref: Phys. Rev. D 91, 071301 (2015) [arXiv:1409.4497]
///\n Input: CH events generated with all interaction modes with flux given in Ref.
class MINERvA_CC0pi_XSec_1DQ2_nu_proton : public Measurement1D {
public:

  ///\brief Setup data histograms, full covariance matrix and coplanar histogram.
  ///\n Available fit options: FIX/FULL,DIAG
  ///\n Sample is given as /nucleon.
  ///
  ///\n Valid Sample Names:
  ///\n 1. MINERvA_CC0pi_XSec_1DQ2_nu_proton - Main analysis
  MINERvA_CC0pi_XSec_1DQ2_nu_proton(nuiskey samplekey);
  virtual ~MINERvA_CC0pi_XSec_1DQ2_nu_proton() {};

  ///\brief Signal is CC0pi1p
  ///
  ///\n 1. CC0pi Selection
  ///\n 2. Proton above 110 MeV
  ///\n 3. Muon Angle < 20deg
  ///\n 4. 1.5 < Enu < 100.0
  bool isSignal(FitEvent* event);

  ///\brief Determine Q2 from the leading proton
  void FillEventVariables(FitEvent *event);

  ///\brief Calculate Extra Weight (Not necessary)
  inline double CalcSampleWeight(FitEvent* event){ return 1.0; }

 private:

  TH1D* fCoplanarMCHist;   /// MC Coplanar Distribution
  TH1D* fCoplanarDataHist; /// Data Coplanar Distribution

};

#endif
