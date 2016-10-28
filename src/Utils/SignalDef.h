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

// contains signal definitions for various interactions
#ifndef SIGNALDEF_H_SEEN
#define SIGNALDEF_H_SEEN

// C/C++ includes
#include <math.h>

// ROOT includes
#include <TLorentzVector.h>

// ExtFit includes
#include "FitEvent.h"

// make it a namespace
namespace SignalDef {

  // NEW!
  bool isCCINC(FitEvent* event, int nuPDG, double EnuMin=0, double EnuMax=0);
  bool isNCINC(FitEvent *event, int nuPDG, double EnuMin=0, double EnuMax=0);
  bool isCC0pi(FitEvent *event, int nuPDG, double EnuMin=0, double EnuMax=0);
  bool isCCQELike(FitEvent *event, int nuPDG, double EnuMin=0, double EnuMax=0);
  bool isCCQE(FitEvent *event, int nuPDG, double EnuMin=0, double EnuMax=0);
  bool isCCCOH(FitEvent *event, int nuPDG, int piPDG, double EnuMin=0, double EnuMax=0);

  bool isCC1pi(FitEvent *event, int nuPDG, int piPDG, double EnuMin=0, double EnuMax=0);
  bool isNC1pi(FitEvent *event, int nuPDG, int piPDG, double EnuMin=0, double EnuMax=0);

  // MINERvA has unfolded and not unfolded muon phase space
  bool isCC1pip_MINERvA (FitEvent *event, double EnuMin, double EnuMax, bool isRestricted = false);
  bool isCCNpip_MINERvA (FitEvent *event, int &nPions, double EnuMin, double EnuMax, bool isRestricted = false);

  // T2K not unfolded phase space restrictions
  bool isCC1pip_T2K_H2O(FitEvent *event, double EnuMin, double EnuMax);
  bool isCC1pip_T2K_CH(FitEvent *event, double EnuMin, double EnuMax, bool Michel);

  bool isCCQEnumu_MINERvA(FitEvent* event, double EnuMin, double EnuMax, bool fullphasespace=true);
  bool isCCQEnumubar_MINERvA(FitEvent* event, double EnuMin, double EnuMax, bool fullphasespace=true);

  bool isCCincLowRecoil_MINERvA(FitEvent *event, double EnuMin, double EnuMax);

  bool isT2K_CC0pi(FitEvent* event, double EnuMin, double EnuMax, bool forwardgoing);
  bool isT2K_CC0pi_STV(FitEvent* event, double EnuMin, double EnuMax);

  // Generic Signal Functions
  bool isCC0pi1p_MINERvA(FitEvent* event, double enumin, double enumax);

  bool HasProtonKEAboveThreshold(FitEvent* event, double threshold);
  
  bool IsRestrictedAngle(FitEvent* event, int nuPDG, int otherPDG, double angle);

  bool IsEnuInRange(FitEvent* event, double emin, double emax);


}

#endif
