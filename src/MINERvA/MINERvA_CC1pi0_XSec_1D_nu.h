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

#ifndef MINERVA_CC1PI0_XSEC_1D_NU_H_SEEN
#define MINERVA_CC1PI0_XSEC_1D_NU_H_SEEN

#include "Measurement1D.h"

// Implementation of 2017 MINERvA numu CC1pi0
// arxiv:1708.03723v1 hep-ex
// c.wret14@imperial.ac.uk

class MINERvA_CC1pi0_XSec_1D_nu : public Measurement1D {
public:
  MINERvA_CC1pi0_XSec_1D_nu(nuiskey samplekey);
  virtual ~MINERvA_CC1pi0_XSec_1D_nu() {};

  void SetupDataSettings();
  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);
  
  private:
  // The enums for the different distributions
  // P.S. the order __IS__ important: after Wexp we require a proton and impose other cuts
  enum DataDistribution {
    // Pion kinetic energy
    kTpi,
    // Pion-neutrino angle
    kth,
    // Muon momentum
    kpmu,
    // Muon-neutrino angle
    kthmu,
    // True Q2
    kQ2,
    // True Enu
    kEnu,
    // And the interesting distributions ^_^
    // Wexperimental
    kWexp,
    // p, pi0 invariant mass with Wexp < 1.8
    kPPi0Mass,
    // p, pi0 invariant mass with Wexp < 1.4
    kPPi0MassDelta,
    // Cos theta Adler angle
    kCosAdler,
    // Phi Adler angle
    kPhiAdler
  } CC1pi0_DataDistributions;

  DataDistribution fDist;

  double WexpCut;
  double ProtonCut;
};

#endif
