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
#include "FitEvent.h"
#include <iostream>
#include "TObjArray.h"

#ifdef __GENIE_ENABLED__
#include "Conventions/Units.h"
#endif

//***************************************************
// Refill all the particle vectors etc for the event
void FitEvent::CalcKinematics() {
//***************************************************
ResetParticleList();

#ifdef __NUANCE_ENABLED__
  if (fType == kNUANCE) NuanceKinematics();
#endif

  if (LOG_LEVEL(FIT)) Print();

  return;
};

void FitEvent::ResetParticleList(){
  for (unsigned int i = 0; i < kMaxParticles; i++) {
    FitParticle* fp = fParticleList[i];
    if (fp) delete fp;
    fParticleList[i] = NULL;
  }
}

//***************************************************
void FitEvent::ResetEvent() {
  //***************************************************

  // Sort Event Info
  fMode = 9999;
  Mode = 9999;
  fEventNo = -1;
  fTotCrs = -1.0;
  fTargetA = -1;
  fTargetZ = -1;
  fTargetH = -1;
  fBound = false;
  fNParticles = 0;

  for (unsigned int i = 0; i < kMaxParticles; i++) {
    FitParticle* fp = fParticleList[i];
    if (fp) delete fp;
    fParticleList[i] = NULL;
  }
}


//***************************************************
void FitEvent::OrderStack() {
  //***************************************************

  // Copy current stack
  int oldpartpdg[kMaxParticles];
  int oldpartstate[kMaxParticles];
  double oldpartmom[kMaxParticles][4];
  int npart = fNParticles;

  for (int i = 0; i < npart; i++) {
    oldpartpdg[i] = fParticlePDG[i];
    oldpartstate[i] = fParticleState[i];
    oldpartmom[i][0] = fParticleMom[i][0];
    oldpartmom[i][1] = fParticleMom[i][1];
    oldpartmom[i][2] = fParticleMom[i][2];
    oldpartmom[i][3] = fParticleMom[i][3];
  }

  // Now run loops for each particle
  fNParticles = 0;
  int stateorder[6] = {kInitialState,   kFinalState,     kFSIState,
                       kNuclearInitial, kNuclearRemnant, kUndefinedState};

  for (int s = 0; s < 6; s++) {
    for (int i = 0; i < npart; i++) {
      if (oldpartstate[i] != stateorder[s]) continue;

      fParticlePDG[fNParticles] = oldpartpdg[i];
      fParticleState[fNParticles] = oldpartstate[i];
      fParticleMom[fNParticles][0] = oldpartmom[i][0];
      fParticleMom[fNParticles][1] = oldpartmom[i][1];
      fParticleMom[fNParticles][2] = oldpartmom[i][2];
      fParticleMom[fNParticles][3] = oldpartmom[i][3];

      fNParticles++;
    }
  }

  LOG(DEB) << "Ordered stack" << std::endl;
  for (int i = 0; i < fNParticles; i++) {
    LOG(DEB) << "Particle " << i << ". " << fParticlePDG[i] << " "
             << fParticleMom[i][0] << " " << fParticleMom[i][1] << " "
             << fParticleMom[i][2] << " " << fParticleMom[i][3] << " "
             << fParticleState[i] << std::endl;
  }

  if (fNParticles != npart) {
    ERR(FTL) << "Dropped some particles when ordering the stack!" << std::endl;
  }

  return;
}

// REQUIRED FUNCTIONS FOR NUANCE
#ifdef __NUANCE_ENABLED__
//***************************************************
void FitEvent::SetEventAddress(NuanceEvent** tempevent) {
  //***************************************************
  fType = kNUANCE;
  nuance_event = *tempevent;
}

//***************************************************
void FitEvent::NuanceKinematics() {
  //***************************************************
  ResetEvent();

  fMode = GeneratorUtils::ConvertNuanceMode(nuance_event);
  Mode = fMode;
  fEventNo = 0.0;
  fTotCrs = 1.0;
  fTargetA = 0.0;
  fTargetZ = 0.0;
  fTargetH = 0;
  fBound = 0.0;

  // Fill particle Stack
  fNParticles = 0;

  // Check Particle Stack
  UInt_t npart = 2 + nuance_event->n_leptons + nuance_event->n_hadrons;

  if (npart > kMaxParticles) {
    ERR(FTL) << "NUANCE has too many particles" << std::endl;
    ERR(FTL) << "npart=" << npart << " kMax=" << kMaxParticles << std::endl;
    throw;
  }

  // Fill Neutrino
  fParticleState[0] = kInitialState;
  fParticleMom[0][0] = nuance_event->p_neutrino[0];
  fParticleMom[0][1] = nuance_event->p_neutrino[1];
  fParticleMom[0][2] = nuance_event->p_neutrino[2];
  fParticleMom[0][3] = nuance_event->p_neutrino[3];
  fParticlePDG[0] = nuance_event->neutrino;

  // Fill Target Nucleon
  fParticleState[1] = kInitialState;
  fParticleMom[1][0] = nuance_event->p_targ[0];
  fParticleMom[1][1] = nuance_event->p_targ[1];
  fParticleMom[1][2] = nuance_event->p_targ[2];
  fParticleMom[1][3] = nuance_event->p_targ[3];
  fParticlePDG[1] = nuance_event->target;
  fNParticles = 2;

  // Fill Outgoing Leptons
  for (int i = 0; i < nuance_event->n_leptons; i++) {
    fParticleState[fNParticles] = kFinalState;
    fParticleMom[fNParticles][0] = nuance_event->p_lepton[i][0];
    fParticleMom[fNParticles][1] = nuance_event->p_lepton[i][1];
    fParticleMom[fNParticles][2] = nuance_event->p_lepton[i][2];
    fParticleMom[fNParticles][3] = nuance_event->p_lepton[i][3];
    fParticlePDG[fNParticles] = nuance_event->lepton[i];
    fNParticles++;
  }

  // Fill Outgoing Hadrons
  for (int i = 0; i < nuance_event->n_hadrons; i++) {
    fParticleState[fNParticles] = kFinalState;
    fParticleMom[fNParticles][0] = nuance_event->p_hadron[i][0];
    fParticleMom[fNParticles][1] = nuance_event->p_hadron[i][1];
    fParticleMom[fNParticles][2] = nuance_event->p_hadron[i][2];
    fParticleMom[fNParticles][3] = nuance_event->p_hadron[i][3];
    fParticlePDG[fNParticles] = nuance_event->hadron[i];
    fNParticles++;
  }
}
#endif

/* Read/Write own event class */
void FitEvent::SetBranchAddress(TChain* tn) {
  fType = kINPUTFITEVENT;

  tn->SetBranchAddress("Mode", &fMode);
  tn->SetBranchAddress("Mode", &Mode);
  tn->SetBranchAddress("EventNo", &fEventNo);
  tn->SetBranchAddress("TotCrs", &fTotCrs);
  tn->SetBranchAddress("TargetA", &fTargetA);
  tn->SetBranchAddress("TargetH", &fTargetH);
  tn->SetBranchAddress("Bound", &fBound);

  tn->SetBranchAddress("InputWeight", &InputWeight);

  tn->SetBranchAddress("NParticles", &fNParticles);
  tn->SetBranchAddress("ParticleState", fParticleState);
  tn->SetBranchAddress("ParticlePDG", fParticlePDG);
  tn->SetBranchAddress("ParticleMom", fParticleMom);
}

void FitEvent::AddBranchesToTree(TTree* tn) {
  tn->Branch("Mode", &fMode, "Mode/I");
  
  tn->Branch("EventNo", &fEventNo, "EventNo/i");
  tn->Branch("TotCrs", &fTotCrs, "TotCrs/D");
  tn->Branch("TargetA", &fTargetA, "TargetA/I");
  tn->Branch("TargetH", &fTargetH, "TargetH/I");
  tn->Branch("Bound", &fBound, "Bound/O");

  tn->Branch("InputWeight", &InputWeight, "InputWeight/D");

  tn->Branch("NParticles", &fNParticles, "NParticles/I");
  tn->Branch("ParticleState", fParticleState, "ParticleState[NParticles]/i");
  tn->Branch("ParticlePDG", fParticlePDG, "ParticlePDG[NParticles]/I");
  tn->Branch("ParticleMom", fParticleMom, "ParticleMom[NParticles][4]/D");
  

  // tn->SetAlias("Enu", "ParticleMom[0][4]");
}

/* Event Access Functions */
//***************************************************

FitParticle* FitEvent::PartInfo(UInt_t i) {
  // Check Valid
  if (i > (UInt_t)fNParticles or i < 0) {
    ERR(FTL) << "Requesting particle beyond stack!" << std::endl;
    ERR(FTL) << "i = " << i << " N = " << fNParticles << std::endl;
    ERR(FTL) << "Mode = " << fMode << std::endl;

    throw;
  }

  // Check particle has been formed
  if (!fParticleList[i]) {
    fParticleList[i] = new FitParticle(fParticleMom[i][0], fParticleMom[i][1],
                                       fParticleMom[i][2], fParticleMom[i][3],
                                       fParticlePDG[i], fParticleState[i]);
  }

  return fParticleList[i];
}

int FitEvent::GetNeutrinoInPos(void) const {
  for (UInt_t i = 0; i < NPart(); i++) {
    if (fParticleState[i] != kInitialState) continue;

    if (abs(fParticlePDG[i]) == 12 || abs(fParticlePDG[i]) == 14 ||
        abs(fParticlePDG[i]) == 16) {
      return i;
    }
  }
  return -1;
}

int FitEvent::GetLeptonOutPos(void) const {
  for (UInt_t i = 0; i < NPart(); i++) {
    if (fParticleState[i] != kFinalState) continue;

    if (abs(fParticlePDG[i]) == 11 || abs(fParticlePDG[i]) == 13 ||
        abs(fParticlePDG[i]) == 15) {
      return i;
    }
  }
  return -1;
}

FitParticle* FitEvent::GetBeamPart(void) { return PartInfo(GetBeamPartPos()); }

FitParticle* FitEvent::GetNeutrinoIn(void) {
  // std::cout << "Neutrino In Pos " << std::endl;
  // Print();
  return PartInfo(GetNeutrinoInPos());
}

FitParticle* FitEvent::GetLeptonOut(void) {
  return PartInfo(GetLeptonOutPos());
}

bool FitEvent::HasParticle(int pdg, int state) {
  bool found = false;
  for (int i = 0; i < fNParticles; i++) {
    if (state != -1 && fParticleState[i] != (uint)state) continue;
    if (fParticlePDG[i] == pdg) found = true;
  }

  return found;
}

int FitEvent::NumParticle(int pdg, int state) {
  int nfound = 0;
  for (int i = 0; i < fNParticles; i++) {
    // std::cout << "fParticlePDG[" << i << "] = " << fParticlePDG[i] <<
    // std::endl;

    if (state != -1 and fParticleState[i] != (uint)state) continue;
    if (pdg == 0 or fParticlePDG[i] == pdg) nfound += 1;
  }

  return nfound;
}

int FitEvent::NumParticle(std::vector<int> pdg, int state) {
  int nfound = 0;
  for (int i = 0; i < fNParticles; i++) {
    if (state != -1 and fParticleState[i] != (uint)state) continue;
    if (std::find(pdg.begin(), pdg.end(), fParticlePDG[i]) != pdg.end())
      nfound += 1;
  }

  return nfound;
}

int FitEvent::NumFSLeptons() {
  int nLeptons = 0;

  for (int i = 0; i < fNParticles; i++) {
    if (fParticleState[i] != kFinalState) continue;
    if (abs(fParticlePDG[i]) == 11 || abs(fParticlePDG[i]) == 13 ||
        abs(fParticlePDG[i]) == 15)
      nLeptons += 1;
  }

  return nLeptons;
}

int FitEvent::NumFSMesons() {
  int nMesons = 0;

  for (int i = 0; i < fNParticles; i++) {
    if (fParticleState[i] != kFinalState) continue;
    if (abs(fParticlePDG[i]) >= 111 && abs(fParticlePDG[i]) <= 557)
      nMesons += 1;
  }

  return nMesons;
}

FitParticle* FitEvent::GetHMParticle(int pdg, int state) {
  double maxmom = -9999999.9;
  int maxind = -1;

  for (int i = 0; i < fNParticles; i++) {
    if (state != -1 and fParticleState[i] != (uint)state) continue;
    if (pdg == 0 or fParticlePDG[i] == pdg) {
      // Update Max Mom
      double mom = sqrt(fParticleMom[i][0] * fParticleMom[i][0] +
                        fParticleMom[i][1] * fParticleMom[i][1] +
                        fParticleMom[i][2] * fParticleMom[i][2]);
      if (fabs(mom) > maxmom) {
        maxmom = fabs(mom);
        maxind = i;
      }
    }
  }

  if (maxind == -1) {
    return NULL;
  }
  return PartInfo(maxind);
}

FitParticle* FitEvent::GetHMParticle(std::vector<int> pdg, int state) {
  double maxmom = -9999999.9;
  int maxind = -1;

  for (int i = 0; i < fNParticles; i++) {
    if (state != -1 and fParticleState[i] != (uint)state) continue;
    if (std::find(pdg.begin(), pdg.end(), fParticlePDG[i]) != pdg.end()) {
      // Update Max Mom
      double mom = sqrt(fParticleMom[i][0] * fParticleMom[i][0] +
                        fParticleMom[i][1] * fParticleMom[i][1] +
                        fParticleMom[i][2] * fParticleMom[i][2]);
      if (fabs(mom) > maxmom) {
        maxmom = fabs(mom);
        maxind = i;
      }
    }
  }

  if (maxind == -1) {
    return NULL;
  }
  return PartInfo(maxind);
}

void FitEvent::Print() {
  LOG(EVT) << "EVTEvent print" << std::endl;
  LOG(EVT) << "Mode: " << fMode << std::endl;
  LOG(EVT) << "Particles: " << fNParticles << std::endl;
  LOG(EVT) << " -> Particle Stack " << std::endl;
  for (int i = 0; i < fNParticles; i++) {
    LOG(EVT) << " -> -> " << i << ". " << fParticlePDG[i] << " "
             << fParticleState[i] << " "
             << "  Mom(" << fParticleMom[i][0] << ", " << fParticleMom[i][1]
             << ", " << fParticleMom[i][2] << ", " << fParticleMom[i][3] << ")."
             << std::endl;
  }
  return;
}
