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

#ifndef MINERVA_CC1PIP_XSEC_1DTH_NU_H_SEEN
#define MINERVA_CC1PIP_XSEC_1DTH_NU_H_SEEN

#include "Measurement1D.h"
/*
  From “Charged Pion Production in nu_mu Interactions on Hydrocarbon at ⟨E_nu⟩= 4.0 GeV” Phys. Rev. D 92, 092008 (2015)
  Superceded by "MINERvA_CC1pip_XSec_1D_2017Update.h". See that header file for more information
*/


class MINERvA_CC1pip_XSec_1Dth_nu : public Measurement1D {
public:
  MINERvA_CC1pip_XSec_1Dth_nu(nuiskey samplekey);
  virtual ~MINERvA_CC1pip_XSec_1Dth_nu() {};

  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);
  bool fFullPhaseSpace;
  bool fFluxCorrection;
  
  private:
};

#endif
