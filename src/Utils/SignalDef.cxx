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
  
bool SignalDef::isCC1pip_MiniBooNE(FitEvent *event, double EnuMin, double EnuMax) {

  if ((event->PartInfo(0))->fPID != 14) return false; 

  if (((event->PartInfo(0))->fP.E() < EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > EnuMax*1000.)) return false; 

  if (((event->PartInfo(2))->fPID != 13) && ((event->PartInfo(3))->fPID != 13)) return false;

  // MiniBooNE definition is:
  // 1 and only 1 muon
  // 1 and only 1 positive pion
  // any protons or neutrons (inc. 0)
  // any multi-nucleon states (inc. 0)
  // any number of photons

  int pipCnt = 0; // counts number of pions
  int lepCnt = 0;

  for (unsigned int j = 2; j < event->Npart(); j++) {
    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; //move on if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if ((abs(PID) >= 111 && abs(PID) <= 210) || (abs(PID) >= 212 && abs(PID) <= 557) || PID == -211) return false; 
    //else if (abs(PID) == 1114 || abs(PID) == 2114 || (abs(PID) >= 2214 && abs(PID) <= 5554)) return false; PHOTONS, NUCLEON, MULTINUCLEON OK
    else if (abs(PID) == 11 || abs(PID) == 13 || abs(PID) == 15 || abs(PID) == 17) lepCnt++;
    else if (abs(PID) == 211) pipCnt++;
    }

  if (pipCnt != 1) return false; //seems like any number of pions is OK; W mass cut ensures only one pion! i.e. should change this to pipCnt == 0 return false
  //maybe do pipCnt + lepCnt == 2? pi+ and mu- sometimes get confused
  if (lepCnt != 1) return false;

  return true;
};

bool SignalDef::isCC1pi0_MiniBooNE(FitEvent *event, double EnuMin, double EnuMax) {
  if ((event->PartInfo(0))->fPID != 14) return false; 

  if (((event->PartInfo(0))->fP.E() < EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > EnuMax*1000.)) return false; 

  if (((event->PartInfo(2))->fPID != 13) && ((event->PartInfo(3))->fPID != 13)) return false; 

// MiniBooNE:
// single mu-
// single pi0 
// both exiting nucleus
// any number of nucleon
// no additional mesons or leptons
// not corrected for nuclear  effects or intra-nuclear

  int pi0Cnt = 0;
  int lepCnt = 0;

  for (unsigned int j = 2; j < event->Npart(); j++) {
    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; //move to next particle if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (abs(PID) >= 113 && abs(PID) <= 557) return false;
    //else if (abs(PID) == 1114 || abs(PID) == 2114 || (abs(PID) >= 2214 && abs(PID) <= 5554)) return false; any number of NUCLEONS MULTINUCLEONS (PHOTONS?)
    else if (abs(PID) == 11 || abs(PID) == 13 || abs(PID) == 15 || abs(PID) == 17) lepCnt++;
    else if (PID == 111) pi0Cnt++;
    }

  if (pi0Cnt != 1) return false;
  if (lepCnt != 1) return false;

  return true;
};

bool SignalDef::isCC1pi0Bar_MINERvA(FitEvent *event, double EnuMin, double EnuMax) {

  if ((event->PartInfo(0))->fPID != -14) return false; 

  if (((event->PartInfo(0))->fP.E() < EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > EnuMax*1000.)) return false; 

  if (((event->PartInfo(2))->fPID != -13) && ((event->PartInfo(3))->fPID != -13)) return false; 

  // MINERvA measurement
  // single pi0
  // no pi+/0 escaping

  int pi0Cnt = 0;
  int lepCnt = 0;

  for (unsigned int j = 2; j < event->Npart(); j++) {
    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; //move to next particle if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (abs(PID) >= 113 && abs(PID) <= 557) {
      return false;
    } else if (abs(PID) == 11 || abs(PID) == 13 || abs(PID) == 15 || abs(PID) == 17) {
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

// MINERvA has unfolded and not unfolded muon phase space
bool SignalDef::isCC1pip_MINERvA(FitEvent *event, double EnuMin, double EnuMax, bool isRestricted) {

  if ((event->PartInfo(0))->fPID != 14) return false; 

  if (((event->PartInfo(0))->fP.E() < EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > EnuMax*1000.)) return false; 

  if (((event->PartInfo(2))->fPID != 13) && ((event->PartInfo(3))->fPID != 13)) return false;

// MINERvA is slightly different to MiniBooNE
// nu_mu interaction
// Exactly 1 charged pion exits (so + and - charge), however, has Michel electron requirement, so look for + only
// Needs 350 MeV pion maximum
// No restriction on neutral pions or other mesons

  int pipCnt = 0; // counts number of pions
  int lepCnt = 0;

  TLorentzVector pnu = (event->PartInfo(0))->fP;
  TLorentzVector pmu;
  TLorentzVector ppi;

  for (unsigned int j = 2; j < event->Npart(); j++) {

    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; //move on if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;

    if (abs(PID) == 11 || abs(PID) == 13 || abs(PID) == 15 || abs(PID) == 17) {
      lepCnt++;
      // if restricted we need the muon to find it's angle and if it's visible in MINOS
      if (isRestricted) {
        pmu = (event->PartInfo(j))->fP;
      }
    }
    else if (PID == 211) {
      ppi = event->PartInfo(j)->fP;
      pipCnt++; // technically also allows for a pi- to be ID as pi+ but there's Michel e criteria which sets this pretty much to zero
    }
  }

  // only one pion-like track
  if (pipCnt != 1) return false; 
  // only one lepton
  if (lepCnt != 1) return false;

  //if (FitUtils::T(ppi)*1000. > 350 || FitUtils::T(ppi)*1000. < 35) return false; // Need to have a 35 to 350 MeV pion kinetic energy requirement

  // MINERvA released another CC1pi+ xsec without muon unfolding!
  // here the muon angle is < 20 degrees (seen in MINOS)
  if (isRestricted) {
    double th_nu_mu = FitUtils::th(pmu, pnu) * 180./M_PI;
    if (th_nu_mu >= 20) return false;
  }

  return true;
};

bool SignalDef::isCCNpip_MINERvA(FitEvent *event, int &nPions, double EnuMin, double EnuMax, bool isRestricted) {

  if ((event->PartInfo(0))->fPID != 14) return false; 

  if (((event->PartInfo(0))->fP.E() < EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > EnuMax*1000.)) return false; 

  if (((event->PartInfo(2))->fPID != 13) && ((event->PartInfo(3))->fPID != 13)) return false;

// MINERvA signal definiton for Npi:
// nu_mu interaction, outgoing muon
// at least one charged pion exits (both - and + charge)
// however, has Michel electron requirement, so look for + only
// no restriction on neutral pions or other mesons

  int pipCnt = 0; // counts number of pions
  int lepCnt = 0;

  TLorentzVector pnu = (event->PartInfo(0))->fP;
  TLorentzVector pmu;

  for (unsigned int j = 2; j < event->Npart(); j++) {
    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; //move on if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (abs(PID) == 11 || abs(PID) == 13 || abs(PID) == 15 || abs(PID) == 17) {
      lepCnt++;
      if (isRestricted) pmu = (event->PartInfo(j))->fP;
    }
    else if (PID == 211) pipCnt++; // technically also allows for a pi- to be ID as pi+, but there's Michel electron criteria too which eliminates this
  }

  // any number of pions greater than 0!
  if (pipCnt == 0) return false; 
  // only one lepton
  if (lepCnt != 1) return false;

  nPions = pipCnt;

  // MINERvA released another CC1pi+ xsec without muon unfolding!
  // here the muon angle is < 20 degrees (seen in MINOS)
  if (isRestricted) {
    double th_nu_mu = FitUtils::th(pmu, pnu) * 180./M_PI;
    if (th_nu_mu >= 20) return false;
  }

  return true;
};

// T2K not unfolded phase space restrictions
bool SignalDef::isCC1pip_T2K(FitEvent *event, double EnuMin, double EnuMax) {
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

