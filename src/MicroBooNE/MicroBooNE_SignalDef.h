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

#ifndef MICROBOONE_SIGNALDEF_H_SEEN
#define MICROBOONE_SIGNALDEF_H_SEEN

#include "SignalDef.h"

namespace SignalDef {
  namespace MicroBooNE {

/**
 * numu CC with 1 muon, N>0 protons, and no pions.
 *
 * Phys. Rev. D 102, 112013 (2020), arxiv:2010.02390.
 */
bool isCC1MuNp(FitEvent* event, double EnuMin, double EnuMax);

/**
 * numu CC with 1 muon (0.1 < Pmu < 1.2 GeV/c), 2 protons (0.3 < Pp < 1 GeV/c), no neutral pions (any momenta), and no charged pions above 65 MeV/c [any number of neutrons is allowed].
 *
 * publication reference to be updated
 */
bool isCC1Mu2p(FitEvent* event, double EnuMin, double EnuMax);

/**
 * numu CC with 1 muon (0.1 < Pmu < 1.2 GeV/c), 1 proton (0.3 < Pp < 1 GeV/c), no neutral pions (any momenta), and no charged pions above 70 MeV/c [any number of neutrons is allowed].
 *
 * publication reference to be updated
 */
bool isCC1Mu1p(FitEvent* event, double EnuMin, double EnuMax);



  }  // namespace MicroBooNE
}  // namespace SignalDef

#endif

