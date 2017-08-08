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

#ifndef MINERVA_SIGNALDEF_H_SEEN
#define MINERVA_SIGNALDEF_H_SEEN

#include "FitEvent.h"

namespace SignalDef {

// *********************************
/// MINERvA CC1pi+/- signal definition (2015 release)
/// Note:  There is a 2016 release which is different to this (listed below),
/// but
/// it is CCNpi+ and has a different W cut
/// Note2: The W cut is implemented in the class implementation in MINERvA/
/// rather than here so we can draw events that don't pass the W cut (W cut is
/// 1.4 GeV)
///        Could possibly be changed for slight speed increase since less events
///        would be used
///
/// MINERvA signal is slightly different to MiniBooNE
///
/// Exactly one negative muon
/// Exactly one charged pion (both + and -); however, there is a Michel e-
/// requirement but UNCLEAR IF UNFOLDED OR NOT (so don't know if should be
/// applied)
/// Exactly 1 charged pion exits (so + and - charge), however, has Michel
/// electron requirement, so look for + only here?
/// No restriction on neutral pions or other mesons
/// MINERvA has unfolded and not unfolded muon phase space for 2015
///
/// Possible problems:
/// 1) Should there be a pi+ only cut implemented due to Michel requirement, or
/// is pi- events filled from MC?
/// 2) There is a T_pi < 350 MeV cut coming from requiring a stopping pion so
/// the
/// Michel e is seen, this is also unclear if it's unfolded so any pion is OK
///
/// Nice things:
/// Much data given: with and without muon angle cuts and with and without shape
/// only data + covariance
bool isCC1pip_MINERvA(FitEvent *event, double EnuMin, double EnuMax,
                      bool isRestricted = false);
// *********************************
/// MINERvA CCNpi+/- signal definition from 2016 publication
/// Different to CC1pi+/- listed above; additional has W < 1.8 GeV
///
/// Still asks for a Michel e and still unclear if this is unfolded or not
/// Says stuff like "requirement that a Michel e isolates a subsample that is
/// more nearly a pi+ prodution", yet the signal definition is both pi+ and pi-?
///
/// One negative muon
/// At least one charged pion
/// 1.5 < Enu < 10
/// No restrictions on pi0 or other mesons or baryons
///
/// Also writes number of pions (nPions) if studies on this want to be done...
bool isCCNpip_MINERvA(FitEvent *event, double EnuMin, double EnuMax,
                      bool isRestricted = false, bool isWtrue=false);

bool isCCQEnumu_MINERvA(FitEvent *event, double EnuMin, double EnuMax,
                        bool fullphasespace = true);
bool isCCQEnumubar_MINERvA(FitEvent *event, double EnuMin, double EnuMax,
                           bool fullphasespace = true);

bool isCCincLowRecoil_MINERvA(FitEvent *event, double EnuMin, double EnuMax);

bool isCC0pi1p_MINERvA(FitEvent *event, double enumin, double enumax);

bool isCC1pi0_MINERvA_2015(FitEvent *event, double EnuMin, double EnuMax);
bool isCC1pi0_MINERvA_2016(FitEvent *event, double EnuMin, double EnuMax);

}

#endif
