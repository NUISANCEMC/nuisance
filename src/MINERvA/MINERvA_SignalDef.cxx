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
// Possible issues with the data:
// 1) pi- is allowed in signal even when Michel cut included; most pi- is efficiency corrected in GENIE
// 2) There is a T_pi < 350 MeV cut coming from requiring a stopping pion; this is efficiency corrected in GENIE
// 3) There is a 1.5 < Enu < 10.0 cut in signal definition
// 4) There is an angular muon cut which is sometimes efficiency corrected (why we have bool isRestricted below)
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

  // MINERvA released another CC1pi+ xsec without muon unfolding!
  // here the muon angle is < 20 degrees (seen in MINOS)
  TLorentzVector pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;

  if (isRestricted) {
    double th_nu_mu = FitUtils::th(pmu, pnu) * 180. / M_PI;
    if (th_nu_mu >= 20) return false;
  }

  // Extract Hadronic Mass
  double hadMass = FitUtils::Wrec(pnu, pmu);

  // Actual cut is True GENIE Ws! Arg.! Use gNtpcConv definition.
#ifdef __GENIE_ENABLED__
  if (event->fType == kGENIE){
    EventRecord *  gevent      = static_cast<EventRecord*>(event->genie_event->event);
    const Interaction * interaction = gevent->Summary();
    const Kinematics &   kine       = interaction->Kine();
    double Ws  = kine.W (true);
    //    std::cout << "Ws versus WRec = " << Ws << " vs " << hadMass << " " << kine.W(false) << std::endl;
    hadMass = Ws * 1000.0;
  }
#endif
  if (hadMass > 1400.0) return false;

  return true;
};

// *********************************
// MINERvA CCNpi+/- signal definition from 2016 publication
// Different to CC1pi+/- listed above; additional has W < 1.8 GeV
//
// For notes on strangeness of signal definition, see CC1pip_MINERvA
//
// One negative muon
// At least one charged pion
// 1.5 < Enu < 10
// No restrictions on pi0 or other mesons or baryons
// W_reconstructed (ignoring initial state motion) cut at 1.8 GeV
//
// Also writes number of pions (nPions) if studies on this want to be done...
bool isCCNpip_MINERvA(FitEvent *event, double EnuMin,
                      double EnuMax, bool isRestricted, bool isWtrue) {
  // *********************************

  // First, make sure it's CCINC
  if (!isCCINC(event, 14, EnuMin, EnuMax)) return false;

  int nLeptons = event->NumFSLeptons();

  // Write the number of pions to the measurement class...
  // Maybe better to just do that inside the class?
  int nPions = event->NumFSParticle(PhysConst::pdg_charged_pions);

  // Check that there is a pion!
  if (nPions == 0) return false;

  // Check that there is only one final state lepton
  if (nLeptons != 1) return false;

  // Need the muon and the neutrino to check angles and W

  TLorentzVector pnu = event->GetNeutrinoIn()->fP;
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;
  // MINERvA released some data with restricted muon angle
  // Here the muon angle is < 20 degrees (seen in MINOS)
  if (isRestricted) {

    double th_nu_mu = FitUtils::th(pmu, pnu) * 180. / M_PI;
    if (th_nu_mu >= 20.) return false;
  }

  // Lastly check the W cut (always at 1.8 GeV)
  double Wrec = FitUtils::Wrec(pnu, pmu) + 0.;

  // Actual cut is True GENIE Ws! Arg.! Use gNtpcConv definition.
  if (isWtrue){
#ifdef __GENIE_ENABLED__
    if (event->fType == kGENIE){
      GHepRecord* ghep = static_cast<GHepRecord*>(event->genie_event->event);
      const Interaction * interaction = ghep->Summary();
      const Kinematics &   kine       = interaction->Kine();
      double Ws  = kine.W (true);
      Wrec = Ws * 1000.0; // Say Wrec is Ws
    }
#endif
  }

  if (Wrec > 1800. || Wrec < 0.0) return false;


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
  if (cos(pnu.Vect().Angle(pmu.Vect())) < 0.93969262078) return false;

  // Cut on muon energy < 1.5 GeV
  if (pmu.E()/1000.0 < 1.5) return false;

  return true;
}

bool isCC0pi1p_MINERvA(FitEvent *event, double enumin, double enumax) {
  // Require numu CC0pi event with a proton above threshold
  bool signal = (isCC0pi(event, 14, enumin, enumax) &&
                 HasProtonKEAboveThreshold(event, 110.0));

  return signal;
}

// 2015 analysis just asks for 1pi0 and no pi+/pi-
bool isCC1pi0_MINERvA_2015(FitEvent *event, double EnuMin, double EnuMax) {
  bool CC1pi0_anu = SignalDef::isCC1pi(event, -14, 111, EnuMin, EnuMax);
  return CC1pi0_anu;
}

// 2016 analysis just asks for 1pi0 and no other charged tracks
bool isCC1pi0_MINERvA_2016(FitEvent *event, double EnuMin, double EnuMax) {
  bool CC1pi0_anu = SignalDef::isCC1pi(event, -14, 111, EnuMin, EnuMax);
  
  /*
  // Additionally look for charged proton track
  bool HasProton = event->HasFSParticle(2212);

  
  if (CC1pi0_anu) {
    if (!HasProton) {
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
  */

  return CC1pi0_anu;
}

}
