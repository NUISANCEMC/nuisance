// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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
  if (EnuMin != EnuMax) {
    if (!SignalDef::IsEnuInRange(event, EnuMin*1000, EnuMax*1000)) {
      return false;
    }
  }

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

bool SignalDef::isNC0pi(FitEvent *event, int nuPDG, double EnuMin, double EnuMax){

  // Check it's NCINC
  if (!SignalDef::isNCINC(event, nuPDG, EnuMin, EnuMax)) return false;

  // Veto event with mesons
  if (event->NumFSMesons() != 0) return false;

  // Veto events with a charged lepton
  if (event->NumFSLeptons() != 0) return false;

  return true;
}


bool SignalDef::isCCQE(FitEvent *event, int nuPDG, double EnuMin, double EnuMax){

  // Check if it's CCINC
  if (!SignalDef::isCCINC(event, nuPDG, EnuMin, EnuMax)) return false;

  // Require modes 1 (CCQE)
  if (abs(event->Mode) != 1) return false;

  return true;
}

bool SignalDef::isNCEL(FitEvent *event, int nuPDG, double EnuMin, double EnuMax){

  // Check if it's NCINC
  if (!SignalDef::isNCINC(event, nuPDG, EnuMin, EnuMax)) return false;

  // Require modes 51/52 (NCEL)
  if (abs(event->Mode) != 51 && abs(event->Mode) != 52) return false;

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
  if (nPion != 1 || nMesons != 1) return false;

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
  for (std::vector<int>::iterator it = pdgs.begin(); it != pdgs.end(); ++it){
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
  //int nFS     = event->NumFSParticle();

  if (nLepton != 1 || nPion != 1) return false;
  // if (nFS != 2) return false;
  // GENIE v3 includes the nucleus in the final state, so modify the definition for now...
  if (abs(event->Mode) != 16) return false;
  return true;
}

bool SignalDef::isNCCOH(FitEvent *event, int nuPDG, int piPDG, double EnuMin, double EnuMax){

  // Check this is an NCINC event
  if (!SignalDef::isNCINC(event, nuPDG, EnuMin, EnuMax)) return false;

  int nLepton = event->NumFSParticle(nuPDG);
  int nPion   = event->NumFSParticle(piPDG);
  int nFS     = event->NumFSParticle();

  if (nLepton != 1 || nPion != 1) return false;
  if (nFS != 2) return false;
  return true;
}


bool SignalDef::HasProtonKEAboveThreshold(FitEvent* event, double threshold){

  for (uint i = 0; i < event->Npart(); i++){
    FitParticle* p = event->PartInfo(i);
    if (!p->IsFinalState()) continue;
    if (p->fPID != 2212) continue;

    if (FitUtils::T(p->fP) > threshold / 1000.0) return true;
  }
  return false;

}

bool SignalDef::HasProtonMomAboveThreshold(FitEvent* event, double threshold){

  for (uint i = 0; i < event->Npart(); i++){
    FitParticle* p = event->PartInfo(i);
    if (!p->IsFinalState()) continue;
    if (p->fPID != 2212) continue;

    if (p->fP.Vect().Mag() > threshold) return true;
  }
  return false;
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
  return (event->GetNeutrinoIn()->fP.E() > emin &&
	  event->GetNeutrinoIn()->fP.E() < emax);
}
