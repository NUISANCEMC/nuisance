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

#include "SignalDef.h"
#include "FitUtils.h"

#include "MINERvA_SignalDef.h"

namespace SignalDef {

// *********************************
// MINERvA CC1pi+/- signal definition (2015 release)
// Note:  There is a 2016 release which is different to this (listed below), but
// it is CCNpi+ and has a different W cut
// Note2: The W cut is implemented in the class implementation in MINERvA/
// rather than here so we can draw events that don't pass the W cut (W cut is
// 1.4 GeV)
//        Could possibly be changed for slight speed increase since less events
//        would be used
//
// MINERvA signal is slightly different to MiniBooNE
//
// Exactly one negative muon
// Exactly one charged pion (both + and -); however, there is a Michel e-
// requirement but UNCLEAR IF UNFOLDED OR NOT (so don't know if should be
// applied)
// Exactly 1 charged pion exits (so + and - charge), however, has Michel
// electron requirement, so look for + only here?
// No restriction on neutral pions or other mesons
// MINERvA has unfolded and not unfolded muon phase space for 2015
//
// Possible problems:
// 1) Should there be a pi+ only cut implemented due to Michel requirement, or
// is pi- events filled from MC?
// 2) There is a T_pi < 350 MeV cut coming from requiring a stopping pion so the
// Michel e is seen, this is also unclear if it's unfolded so any pion is OK
//
// Nice things:
// Much data given: with and without muon angle cuts and with and without shape
// only data + covariance
//
bool isCC1pip_MINERvA(FitEvent *event, double EnuMin, double EnuMax,
                      bool isRestricted) {
  // *********************************

  // Signal is both pi+ and pi-
  // WARNING: PI- CONTAMINATION IS FULLY GENIE BECAUSE THE MICHEL TAG
  // First, make sure it's CCINC
  if (!isCCINC(event, 14, EnuMin, EnuMax)) return false;

  // Allow pi+/pi-
  int piPDG[] = {211, -211};
  int nLeptons = event->NumFSLeptons();
  int nPion = event->NumFSParticle(piPDG);

  // Check that the desired pion exists and is the only meson
  if (nPion != 1) return false;

  // Check that there is only one final state lepton
  if (nLeptons != 1) return false;

  TLorentzVector pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;

  // Pion kinetic energy requirement for Michel tag, leave commented for now
  // if (FitUtils::T(ppi)*1000. > 350 || FitUtils::T(ppi)*1000. < 35) return
  // false; // Need to have a 35 to 350 MeV pion kinetic energy requirement

  // MINERvA released another CC1pi+ xsec without muon unfolding!
  // here the muon angle is < 20 degrees (seen in MINOS)
  if (isRestricted) {
    double th_nu_mu = FitUtils::th(pmu, pnu) * 180. / M_PI;
    if (th_nu_mu >= 20) return false;
  }

  return true;
};

// *********************************
// MINERvA CCNpi+/- signal definition from 2016 publication
// Different to CC1pi+/- listed above; additional has W < 1.8 GeV
//
// Still asks for a Michel e and still unclear if this is unfolded or not
// Says stuff like "requirement that a Michel e isolates a subsample that is
// more nearly a pi+ prodution", yet the signal definition is both pi+ and pi-?
//
// One negative muon
// At least one charged pion
// 1.5 < Enu < 10
// No restrictions on pi0 or other mesons or baryons
//
// Also writes number of pions (nPions) if studies on this want to be done...
bool isCCNpip_MINERvA(FitEvent *event, int &nPions, double EnuMin,
                      double EnuMax, bool isRestricted) {
  // *********************************

  // First, make sure it's CCINC
  if (!isCCINC(event, 14, EnuMin, EnuMax)) return false;

  int nLeptons = event->NumFSLeptons();

  // Write the number of pions to the measurement class...
  // Maybe better to just do that inside the class?
  int pdgs[] = {-211, 211};
  nPions = event->NumFSParticle(pdgs);

  // Check that there is a pion!
  if (nPions == 0) return false;

  // Check that there is only one final state lepton
  if (nLeptons != 1) return false;

  TLorentzVector pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;

  // MINERvA released another CC1pi+ xsec without muon unfolding!
  // Here the muon angle is < 20 degrees (seen in MINOS)
  if (isRestricted) {
    double th_nu_mu = FitUtils::th(pmu, pnu) * 180. / M_PI;
    if (th_nu_mu >= 20) return false;
  }

  return true;
};

//********************************************************************
bool isCCQEnumu_MINERvA(FitEvent *event, double EnuMin, double EnuMax,
                        bool fullphasespace) {
  //********************************************************************

  if (!isCCQELike(event, 14, EnuMin, EnuMax)) return false;

  TLorentzVector pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;

  double ThetaMu = pnu.Vect().Angle(pmu.Vect());
  double Enu_rec = FitUtils::EnuQErec(pmu, cos(ThetaMu), 34., true);

  // If Restricted phase space
  if (!fullphasespace && ThetaMu > 0.34906585) return false;

  // restrict energy range
  if (Enu_rec < EnuMin || Enu_rec > EnuMax) return false;

  return true;
};

//********************************************************************
bool isCCQEnumubar_MINERvA(FitEvent *event, double EnuMin, double EnuMax,
                           bool fullphasespace) {
  //********************************************************************

  if (!isCCQELike(event, -14, EnuMin, EnuMax)) return false;

  TLorentzVector pnu = event->GetHMISParticle(-14)->fP;
  TLorentzVector pmu = event->GetHMFSParticle(-13)->fP;

  double ThetaMu = pnu.Vect().Angle(pmu.Vect());
  double Enu_rec = FitUtils::EnuQErec(pmu, cos(ThetaMu), 30., true);

  // If Restricted phase space
  if (!fullphasespace && ThetaMu > 0.34906585) return false;

  // restrict energy range
  if (Enu_rec < EnuMin || Enu_rec > EnuMax) return false;

  return true;
}

//********************************************************************
bool isCCincLowRecoil_MINERvA(FitEvent *event, double EnuMin, double EnuMax) {
  //********************************************************************

  if (!isCCINC(event, 14, EnuMin, EnuMax)) return false;

  // Need at least one muon
  if (event->NumFSParticle(13) < 1) return false;
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector pnu = event->GetHMISParticle(14)->fP;

  // Cut on muon angle greated than 20deg
  if (pnu.Vect().Angle(pmu.Vect()) < 0.93969262078) return false;

  // Cut on muon energy < 1.5 GeV
  if (pmu.E() < 1.5) return false;

  return true;
}

bool isCC0pi1p_MINERvA(FitEvent *event, double enumin, double enumax) {
  // Require numu CC0pi event with a proton above threshold
  bool signal = (isCC0pi(event, 14, enumin, enumax) &&
                 HasProtonKEAboveThreshold(event, 110.0));

  return signal;
}
}
