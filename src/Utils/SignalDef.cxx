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

#include "FitUtils.h"

#include "SignalDef.h"


bool SignalDef::isCCINC(FitEvent *event, int nuPDG, double EnuMin, double EnuMax) {

  // Check for the desired PDG code
  if (!event->HasISParticle(nuPDG)) return false;

  // Check that it's within the allowed range if set
  if (EnuMin != EnuMax)
    if (!SignalDef::IsEnuInRange(event, EnuMin*1000, EnuMax*1000)) 
      return false;

  // Check that the charged lepton we expect has been produced
  if (!event->HasFSParticle(nuPDG > 0 ? nuPDG-1 : nuPDG+1)) return false;

  return true;
}

bool SignalDef::isNCINC(FitEvent *event, int nuPDG, double EnuMin, double EnuMax) {

  // Check for the desired PDG code before and after the interaction
  if (!event->HasISParticle(nuPDG) ||
      !event->HasFSParticle(nuPDG)) return false;

  // Check that it's within the allowed range if set
  if (EnuMin != EnuMax)
    if (!SignalDef::IsEnuInRange(event, EnuMin*1000, EnuMax*1000))
      return false;

  return true;
}


bool SignalDef::isCC0pi(FitEvent *event, int nuPDG, double EnuMin, double EnuMax){

  // Check it's CCINC
  if (!SignalDef::isCCINC(event, nuPDG, EnuMin, EnuMax)) return false;

  // Veto event with mesons
  if (event->NumFSMesons() != 0) return false;
  
  // Veto events which don't have exactly 1 outgoing charged lepton
  if (event->NumFSLeptons() != 1) return false;

  return true;
}

bool SignalDef::isCCQE(FitEvent *event, int nuPDG, double EnuMin, double EnuMax){

  // Check if it's CCINC
  if (!SignalDef::isCCINC(event, nuPDG, EnuMin, EnuMax)) return false;
  
  // Require modes 1 (CCQE)
  if (abs(event->Mode) != 1) return false;

  return true;
}

bool SignalDef::isCCQELike(FitEvent *event, int nuPDG, double EnuMin, double EnuMax){

  // Check if it's CCINC
  if (!SignalDef::isCCINC(event, nuPDG, EnuMin, EnuMax)) return false;

  // Require modes 1/2 (CCQE and MEC)                                                                                                                                         
  if (abs(event->Mode) != 1 && abs(event->Mode) != 2) return false;

  return true;
}

// Require one meson, one charged lepton. types specified in the arguments
bool SignalDef::isCC1pi(FitEvent *event, int nuPDG, int piPDG, 
			double EnuMin, double EnuMax){
  
  // First, make sure it's CCINC
  if (!SignalDef::isCCINC(event, nuPDG, EnuMin, EnuMax)) return false;

  int nMesons  = event->NumFSMesons();
  int nLeptons = event->NumFSLeptons();
  int nPion    = event->NumFSParticle(piPDG);

  // Check that the desired pion exists and is the only meson
  if (nPion != 1 || nMesons != 1) return false;

  // Check that there is only one final state lepton
  if (nLeptons != 1) return false;

  return true;
}

// Require one meson, one neutrino. Types specified as arguments
bool SignalDef::isNC1pi(FitEvent *event, int nuPDG, int piPDG,
                        double EnuMin, double EnuMax){

  // First, make sure it's NCINC
  if (!SignalDef::isNCINC(event, nuPDG, EnuMin, EnuMax)) return false;

  int nMesons  = event->NumFSMesons();
  int nLeptons = event->NumFSLeptons();
  int nPion    = event->NumFSParticle(piPDG);

  // Check that the desired pion exists and is the only meson
  if (nPion != 1 && nMesons != 1) return false;

  // Check that there are no charged leptons
  if (nLeptons != 0) return false;

  return true;
}

// A slightly ugly function to replace the BC 2pi channels.
// All particles which are allowed in the final state are specified
bool SignalDef::isCCWithFS(FitEvent *event, int nuPDG, std::vector<int> pdgs,
			   double EnuMin, double EnuMax){

  // Check it's CCINC
  if (!SignalDef::isCCINC(event, nuPDG, EnuMin, EnuMax)) return false;
  
  // Remove events where the number of final state particles
  // do not match the number specified in the signal definition
  if ((int)pdgs.size() != event->NumFSParticle()) return false;

  // For every particle in the list, check the number in the FS
  for (std::vector<int>::iterator it = pdgs.begin(); 
       it != pdgs.end(); ++it){
    // Check how many times this pdg is in the vector
    int nEntries = std::count (pdgs.begin(), pdgs.end(), *it);
    if (event->NumFSParticle(*it) != nEntries) 
      return false;
  }
  return true;
}

// Require one meson, one charged lepton, AND specify the only other final state particle
// This is only suitable for bubble chambers. Types specified in the arguments
bool SignalDef::isCC1pi3Prong(FitEvent *event, int nuPDG, int piPDG,
			      int thirdPDG, double EnuMin, double EnuMax){

  // First, make sure it's CC1pi
  if (!SignalDef::isCC1pi(event, nuPDG, piPDG, EnuMin, EnuMax)) return false;

  // Check we have the third prong
  if (event->NumFSParticle(thirdPDG) == 0) return false;

  // Check that there are only three FS particles
  if (event->NumFSParticle() != 3) return false;
  
  return true;
}

// Require one meson, one neutrino, AND specify the only other final state particle
// This is only suitable for bubble chambers. Types specified in the arguments
bool SignalDef::isNC1pi3Prong(FitEvent *event, int nuPDG, int piPDG,
                              int thirdPDG, double EnuMin, double EnuMax){

  // First, make sure it's NC1pi
  if (!SignalDef::isNC1pi(event, nuPDG, piPDG, EnuMin, EnuMax)) return false;

  // Check we have the third prong
  if (event->NumFSParticle(thirdPDG) == 0) return false;

  // Check that there are only three FS particles
  if (event->NumFSParticle() != 3) return false;

  return true;
}


bool SignalDef::isCCCOH(FitEvent *event, int nuPDG, int piPDG, double EnuMin, double EnuMax){
  
  // Check this is a CCINC event
  if (!SignalDef::isCCINC(event, nuPDG, EnuMin, EnuMax)) return false;

  int nLepton = event->NumFSParticle(nuPDG > 0 ? nuPDG-1 : nuPDG+1);
  int nPion   = event->NumFSParticle(piPDG);
  int nFS     = event->NumFSParticle();
  
  if (nLepton != 1 || nPion != 1) return false;
  if (nFS != 2) return false;
  return true;
}

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
// MOVE MINERvA
bool SignalDef::isCC1pip_MINERvA(FitEvent *event, double EnuMin, double EnuMax,
                                 bool isRestricted) {
  // *********************************

  // Signal is both pi+ and pi-
  // WARNING: PI- CONTAMINATION IS FULLY GENIE BECAUSE THE MICHEL TAG 
  // First, make sure it's CCINC
  if (!SignalDef::isCCINC(event, 14, EnuMin, EnuMax)) return false;
  
  // Allow pi+/pi-
  int piPDG[]  = {211, -211};
  int nLeptons = event->NumFSLeptons();
  int nPion    = event->NumFSParticle(GeneralUtils::makeVector(piPDG));
  
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
// MOVE MINERvA
bool SignalDef::isCCNpip_MINERvA(FitEvent *event, int &nPions, double EnuMin,
                                 double EnuMax, bool isRestricted) {
  // *********************************

  // First, make sure it's CCINC
  if (!SignalDef::isCCINC(event, 14, EnuMin, EnuMax)) return false;

  int nLeptons = event->NumFSLeptons();

  // Write the number of pions to the measurement class...
  // Maybe better to just do that inside the class?
  int pdgs[] = {-211, 211};
  nPions     = event->NumFSParticle(GeneralUtils::makeVector(pdgs));

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

// MOVE T2K
// T2K H2O signal definition
bool SignalDef::isCC1pip_T2K_H2O(FitEvent *event, double EnuMin,
                                 double EnuMax) {

  if (!SignalDef::isCC1pi(event, 14, 211, EnuMin, EnuMax)) return false;

  TLorentzVector Pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector Pmu = event->GetHMISParticle(13)->fP;
  TLorentzVector Ppip = event->GetHMISParticle(211)->fP;

  double p_mu = FitUtils::p(Pmu) * 1000;
  double p_pi = FitUtils::p(Ppip) * 1000;
  double cos_th_mu = cos(FitUtils::th(Pnu, Pmu));
  double cos_th_pi = cos(FitUtils::th(Pnu, Ppip));

  if (p_mu <= 200 || p_pi <= 200 || cos_th_mu <= 0.3 || cos_th_pi <= 0.3)
    return false;

  return true;
};

// ******************************************************
// T2K CC1pi+ CH analysis (Raquel's thesis)
// Has different phase space cuts depending on if using Michel tag or not
//
// Essentially consists of two samples: one sample which has Michel e (which we
// can't get pion direction from); this covers backwards angles quite well.
// Measurements including this sample does not have include pion kinematics cuts
//                                      one sample which has PID in FGD and TPC
//                                      and no Michel e. These are mostly
//                                      forward-going so require a pion
//                                      kinematics cut
//
//  Essentially, cuts are:
//                          1 negative muon
//                          1 positive pion
//                          Any number of nucleons
//                          No other particles in the final state
//
// MOVE T2K
bool SignalDef::isCC1pip_T2K_CH(FitEvent *event, double EnuMin, double EnuMax,
                                bool MichelElectron) {
  // ******************************************************

  if (!SignalDef::isCC1pi(event, 14, 211, EnuMin, EnuMax)) return false;

  TLorentzVector Pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;

  // If this event passes the criteria on particle counting, enforce the T2K
  // ND280 phase space constraints
  // Will be different if Michel tag sample is included or not
  // Essentially, if there's a Michel tag we don't cut on the pion variables

  double p_mu = FitUtils::p(Pmu) * 1000;
  double p_pi = FitUtils::p(Ppip) * 1000;
  double cos_th_mu = cos(FitUtils::th(Pnu, Pmu));
  double cos_th_pi = cos(FitUtils::th(Pnu, Ppip));

  // If we're using Michel e- requirement we only care about the muon restricted
  // phase space and use full pion phase space
  if (MichelElectron) {
    // Make the cuts on the muon variables
    if (p_mu <= 200 || cos_th_mu <= 0.2) {
      return false;
    } else {
      return true;
    }

    // If we aren't using Michel e- (i.e. we use directional information from
    // pion) we need to impose the phase space cuts on the muon AND the pion)
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

// MOVE MINERvA
//********************************************************************
bool SignalDef::isCCQEnumu_MINERvA(FitEvent *event, double EnuMin,
                                   double EnuMax, bool fullphasespace) {
  //********************************************************************

  if (!SignalDef::isCCQELike(event, 14, EnuMin, EnuMax)) return false;

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

// MOVE MINERvA
//********************************************************************
bool SignalDef::isCCQEnumubar_MINERvA(FitEvent *event, double EnuMin,
                                      double EnuMax, bool fullphasespace) {
  //********************************************************************

  if (!SignalDef::isCCQELike(event, -14, EnuMin, EnuMax)) return false;

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

// MOVE MINERVA
//********************************************************************
bool SignalDef::isCCincLowRecoil_MINERvA(FitEvent *event, double EnuMin,
                                         double EnuMax) {
  //********************************************************************

  if (!SignalDef::isCCINC(event, 14, EnuMin, EnuMax)) return false;

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

// MOVE T2K
bool SignalDef::isT2K_CC0pi(FitEvent *event, double EnuMin, double EnuMax,
                            bool forwardgoing) {

  // Require a numu CC0pi event
  if (!SignalDef::isCC0pi(event, 14, EnuMin, EnuMax)) return false;

  TLorentzVector pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;

  double CosThetaMu = pnu.Vect().Angle(pmu.Vect());

  // restricted phase space
  if (forwardgoing and CosThetaMu < 0.0) return false;
  return true;
}


// MOVE T2K
bool SignalDef::isT2K_CC0pi_STV(FitEvent *event, double EnuMin, double EnuMax) {

  // Require a numu CC0pi event
  if (!SignalDef::isCC0pi(event, 14, EnuMin, EnuMax)) return false;
  
  // Require at least one FS proton
  if (event->NumFSParticle(2212) == 0) return false;

  TLorentzVector pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector pp  = event->GetHMFSParticle(2212)->fP;
  
  // mu phase space
  if ((pmu.Vect().Mag() < 250) || (pnu.Vect().Angle(pmu.Vect()) < -0.6))
    return false;

  // p phase space
  if ((pp.Vect().Mag() < 250) || (pp.Vect().Mag() > 1E3) ||
      (pnu.Vect().Angle(pp.Vect()) < 0.4)) {
    return false;
  }
  return true;
}

// MOVE MINERVA
bool SignalDef::isCC0pi1p_MINERvA(FitEvent* event, double enumin, double enumax){
  
  // Require numu CC0pi event with a proton above threshold
  bool signal = (SignalDef::isCC0pi(event, 14, enumin, enumax) &&
		 SignalDef::HasProtonKEAboveThreshold(event, 110.0));
 
  return signal;
}


bool SignalDef::HasProtonKEAboveThreshold(FitEvent* event, double threshold){
  
  double pe = -1.0;
  if (event->HasFSProton()) pe = FitUtils::T(event->GetHMFSProton()->fP);

  return pe > threshold / 1000.0;

}

// Calculate the angle between the neutrino and an outgoing particle, apply a cut
bool SignalDef::IsRestrictedAngle(FitEvent* event, int nuPDG, int otherPDG, double angle){
  
  // If the particles don't exist, return false
  if (!event->HasISParticle(nuPDG) || !event->HasFSParticle(otherPDG)) return false;

  // Get Mom
  TVector3 pnu = event->GetHMISParticle(nuPDG)->fP.Vect();
  TVector3 p2  = event->GetHMFSParticle(otherPDG)->fP.Vect();

  double theta = pnu.Angle(p2) * 180. / TMath::Pi();

  return (theta < angle);
}

bool SignalDef::IsEnuInRange(FitEvent* event, double emin, double emax){
  return (event->PartInfo(0)->fP.E() > emin && 
	  event->PartInfo(0)->fP.E() < emax);
}
