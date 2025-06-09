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

#ifndef MINERvA_NukeCC1pip_XSec_1D_NU_H_SEEN
#define MINERvA_NukeCC1pip_XSec_1D_NU_H_SEEN

#include "Measurement1D.h"

/*
 A. Bercellie et al. "Simultaneous measurement of muon neutrino νμ charged-current single π+ production in CH, C, H2O, Fe, and Pb targets in MINERvA", Phys.Rev.Lett. 131 (2023) 1, 011801
 arXiv:2209.07852
 Data release page: https://minerva.fnal.gov/nuclear-target-charged-current-pi-production-data-release-page/
*/
class MINERvA_NukeCC1pip_XSec_1D_nu : public Measurement1D {

public:

  enum target {
    kCH,
    kC,
    kH2O,
    kFe,
    kPb,
  };

  enum distribution {
    kPmu,
    kThmu,
    kPlmu,
    kPtmu,
    kQ2,
    kWexp,
    kTpi,
    kThpi,
  };

  MINERvA_NukeCC1pip_XSec_1D_nu(nuiskey samplekey);
  virtual ~MINERvA_NukeCC1pip_XSec_1D_nu() {};

  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);

  target fTarget;
  distribution fDistribution;

  void SetupDataSettings();

};


#endif
