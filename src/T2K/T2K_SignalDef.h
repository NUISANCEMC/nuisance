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

#ifndef T2K_SIGNALDEF_H_SEEN
#define T2K_SIGNALDEF_H_SEEN

#include "SignalDef.h"

namespace SignalDef {
bool isCC1pip_T2K_PRD97_012001(FitEvent *event, double EnuMin, double EnuMax);

enum arxiv1909_03936_PScuts {
  kMuonFwd = (1 << 0), // cos(th_mu) > 0
  kMuonHighEff = (1 << 1),  // cos(th_mu) > 0.2, pmu > 200
  kPionFwd = (1 << 2), // cos(th_pi) > 0
  kPionVFwd = (1 << 3), // cos(th_pi) > 0.2
  kPionHighMom = (1 << 4) // ppi > 200
};

bool isCC1pip_T2K_arxiv1909_03936(FitEvent *event, double EnuMin, double EnuMax,
                                  int cuts);

enum PRD93112012_Ana {
  kAnalysis_I,
  kAnalysis_II,
};

bool isT2K_CC0pi(FitEvent *event, double EnuMin, double EnuMax,
                 int analysis);
                 
bool isT2K_CC0piNp(FitEvent *event, double EnuMin, double EnuMax);
bool isT2K_CC0pi1p(FitEvent *event, double EnuMin, double EnuMax);
bool isT2K_CC0pi0p(FitEvent *event, double EnuMin, double EnuMax);
bool isT2K_CC0pi_STV(FitEvent *event, double EnuMin, double EnuMax);
bool isT2K_CC0pi_1bin(FitEvent *event, double EnuMin, double EnuMax);
bool isT2K_CC0pi_ifk(FitEvent *event, double EnuMin, double EnuMax);
bool isT2K_CC0piAnuP0D(FitEvent *event, double EnuMin, double EnuMax); // TN328
} // namespace SignalDef

#endif
