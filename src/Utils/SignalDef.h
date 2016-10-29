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

namespace SignalDef {

  bool isCCINC(FitEvent* event, int nuPDG, double EnuMin=0, double EnuMax=0);
  bool isNCINC(FitEvent *event, int nuPDG, double EnuMin=0, double EnuMax=0);
  bool isCC0pi(FitEvent *event, int nuPDG, double EnuMin=0, double EnuMax=0);
  bool isCCQELike(FitEvent *event, int nuPDG, double EnuMin=0, double EnuMax=0);
  bool isCCQE(FitEvent *event, int nuPDG, double EnuMin=0, double EnuMax=0);
  bool isCCCOH(FitEvent *event, int nuPDG, int piPDG, double EnuMin=0, double EnuMax=0);

  bool isCC1pi(FitEvent *event, int nuPDG, int piPDG, double EnuMin=0, double EnuMax=0);
  bool isNC1pi(FitEvent *event, int nuPDG, int piPDG, double EnuMin=0, double EnuMax=0);

  bool isCC1pi3Prong(FitEvent *event, int nuPDG, int piPDG,
		     int thirdPDG, double EnuMin=0, double EnuMax=0);
  bool isNC1pi3Prong(FitEvent *event, int nuPDG, int piPDG,
                     int thirdPDG, double EnuMin=0, double EnuMax=0);
  bool isCCWithFS(FitEvent *event, int nuPDG, std::vector<int> pdgs,
		  double EnuMin=0, double EnuMax=0);

  // Generic Signal Functions
  bool HasProtonKEAboveThreshold(FitEvent* event, double threshold);
  bool IsRestrictedAngle(FitEvent* event, int nuPDG, int otherPDG, double angle);
  bool IsEnuInRange(FitEvent* event, double emin, double emax);


}

#endif
