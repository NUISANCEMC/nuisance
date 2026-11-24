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

#ifndef MINERvA_NukeCCinc_XSec_1DPTMU_ANTINU_H_SEEN
#define MINERvA_NukeCCinc_XSec_1DPTMU_ANTINU_H_SEEN

#include "Measurement1D.h"

/*
  Author: Anežka Klustová @anezkaklust (a.klustova20@imperial.ac.uk or klustova.a@gmail.com)
  MINERvA antineutrino CC inclusive on nuclear targets (RHC flux)
  Signal: numubar CC inclusive, 1 mu+, 2 < Emu < 20 GeV, thmu < 17 deg
  arXiv:2604.07091
  PRL DOI: https://doi.org/10.1103/n7s9-8btb
  Zenodo DOI: https://doi.org/10.5281/zenodo.24306935
*/
class MINERvA_NukeCCinc_XSec_1Dptmu_antinu : public Measurement1D {

public:

  enum target {
    kCH,
    kC,
    kFe,
    kPb,
    kCH_C_Flux,   // tracker CH daisy-reweighted to the C target flux
    kCH_Fe_Flux,  // tracker CH daisy-reweighted to the Fe target flux
    kCH_Pb_Flux,  // tracker CH daisy-reweighted to the Pb target flux
  };

  enum distribution {
    kPtmu,
  };

  MINERvA_NukeCCinc_XSec_1Dptmu_antinu(nuiskey samplekey);
  virtual ~MINERvA_NukeCCinc_XSec_1Dptmu_antinu() {};

  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);

  target fTarget;
  distribution fDistribution;

  void SetupDataSettings();

};

#endif
