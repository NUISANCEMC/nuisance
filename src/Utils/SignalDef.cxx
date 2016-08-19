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

bool SignalDef::isCCQE(FitEvent *event, double EnuMin, double EnuMax, bool isRestricted) {

  if (event->Mode != 1 && event->Mode != 2) return false;
  if (event->PartInfo(0)->fPID != 14) return false;
  if ((event->PartInfo(0)->fP.E() < EnuMin*1000.) || (event->PartInfo(0)->fP.E() > EnuMax*1000.)) return false;

// NO MESONS and ONE MUON
  int lepCnt = 0;

  TLorentzVector pnu = (event->PartInfo(0))->fP;
  TLorentzVector pmu;

  for (unsigned int j = 2; j < event->Npart(); j++) {
    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; //move on if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (abs(PID) >= 111 && abs(PID) <= 557) return false;
    else if (abs(PID) == 11 || abs(PID) == 13 || abs(PID) == 15 || abs(PID) == 17) {
      lepCnt++;
      if (isRestricted) pmu = (event->PartInfo(j))->fP;
    }
  }

  if (lepCnt != 1) return false;

  if (isRestricted) {
    double th_nu_mu = FitUtils::th(pmu, pnu) * 180./M_PI;
    if (th_nu_mu >= 20) return false;
  }

  return true;
};

bool SignalDef::isCCQEBar(FitEvent *event, double EnuMin, double EnuMax, bool isRestricted) {
  // checks mode is correct
  if (event->Mode != -1 && event->Mode != -2) return false;
  if (event->PartInfo(0)->fPID != -14) return false;
  if ((event->PartInfo(0)->fP.E() < EnuMin*1000.) || (event->PartInfo(0)->fP.E() > EnuMax*1000.)) return false;

  //checks that we have NO mesons and ONE muon
  int lepCnt = 0;

  TLorentzVector pnu = (event->PartInfo(0))->fP;
  TLorentzVector pmu;

  for (unsigned int j = 2; j < event->Npart(); j++) {
    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; //move on if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (abs(PID) >= 111 && abs(PID) <= 557) return false;
    else if (abs(PID) == 11 || abs(PID) == 13 || abs(PID) == 15 || abs(PID) == 17) {
      lepCnt++;
      if (isRestricted) pmu = (event->PartInfo(j))->fP;
    }
  }

  if (lepCnt != 1) return false;

  if (isRestricted) {
    double th_nu_mu = FitUtils::th(pmu, pnu) * 180./M_PI;
    if (th_nu_mu >= 20) return false;
  }

  return true;
};

bool SignalDef::isCCQELike(FitEvent *event, double EnuMin, double EnuMax) {
  if (event->PartInfo(0)->fPID != 14) return false;
  if ((event->PartInfo(0)->fP.E() < EnuMin*1000.) || (event->PartInfo(0)->fP.E() > EnuMax*1000.)) return false;
  if (((event->PartInfo(2))->fPID != 13) && ((event->PartInfo(3))->fPID != 13)) return false;

  int lepCnt = 0;

  for (unsigned int j = 2; j < event->Npart(); j++) {
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0) continue;
    int PID = (event->PartInfo(j))->fPID;
    if (abs(PID) >= 110 && abs(PID) <= 557) return false;
    //else if (abs(PID) == 1114 || abs(PID) == 2114 || (abs(PID) >= 2214 && abs(PID) <= 5554)) return false;
    else if (abs(PID) == 11 || abs(PID) == 13 || abs(PID) == 15 || abs(PID) == 17) lepCnt++;
  }

  if (lepCnt != 1) return false;

  return true;
};

bool SignalDef::isCCQELikeBar(FitEvent *event, double EnuMin, double EnuMax) {
  if (event->PartInfo(0)->fPID != -14) return false;
  if ((event->PartInfo(0)->fP.E() < EnuMin*1000.) || (event->PartInfo(0)->fP.E() > EnuMax*1000.)) return false;

  for (unsigned int j = 2; j < event->Npart(); j++) {
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0)
      continue; // maybe need not 2212 2112?
    if ((event->PartInfo(j))->fPID != 22 &&   // photon OK
        (event->PartInfo(j))->fPID != 2212 && // neutron OK
        (event->PartInfo(j))->fPID != 2112 && // proton OK
        (event->PartInfo(j))->fPID != -13)     // muon OK
      return false;
    }
  return true;
};

bool SignalDef::isMiniBooNE_CCQELike(FitEvent *event, double EnuMin, double EnuMax) {

  if (abs(event->PartInfo(0)->fPID) != 14) return false;
  if ((event->PartInfo(0)->fP.E() < EnuMin*1000.) || (event->PartInfo(0)->fP.E() > EnuMax*1000.)) return false;

  int lepCnt = 0;

  for (unsigned int j = 2; j < event->Npart(); j++) {
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0)
      continue; // maybe need not 2212 2112?

    int PID = event->PartInfo(j)->fPID;
    if (abs(PID) == 11 || abs(PID) == 13 || abs(PID) == 15 || abs(PID) == 17) lepCnt++;

    if ((event->PartInfo(j))->fPID != 22 &&   // photon OK
        (event->PartInfo(j))->fPID != 2212 && // neutron OK
        (event->PartInfo(j))->fPID != 2112 && // proton OK
        abs((event->PartInfo(j))->fPID) != 13)     // muon OK
      return false;
  }
  
  if (lepCnt != 1) return false;

  return true;
};

bool SignalDef::isMiniBooNE_CCQE(FitEvent *event, double EnuMin, double EnuMax) {

  // Only NUMU
  if (event->PartInfo(0)->fPID != 14) return false;

  // E Within Range
  if ((event->PartInfo(0)->fP.E() < EnuMin*1000.) || (event->PartInfo(0)->fP.E() > EnuMax*1000.)) return false;

  // Mode == 1 or 2
  if (event->Mode != 2 and event->Mode != 1) return false;

  return true;
}

bool SignalDef::isMiniBooNE_CCQEBar(FitEvent *event, double EnuMin, double EnuMax) {

  if (event->PartInfo(0)->fPID != -14) return false;

  if ((event->PartInfo(0)->fP.E() < EnuMin*1000.) || (event->PartInfo(0)->fP.E() > EnuMax*1000.)) return false;

  if (event->Mode != -2 and event->Mode != -1) return false;

  return true;
}


// *********************************************
// MiniBooNE CC1pi+ signal definition
// Warning: This one is a little scary because there's a W = 1.35 GeV cut for signal in the selection
//          Although this is unfolded over and is filled up with NUANCE
//          So there is actually no W cut applied, but everything above W = 1.35 GeV is NUANCE...
// 
// The signal definition is:
//                            Exactly one negative muon
//                            Exactly one positive pion
//                            No other mesons
//                            No requirements on photons, nucleons and multi-nucleons
//                            Doesn't mention other leptons
//
// Additionally, it asks for 2 Michel e- from decay of muon and pion
// So there is good purity and we can be fairly sure that the positive pion is a positive pion
bool SignalDef::isCC1pip_MiniBooNE(FitEvent *event, double EnuMin, double EnuMax) {
// *********************************************

  // Do some initial checks on the incoming neutrino
  // Make sure it's a muon neutrino
  if ((event->PartInfo(0))->fPID != 14) return false;

  // Make sure the muon neutrino is within the E_nu defined at the experiment
  if (((event->PartInfo(0))->fP.E() < EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > EnuMax*1000.)) return false;

  // Make sure the outgoing lepton is a muon
  if (((event->PartInfo(2))->fPID != 13) && ((event->PartInfo(3))->fPID != 13)) return false;

  int pipCnt = 0; // Counts number of pions
  int lepCnt = 0; // Counts number of muons

  for (unsigned int j = 2; j < event->Npart(); j++) {

    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; // Move on if NOT ALIVE and NOT NORMAL

    int PID = (event->PartInfo(j))->fPID;

    // Reject other mesons than pi+
    if ((abs(PID) >= 111 && abs(PID) <= 210) || (abs(PID) >= 212 && abs(PID) <= 557) || PID == -211) {
      return false;
    // Reject other leptons
    } else if (abs(PID) == 11 || PID == -13 || abs(PID) == 15 || abs(PID) == 17) {
      return false;
    // Count the number of muons
    } else if (PID == 13) {
      lepCnt++;
    // Count the number of pions
    } else if (PID == 211) {
      pipCnt++;
    }
  }

  // Make sure there's only one pion
  if (pipCnt != 1) {
    return false;
  }
  // Make sure there's only one muon
  if (lepCnt != 1) {
    return false;
  }

  // If it's passed all of the above we're good and have passed the selection
  return true;
};

// **************************************************
// MiniBooNE CC1pi0 signal definition
//
// The signal definition is:
//                          Exactly one negative muon
//                          Exactly one pi0 
//                          No additional mesons
//                          Any number of nucleons
//
// Does a few clever cuts on the likelihood to reduce CCQE contamination by looking at "fuzziness" of the ring; CCQE events are sharp, CC1pi0 are fuzzy (because of the pi0->2 gamma collinearity)
bool SignalDef::isCC1pi0_MiniBooNE(FitEvent *event, double EnuMin, double EnuMax) {
// **************************************************

  if ((event->PartInfo(0))->fPID != 14) return false;

  if (((event->PartInfo(0))->fP.E() < EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > EnuMax*1000.)) return false;

  if (((event->PartInfo(2))->fPID != 13) && ((event->PartInfo(3))->fPID != 13)) return false;

  int pi0Cnt = 0;
  int lepCnt = 0;

  for (unsigned int j = 2; j < event->Npart(); j++) {
    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; //move to next particle if NOT ALIVE and NOT NORMAL

    int PID = (event->PartInfo(j))->fPID;
    // Reject if any other mesons
    if (abs(PID) >= 113 && abs(PID) <= 557) {
      return false;
    // Reject other leptons
    } else if (abs(PID) == 11 || PID == -13 || abs(PID) == 15) {
      return false;
    // Count number of leptons
    } else if (PID == 13) {
      lepCnt++;
    // Count number of pi0
    } else if (PID == 111) {
      pi0Cnt++;
    }
  }

  if (pi0Cnt != 1) return false;
  if (lepCnt != 1) return false;

  return true;
};

// **************************************
// MINERvA CC1pi0 in anti-neutrino mode
// Unfortunately there's no information on the neutrino component which is subtracted off
//
// 2014 analysis:   
//                Exactly one positive muon
//                Exactly one observed pi0
//                No pi+/pi allowed
//                No information on what is done with mesons, oops?
//                No information on what is done with nucleons, oops?
//
// 2016 analysis:
//                Exactly one positive muon
//                Exactly one observed pi0
//                No other mesons
//                No restriction on number of nucleons
//
bool SignalDef::isCC1pi0Bar_MINERvA(FitEvent *event, double EnuMin, double EnuMax) {
// **************************************

  if ((event->PartInfo(0))->fPID != -14) return false;

  if (((event->PartInfo(0))->fP.E() < EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > EnuMax*1000.)) return false;

  if (((event->PartInfo(2))->fPID != -13) && ((event->PartInfo(3))->fPID != -13)) return false;

  int pi0Cnt = 0;
  int lepCnt = 0;

  for (unsigned int j = 2; j < event->Npart(); j++) {

    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; // Move to next particle if NOT ALIVE and NOT NORMAL

    int PID = (event->PartInfo(j))->fPID;

    // No other mesons
    if (abs(PID) >= 113 && abs(PID) <= 557) {
      return false;
    } else if (PID == -13) {
      lepCnt++;
    } else if (PID == 111) {
      pi0Cnt++;
    }
  }

  if (pi0Cnt != 1) return false;
  if (lepCnt != 1) return false;

  return true;
};

bool SignalDef::isNC1pi0_MiniBooNE(FitEvent *event, double EnuMin, double EnuMax) {
  if ((event->PartInfo(0))->fPID != 14) return false;

  if (((event->PartInfo(0))->fP.E() < EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > EnuMax*1000.)) return false;

  if (((event->PartInfo(2))->fPID != 14) && ((event->PartInfo(3))->fPID != 14)) return false;

  int pi0Cnt = 0;

  for (unsigned int j = 2; j < event->Npart(); j++) {
    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; //move to next particle if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (abs(PID) >= 113 && abs(PID) <= 557) return false;
    //else if (abs(PID) == 1114 || abs(PID) == 2114 || (abs(PID) >= 2214 && abs(PID) <= 5554)) return false;
    else if (abs(PID) == 11 || abs(PID) == 13 || abs(PID) == 15 || abs(PID) == 17) return false;
    else if (PID == 111) pi0Cnt++;
    }

  if (pi0Cnt != 1) return false;

  return true;
};

bool SignalDef::isNC1pi0Bar_MiniBooNE(FitEvent *event, double EnuMin, double EnuMax) {
  if ((event->PartInfo(0))->fPID != -14) return false;

  if (((event->PartInfo(0))->fP.E() < EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > EnuMax*1000.)) return false;

  if (((event->PartInfo(2))->fPID != -14) && ((event->PartInfo(3))->fPID != -14)) return false;

  int pi0Cnt = 0;

  for (unsigned int j = 2; j < event->Npart(); j++) {
    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; //move to next particle if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (abs(PID) >= 113 && abs(PID) <= 557) return false;
    //else if (abs(PID) == 1114 || abs(PID) == 2114 || (abs(PID) >= 2214 && abs(PID) <= 5554)) return false;
    else if (abs(PID) == 11 || abs(PID) == 13 || abs(PID) == 15 || abs(PID) == 17) return false;
    else if (PID == 111) pi0Cnt++;
    }

  if (pi0Cnt != 1) return false;

  return true;
};

bool SignalDef::isCCcoh_MINERvA(FitEvent *event, double EnuMin, double EnuMax) {
  if ((event->PartInfo(0))->fPID != 14) return false;
  if (((event->PartInfo(0))->fP.E() < EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > EnuMax*1000.)) return false;
  if (((event->PartInfo(2))->fPID != 13) && ((event->PartInfo(3))->fPID != 13)) return false;

  int pipCnt = 0; // counts number of pions
  int lepCnt = 0;

  //double vertexE = 0;

  for (unsigned int j = 2; j < event->Npart(); j++) {
    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; //move on if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 13) lepCnt++;
    else if (PID == 211) pipCnt++;
    else return false; // CCcoh definition is only 1 pi, only 1 lep
  }

  if (pipCnt != 1) return false;
  if (lepCnt != 1) return false;

  return true;
};

bool SignalDef::isCCcohBar_MINERvA(FitEvent *event, double EnuMin, double EnuMax) {

  if ((event->PartInfo(0))->fPID != -14) return false;
  if (((event->PartInfo(0))->fP.E() < EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > EnuMax*1000.)) return false;
  if (((event->PartInfo(2))->fPID != -13) && ((event->PartInfo(3))->fPID != -13)) return false;

  int pipCnt = 0; // counts number of pions
  int lepCnt = 0;

  //double vertexE = 0;

  for (unsigned int j = 2; j < event->Npart(); j++) {
    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; //move on if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (PID == -13) lepCnt++;
    else if (PID == -211) pipCnt++;
    else return false; // CCcoh definition is only 1 pi, only 1 lep
  }

  if (pipCnt != 1) return false;
  if (lepCnt != 1) return false;

  return true;
};

// *********************************
// MINERvA CC1pi+/- signal definition (2015 release)
// Note:  There is a 2016 release which is different to this (listed below), but it is CCNpi+ and has a different W cut
// Note2: The W cut is implemented in the class implementation in MINERvA/ rather than here so we can draw events that don't pass the W cut (W cut is 1.4 GeV)
//        Could possibly be changed for slight speed increase since less events would be used
//
// MINERvA signal is slightly different to MiniBooNE
//
// Exactly one negative muon
// Exactly one charged pion (both + and -); however, there is a Michel e- requirement but UNCLEAR IF UNFOLDED OR NOT (so don't know if should be applied)
// Exactly 1 charged pion exits (so + and - charge), however, has Michel electron requirement, so look for + only here?
// No restriction on neutral pions or other mesons
// MINERvA has unfolded and not unfolded muon phase space for 2015
//
// Possible problems:
// 1) Should there be a pi+ only cut implemented due to Michel requirement, or is pi- events filled from MC?
// 2) There is a T_pi < 350 MeV cut coming from requiring a stopping pion so the Michel e is seen, this is also unclear if it's unfolded so any pion is OK
//
// Nice things:
// Much data given: with and without muon angle cuts and with and without shape only data + covariance
//
bool SignalDef::isCC1pip_MINERvA(FitEvent *event, double EnuMin, double EnuMax, bool isRestricted) {
// *********************************

  if ((event->PartInfo(0))->fPID != 14) return false;

  if (((event->PartInfo(0))->fP.E() < EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > EnuMax*1000.)) return false;

  if (((event->PartInfo(2))->fPID != 13) && ((event->PartInfo(3))->fPID != 13)) return false;

  int pipCnt = 0; // Counts number of pi+
  int lepCnt = 0; // Counts number of leptons

  TLorentzVector pnu = (event->PartInfo(0))->fP;
  TLorentzVector pmu;
  TLorentzVector ppi;

  for (unsigned int j = 2; j < event->Npart(); j++) {

    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; // Move on if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;

    if (PID == 13) {
      lepCnt++;
      // if restricted we need the muon to find it's angle and if it's visible in MINOS
      if (isRestricted) {
        pmu = (event->PartInfo(j))->fP;
      }

    // Signal is both pi+ and pi-
    // WARNING: PI- CONTAMINATION IS FULLY GENIE BECAUSE THE MICHEL TAG
    } else if (abs(PID) == 211) {

      ppi = event->PartInfo(j)->fP;
      pipCnt++;
    }
  }

  // Only one pion-like track
  if (pipCnt != 1) return false;
  // only one lepton
  if (lepCnt != 1) return false;

  // Pion kinetic energy requirement for Michel tag, leave commented for now
  //if (FitUtils::T(ppi)*1000. > 350 || FitUtils::T(ppi)*1000. < 35) return false; // Need to have a 35 to 350 MeV pion kinetic energy requirement

  // MINERvA released another CC1pi+ xsec without muon unfolding!
  // here the muon angle is < 20 degrees (seen in MINOS)
  if (isRestricted) {
    double th_nu_mu = FitUtils::th(pmu, pnu) * 180./M_PI;
    if (th_nu_mu >= 20) return false;
  }

  return true;
};

// *********************************
// MINERvA CCNpi+/- signal definition from 2016 publication
// Different to CC1pi+/- listed above; additional has W < 1.8 GeV
//
// Still asks for a Michel e and still unclear if this is unfolded or not
// Says stuff like "requirement that a Michel e isolates a subsample that is more nearly a pi+ prodution", yet the signal definition is both pi+ and pi-?
//
// One negative muon 
// At least one charged pion
// 1.5 < Enu < 10
// No restrictions on pi0 or other mesons or baryons
//
// Also writes number of pions (nPions) if studies on this want to be done...
bool SignalDef::isCCNpip_MINERvA(FitEvent *event, int &nPions, double EnuMin, double EnuMax, bool isRestricted) {
// *********************************

  if ((event->PartInfo(0))->fPID != 14) return false;

  if (((event->PartInfo(0))->fP.E() < EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > EnuMax*1000.)) return false;

  if (((event->PartInfo(2))->fPID != 13) && ((event->PartInfo(3))->fPID != 13)) return false;

  int pipCnt = 0; // Counts number of pions
  int lepCnt = 0; // Counts number of leptons

  TLorentzVector pnu = (event->PartInfo(0))->fP;
  TLorentzVector pmu;

  for (unsigned int j = 2; j < event->Npart(); j++) {
    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; // Move on if NOT ALIVE and NOT NORMAL

    int PID = (event->PartInfo(j))->fPID;
    if (PID == 13) {
      lepCnt++;
      if (isRestricted) {
        pmu = (event->PartInfo(j))->fP;
      }

    } else if (abs(PID) == 211) {
      pipCnt++; // technically also allows for a pi- to be ID as pi+, but there's Michel electron criteria too which eliminates this
    }
    // Has no restrictions on mesons, neutral pions or baryons
  }

  // Any number of pions greater than 0
  if (pipCnt == 0) return false;
  // Only one lepton
  if (lepCnt != 1) return false;

  // Just write the number of pions so the experiment class can use it
  nPions = pipCnt;

  // MINERvA released another CC1pi+ xsec without muon unfolding!
  // Here the muon angle is < 20 degrees (seen in MINOS)
  if (isRestricted) {
    double th_nu_mu = FitUtils::th(pmu, pnu) * 180./M_PI;
    if (th_nu_mu >= 20) return false;
  }

  return true;
};

// T2K H2O signal definition
bool SignalDef::isCC1pip_T2K_H2O(FitEvent *event, double EnuMin, double EnuMax) {
  if ((event->PartInfo(0))->fPID != 14) return false;

  if (((event->PartInfo(0))->fP.E() < EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > EnuMax*1000.)) return false;

  if (((event->PartInfo(2))->fPID != 13) && ((event->PartInfo(3))->fPID != 13)) return false;

  int pipCnt = 0; // counts number of pions
  int lepCnt = 0;

  TLorentzVector Pnu = (event->PartInfo(0))->fP;
  TLorentzVector Ppip;
  TLorentzVector Pmu;

  for (unsigned int j = 2; j < event->Npart(); j++) {
    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; //move on if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if ((abs(PID) >= 111 && abs(PID) <= 210) || (abs(PID) >= 212 && abs(PID) <= 557) || PID == -211) return false;
    else if (abs(PID) == 11 || abs(PID) == 13 || abs(PID) == 15 || abs(PID) == 17) {
      lepCnt++;
      Pmu = (event->PartInfo(j))->fP;
    }
    else if (PID == 211) {
      pipCnt++;
      Ppip = (event->PartInfo(j))->fP;
    }
  }

  if (pipCnt != 1) return false;
  if (lepCnt != 1) return false;

  // relatively generic CC1pi+ definition done
  // now measurement specific (p_mu > 200 MeV, p_pi > 200 MeV, cos th_mu > 0.3,
  // cos th_pi > 0.3 in TRUE AND RECONSTRUCTED!

  double p_mu = FitUtils::p(Pmu)*1000;
  double p_pi = FitUtils::p(Ppip)*1000;
  double cos_th_mu = cos(FitUtils::th(Pnu, Pmu));
  double cos_th_pi = cos(FitUtils::th(Pnu, Ppip));

  if (p_mu <= 200 || p_pi <= 200 || cos_th_mu <= 0.3 || cos_th_pi <= 0.3) return false;


  return true;
};

// ******************************************************
// T2K CC1pi+ CH analysis (Raquel's thesis)
// Has different phase space cuts depending on if using Michel tag or not
//
// Essentially consists of two samples: one sample which has Michel e (which we can't get pion direction from); this covers backwards angles quite well. Measurements including this sample does not have include pion kinematics cuts
//                                      one sample which has PID in FGD and TPC and no Michel e. These are mostly forward-going so require a pion kinematics cut
//
//  Essentially, cuts are:
//                          1 negative muon
//                          1 positive pion
//                          Any number of nucleons
//                          No other particles in the final state
//
bool SignalDef::isCC1pip_T2K_CH(FitEvent *event, double EnuMin, double EnuMax, bool MichelElectron) {
// ******************************************************

  if ((event->PartInfo(0))->fPID != 14) return false; 

  if (((event->PartInfo(0))->fP.E() < EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > EnuMax*1000.)) return false; 

  if (((event->PartInfo(2))->fPID != 13) && ((event->PartInfo(3))->fPID != 13)) return false;

  int pipCnt = 0; // Counts number of positive pions
  int lepCnt = 0; // Counts number of muons

  TLorentzVector Pnu = (event->PartInfo(0))->fP;
  TLorentzVector Ppip;
  TLorentzVector Pmu;

  for (unsigned int j = 2; j < event->Npart(); j++) {

    if (!(event->PartInfo(j)->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; // Move on if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;

    // No additional mesons in the final state
    if ((abs(PID) >= 111 && abs(PID) <= 210) || (abs(PID) >= 212 && abs(PID) <= 557) || PID == -211) return false; 
    // Count leptons
    else if (PID == 13) {
      lepCnt++;
      Pmu = (event->PartInfo(j))->fP;
    // Count the pi+
    } else if (PID == 211) {
      pipCnt++;
      Ppip = (event->PartInfo(j))->fP;
    }
  }

  // Make the cuts on the final state particles
  if (pipCnt != 1) return false; 
  if (lepCnt != 1) return false;

  // Done with relatively generic CC1pi+ definition

  // If this event passes the criteria on particle counting, enforce the T2K ND280 phase space constraints
  // Will be different if Michel tag sample is included or not
  // Essentially, if there's a Michel tag we don't cut on the pion variables

  double p_mu = FitUtils::p(Pmu)*1000;
  double p_pi = FitUtils::p(Ppip)*1000;
  double cos_th_mu = cos(FitUtils::th(Pnu, Pmu));
  double cos_th_pi = cos(FitUtils::th(Pnu, Ppip));

  // If we're using Michel e- requirement we only care about the muon restricted phase space and use full pion phase space
  if (MichelElectron) {

    // Make the cuts on the muon variables
    if (p_mu <= 200 || cos_th_mu <= 0.2) {
      return false;
    } else {
      return true;
    }

 // If we aren't using Michel e- (i.e. we use directional information from pion) we need to impose the phase space cuts on the muon AND the pion)
  } else {

    // Make the cuts on muon and pion variables
    if (p_mu <= 200 || p_pi <= 200 || cos_th_mu <= 0.2 || cos_th_pi <= 0.2) {
      return false;
    } else {
      return true;
    }
  }

  // Default to false; should never fire
  return false;
};

//********************************************************************
bool SignalDef::isCCQEnumu_MINERvA(FitEvent* event, double EnuMin,
				   double EnuMax, bool fullphasespace){
//********************************************************************

  // For now, define as the true mode being CCQE or npnh
  if (event->Mode != 1 and event->Mode != 2) return false;

  // Only look at numu events
  if ((event->PartInfo(0))->fPID != 14) return false;

  // Get Theta Variables
  double ThetaMu = 999.9;
  double Enu_rec = -1.0;

  for (UInt_t i = 2; i < event->Npart(); i++){
    if (event->PartInfo(i)->fPID == 13){

      ThetaMu = (event->PartInfo(0)->fP.Vect().Angle(event->PartInfo(i)->fP.Vect()));
      Enu_rec = FitUtils::EnuQErec((event->PartInfo(i))->fP, cos(ThetaMu), 34.,true);
      break;
    }
  }

  // If Restricted phase space
  if (!fullphasespace &&  ThetaMu > 0.34906585) return false;

  // restrict energy range
  if (event->Enu()/1000.0 < EnuMin || event->Enu()/1000.0 > EnuMax) return false;
  if (Enu_rec < EnuMin || Enu_rec > EnuMax) return false;

  return true;
};

//********************************************************************
bool SignalDef::isCCQEnumubar_MINERvA (FitEvent* event, double EnuMin,
				       double EnuMax, bool fullphasespace){
//********************************************************************

  // For now, define as the true mode being CCQE or npnh
  if (event->Mode != -1 and event->Mode != -2) return false;

  // Only look at numu events
  if ((event->PartInfo(0))->fPID != -14) return false;

  // Get Theta Variables
  double ThetaMu = 999.9;
  double Enu_rec = -1.0;

  for (UInt_t i = 2; i < event->Npart(); i++){
    if (event->PartInfo(i)->fPID == -13){

      ThetaMu = (event->PartInfo(0)->fP.Vect().Angle(event->PartInfo(i)->fP.Vect()));
      Enu_rec = FitUtils::EnuQErec((event->PartInfo(i))->fP, cos(ThetaMu), 30.,false);
      break;
    }
  }

  // If Restricted phase space
  if (!fullphasespace &&  ThetaMu > 0.34906585) return false;

  // restrict energy range
  if (event->Enu()/1000.0 < EnuMin || event->Enu()/1000.0 > EnuMax) return false;
  if (Enu_rec < EnuMin || Enu_rec > EnuMax) return false;

  return true;
}

//********************************************************************
bool SignalDef::isCCincLowRecoil_MINERvA(FitEvent *event, double EnuMin,
					 double EnuMax, bool hadroncut){
//********************************************************************

  // Only look at numu events
  if ((event->PartInfo(0))->fPID != 14) return false;

  // Restrict true energy range
  if (event->Enu()/1000.0 < EnuMin || event->Enu()/1000.0 > EnuMax) return false;

  // Loop Particles
  int nhadrons = 0;
  int nmuons = 0;
  double ThetaMu = 0.0;
  double Emu = 0.0;

  for (UInt_t i = 2; i < event->Npart(); i++){

    if (!(event->PartInfo(i))->fIsAlive) continue;
    if (event->PartInfo(i)->fStatus != 0) continue;

    int PID = event->PartInfo(i)->fPID;
    if (PID == 13){
      nmuons++;
      ThetaMu = event->PartInfo(i)->fP.Vect().Angle(event->PartInfo(0)->fP.Vect());
      Emu = event->PartInfo(i)->fP.E()/1000.0;
    } else if (PID != 2112 and PID < 999 and PID != 22 and abs(PID) != 14){
      nhadrons++;
    }
  }

  // Need at least one muon
  if (nmuons < 1) return false;

  // Require Eav > 0.0
  if (hadroncut and nhadrons < 1) return false;

  // Cut on muon angle greated than 20deg
  if (cos(ThetaMu) < 0.93969262078) return false;

  // Cut on muon energy < 1.5 GeV
  if (Emu < 1.5) return false;

  return true;
}

bool SignalDef::isT2K_CC0pi(FitEvent* event, double EnuMin,
			    double EnuMax, bool forwardgoing){

  // Only Numu
  if (!event->PartInfo(0)->fPID == 14) return false;

  // Cut on Energy
  if (event->Enu()/1000.0 < EnuMin ||
      event->Enu()/1000.0 > EnuMax) return false;

  // Particle Checks
  bool only_allowed_particles = true;
  bool muon_found = false;
  double CosThetaMu = -9999.9;

  // Loop over all particles
  for (UInt_t j = 2; j < event->Npart(); ++j){

    // Get only final state
    if (!(event->PartInfo(j))->fIsAlive or (event->PartInfo(j))->fStatus != 0) continue;

    // Get PDG
    int particle_pdg = (event->PartInfo(j))->fPID;
    
    // Muon section
    if (particle_pdg == 13){
      CosThetaMu = cos(((event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect())));
      muon_found = true;
    } else if (particle_pdg != 22 &&
	       particle_pdg != 2212 &&
	       particle_pdg != 2112 &&
	       particle_pdg != 13){
      only_allowed_particles = false;
    }
  }

  // CC0PI Cut
  if (!only_allowed_particles or !muon_found) return false;
  
  // restricted phase space
  if (forwardgoing and
      CosThetaMu < 0.0 and
      CosThetaMu != -9999.9)
    return false;
}
