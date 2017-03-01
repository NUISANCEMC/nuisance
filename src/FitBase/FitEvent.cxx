// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is pddrt of NUISANCE.
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

void FitEvent::ResetParticleList() {
  for (unsigned int i = 0; i < kMaxParticles; i++) {
    FitParticle* fp = fParticleList[i];
    if (fp) delete fp;
    fParticleList[i] = NULL;
  }
}

void FitEvent::HardReset() {
  for (unsigned int i = 0; i < kMaxParticles; i++) {
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
    // FitParticle* fp = fParticleList[i];
    // std::cout << "Fit Particle = " << fp << std::endl;
    // if (fp) delete fp;
    fParticleList[i] = NULL;

    fParticlePDG[i] = 0;
    fParticleState[i] = kUndefinedState;
    fParticleMom[i][0] = 0.0;
    fParticleMom[i][1] = 0.0;
    fParticleMom[i][2] = 0.0;
    fParticleMom[i][3] = 0.0;

    fOrigParticlePDG[i] = 0;
    fOrigParticleState[i] = kUndefinedState;
    fOrigParticleMom[i][0] = 0.0;
    fOrigParticleMom[i][1] = 0.0;
    fOrigParticleMom[i][2] = 0.0;
    fOrigParticleMom[i][3] = 0.0;
  }

}


//***************************************************
void FitEvent::OrderStack() {
  //***************************************************

  // Copy current stack
  int npart = fNParticles;

  for (int i = 0; i < npart; i++) {
    fOrigParticlePDG[i]    = fParticlePDG[i];
    fOrigParticleState[i]  = fParticleState[i];
    fOrigParticleMom[i][0] = fParticleMom[i][0];
    fOrigParticleMom[i][1] = fParticleMom[i][1];
    fOrigParticleMom[i][2] = fParticleMom[i][2];
    fOrigParticleMom[i][3] = fParticleMom[i][3];
  }

  // Now run loops for each particle
  fNParticles = 0;
  int stateorder[6] = {kInitialState,   kFinalState,     kFSIState,
                       kNuclearInitial, kNuclearRemnant, kUndefinedState
                      };

  for (int s = 0; s < 6; s++) {
    for (int i = 0; i < npart; i++) {
      if (fOrigParticleState[i] != stateorder[s]) continue;

      fParticlePDG[fNParticles]    = fOrigParticlePDG[i];
      fParticleState[fNParticles]  = fOrigParticleState[i];
      fParticleMom[fNParticles][0] = fOrigParticleMom[i][0];
      fParticleMom[fNParticles][1] = fOrigParticleMom[i][1];
      fParticleMom[fNParticles][2] = fOrigParticleMom[i][2];
      fParticleMom[fNParticles][3] = fOrigParticleMom[i][3];

      fNParticles++;
    }
  }

  if (LOG_LEVEL(DEB)) {
    LOG(DEB) << "Ordered stack" << std::endl;
    for (int i = 0; i < fNParticles; i++) {
      LOG(DEB) << "Particle " << i << ". " << fParticlePDG[i] << " "
               << fParticleMom[i][0] << " " << fParticleMom[i][1] << " "
               << fParticleMom[i][2] << " " << fParticleMom[i][3] << " "
               << fParticleState[i] << std::endl;
    }
  }

  if (fNParticles != npart) {
    ERR(FTL) << "Dropped some particles when ordering the stack!" << std::endl;
  }

  return;
}

/* Read/Write own event class */
void FitEvent::SetBranchAddress(TChain* tn) {
  fType = kINPUTFITEVENT;

  std::cout << "Setting Branch Address " << std::endl;
  sleep(1);
  tn->SetBranchAddress("Mode", &fMode);
  tn->SetBranchAddress("Mode", &Mode);
  tn->SetBranchAddress("EventNo", &fEventNo);
  tn->SetBranchAddress("TotCrs", &fTotCrs);
  tn->SetBranchAddress("TargetA", &fTargetA);
  tn->SetBranchAddress("TargetH", &fTargetH);
  tn->SetBranchAddress("Bound", &fBound);

  tn->SetBranchAddress("InputWeight", &InputWeight);

  tn->SetBranchAddress("NParticles", &fNParticles);

  // Save original particle stack/unordered
  tn->SetBranchAddress("ParticleState", fParticleState);
  tn->SetBranchAddress("ParticlePDG", fParticlePDG);
  tn->SetBranchAddress("ParticleMom", fParticleMom);

  // Shouldn't be a need to read back in Generator Info...
  // fGenInfo->SetBranchAddress(tn);
}

void FitEvent::AddBranchesToTree(TTree* tn) {
  tn->Branch("Mode", &Mode, "Mode/I");

  tn->Branch("EventNo", &fEventNo, "EventNo/i");
  tn->Branch("TotCrs", &fTotCrs, "TotCrs/D");
  tn->Branch("TargetA", &fTargetA, "TargetA/I");
  tn->Branch("TargetH", &fTargetH, "TargetH/I");
  tn->Branch("Bound", &fBound, "Bound/O");

  tn->Branch("InputWeight", &InputWeight, "InputWeight/D");

  tn->Branch("NParticles", &fNParticles, "NParticles/I");
  // Load original particle stack into norm stack, before ordering.
  tn->Branch("ParticleState", fOrigParticleState, "ParticleState[NParticles]/i");
  tn->Branch("ParticlePDG", fOrigParticlePDG, "ParticlePDG[NParticles]/I");
  tn->Branch("ParticleMom", fOrigParticleMom, "ParticleMom[NParticles][4]/D");

  // Save Extra Generator Information
  // if (fGenInfo) fGenInfo->AddBranchesToTree(tn);
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

  if (LOG_LEVEL(EVT)or true) {
    LOG() << "EVTEvent print" << std::endl;
    LOG() << "Mode: " << fMode << std::endl;
    LOG() << "Particles: " << fNParticles << std::endl;
    LOG() << " -> Particle Stack " << std::endl;
    for (int i = 0; i < fNParticles; i++) {
      LOG() << " -> -> " << i << ". " << fParticlePDG[i] << " "
            << fParticleState[i] << " "
            << "  Mom(" << fParticleMom[i][0] << ", " << fParticleMom[i][1]
            << ", " << fParticleMom[i][2] << ", " << fParticleMom[i][3] << ")."
            << std::endl;
    }
  }
  return;
}
