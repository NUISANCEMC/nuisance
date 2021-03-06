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

#ifndef MINERVA_CCQE_XSec_1DQ2_antinu_H_SEEN
#define MINERVA_CCQE_XSec_1DQ2_antinu_H_SEEN

#include "Measurement1D.h"
#include "MINERvAUtils.h"
//********************************************************************
class MINERvA_CCQE_XSec_1DQ2_antinu : public Measurement1D {
//********************************************************************
public:

  MINERvA_CCQE_XSec_1DQ2_antinu(nuiskey samplekey);
  virtual ~MINERvA_CCQE_XSec_1DQ2_antinu() {};

  // Functions for handling each neut event
  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);

 private:

  bool isFluxFix, fullphasespace;

  bool fSaveExtra;
  TH1D* fExtra_Eav;
  MINERvAUtils::ModeStack* fExtra_Eav_MODES;
  TH2D* fExtra_EavQ2;
  MINERvAUtils::ModeStack* fExtra_EavQ2_MODES;
  TF1* fEavQ2Cut;


};

#endif
