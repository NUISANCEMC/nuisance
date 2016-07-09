// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NuFiX.
*
*    NuFiX is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NuFiX is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

// contains signal definitions for various interactions
#ifndef SIGNALDEF_H_SEEN
#define SIGNALDEF_H_SEEN

// C/C++ includes
#include <math.h>

// ROOT includes
#include <TLorentzVector.h>

// ExtFit includes
#include "FitUtils.h"

// NEUT includes
#include "FitEvent.h"
#include "FitUtils.h"


// make it a namespace
namespace SignalDef {

  // These are all the interaction modes for neutrinos
  bool isCCQE(FitEvent *event, double EnuMin, double EnuMax, bool isRestricted = false);
  bool isCCQEBar(FitEvent *event, double EnuMin, double EnuMax, bool isRestricted = false);
  bool isCCQELike(FitEvent *event, double EnuMin, double EnuMax);

  bool isCCQEBar_res(FitEvent *event, double EnuMin, double EnuMax);
  bool isCCQELikeBar(FitEvent *event, double EnuMin, double EnuMax);
  
  // MiniBooNE CC1pi+ differs from MINERvA CC1pi+ differs from T2K CC1pi+!
  bool isCC1pip_MiniBooNE(FitEvent *event, double EnuMin, double EnuMax);

  // MINERvA has unfolded and not unfolded muon phase space
  bool isCC1pip_MINERvA (FitEvent *event, double EnuMin, double EnuMax, bool isRestricted = false);
  bool isCCNpip_MINERvA (FitEvent *event, int &nPions, double EnuMin, double EnuMax, bool isRestricted = false);

  // T2K not unfolded phase space restrictions
  bool isCC1pip_T2K(FitEvent *event, double EnuMin, double EnuMax);

  bool isCC1pi0_MiniBooNE   (FitEvent *event, double EnuMin, double EnuMax);
  bool isCC1pi0Bar_MINERvA  (FitEvent *event, double EnuMin, double EnuMax);

  bool isNC1pi0_MiniBooNE   (FitEvent *event, double EnuMin, double EnuMax);
  bool isNC1pi0Bar_MiniBooNE(FitEvent *event, double EnuMin, double EnuMax);

  bool isCCcoh_MINERvA    (FitEvent *event, double EnuMin, double EnuMax);
  bool isCCcohBar_MINERvA (FitEvent *event, double EnuMin, double EnuMax);


  bool isCCQEnumu_MINERvA(FitEvent* event, double EnuMin, double EnuMax, bool fullphasespace=true);
  bool isCCQEnumubar_MINERvA(FitEvent* event, double EnuMin, double EnuMax, bool fullphasespace=true);

  bool isCCincLowRecoil_MINERvA(FitEvent *event, double EnuMin, double EnuMax, bool hadroncut);
  
};

#endif
