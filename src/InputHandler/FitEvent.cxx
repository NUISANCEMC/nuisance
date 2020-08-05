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
#include "TObjArray.h"
#include <iostream>

FitEvent::FitEvent() {
  fGenInfo = NULL;
  kRemoveFSIParticles = true;
  kRemoveUndefParticles = true;

  AllocateParticleStack(400);
};

void FitEvent::AddGeneratorInfo(GeneratorInfoBase *gen) {
  fGenInfo = gen;
  gen->AllocateParticleStack(kMaxParticles);
}

void FitEvent::AllocateParticleStack(int stacksize) {
  NUIS_LOG(DEB, "Allocating particle stack of size: " << stacksize);
  kMaxParticles = stacksize;

  fParticleList = new FitParticle *[kMaxParticles];

  fParticleMom = new double *[kMaxParticles];
  fParticleState = new UInt_t[kMaxParticles];
  fParticlePDG = new int[kMaxParticles];
  fPrimaryVertex = new bool[kMaxParticles];

  fOrigParticleMom = new double *[kMaxParticles];
  fOrigParticleState = new UInt_t[kMaxParticles];
  fOrigParticlePDG = new int[kMaxParticles];
  fOrigPrimaryVertex = new bool[kMaxParticles];

  for (size_t i = 0; i < kMaxParticles; i++) {
    fParticleList[i] = NULL;
    fParticleMom[i] = new double[4];
    fOrigParticleMom[i] = new double[4];
  }

  if (fGenInfo)
    fGenInfo->AllocateParticleStack(kMaxParticles);
}

void FitEvent::ExpandParticleStack(int stacksize) {
  DeallocateParticleStack();
  AllocateParticleStack(stacksize);
}

void FitEvent::DeallocateParticleStack() {
  for (size_t i = 0; i < kMaxParticles; i++) {
    if (fParticleList[i])
      delete fParticleList[i];
    delete fParticleMom[i];
    delete fOrigParticleMom[i];
  }
  delete fParticleMom;
  delete fOrigParticleMom;

  delete fParticleList;

  delete fParticleState;
  delete fParticlePDG;
  delete fPrimaryVertex;

  delete fOrigParticleState;
  delete fOrigParticlePDG;
  delete fOrigPrimaryVertex;

  if (fGenInfo)
    fGenInfo->DeallocateParticleStack();

  kMaxParticles = 0;
}

void FitEvent::ClearFitParticles() {
  for (size_t i = 0; i < kMaxParticles; i++) {
    fParticleList[i] = NULL;
  }
}

void FitEvent::FreeFitParticles() {
  for (size_t i = 0; i < kMaxParticles; i++) {
    FitParticle *fp = fParticleList[i];
    if (fp)
      delete fp;
    fParticleList[i] = NULL;
  }
}

void FitEvent::ResetParticleList() {
  for (unsigned int i = 0; i < kMaxParticles; i++) {
    FitParticle *fp = fParticleList[i];
    if (fp)
      delete fp;
    fParticleList[i] = NULL;
  }
}

void FitEvent::HardReset() {
  for (unsigned int i = 0; i < kMaxParticles; i++) {
    fParticleList[i] = NULL;
  }
}

void FitEvent::ResetEvent() {
  Mode = 9999;
  fEventNo = -1;
  fTotCrs = -1.0;
  fTargetA = -1;
  fTargetZ = -1;
  fTargetH = -1;
  fBound = false;
  fNParticles = 0;

  if (fGenInfo)
    fGenInfo->Reset();

  for (unsigned int i = 0; i < kMaxParticles; i++) {
    if (fParticleList[i])
      delete fParticleList[i];
    fParticleList[i] = NULL;

    continue;

    fParticlePDG[i] = 0;
    fParticleState[i] = kUndefinedState;
    fParticleMom[i][0] = 0.0;
    fParticleMom[i][1] = 0.0;
    fParticleMom[i][2] = 0.0;
    fParticleMom[i][3] = 0.0;
    fPrimaryVertex[i] = false;

    fOrigParticlePDG[i] = 0;
    fOrigParticleState[i] = kUndefinedState;
    fOrigParticleMom[i][0] = 0.0;
    fOrigParticleMom[i][1] = 0.0;
    fOrigParticleMom[i][2] = 0.0;
    fOrigParticleMom[i][3] = 0.0;
    fOrigPrimaryVertex[i] = false;
  }
}

void FitEvent::OrderStack() {
  // Copy current stack
  int npart = fNParticles;

  for (int i = 0; i < npart; i++) {
    fOrigParticlePDG[i] = fParticlePDG[i];
    fOrigParticleState[i] = fParticleState[i];
    fOrigParticleMom[i][0] = fParticleMom[i][0];
    fOrigParticleMom[i][1] = fParticleMom[i][1];
    fOrigParticleMom[i][2] = fParticleMom[i][2];
    fOrigParticleMom[i][3] = fParticleMom[i][3];
    fOrigPrimaryVertex[i] = fPrimaryVertex[i];
  }

  // Now run loops for each particle
  fNParticles = 0;
  int stateorder[6] = {kInitialState,   kFinalState,     kFSIState,
                       kNuclearInitial, kNuclearRemnant, kUndefinedState};

  for (int s = 0; s < 6; s++) {
    for (int i = 0; i < npart; i++) {
      if ((UInt_t)fOrigParticleState[i] != (UInt_t)stateorder[s])
        continue;

      fParticlePDG[fNParticles] = fOrigParticlePDG[i];
      fParticleState[fNParticles] = fOrigParticleState[i];
      fParticleMom[fNParticles][0] = fOrigParticleMom[i][0];
      fParticleMom[fNParticles][1] = fOrigParticleMom[i][1];
      fParticleMom[fNParticles][2] = fOrigParticleMom[i][2];
      fParticleMom[fNParticles][3] = fOrigParticleMom[i][3];
      fPrimaryVertex[fNParticles] = fOrigPrimaryVertex[i];

      fNParticles++;
    }
  }

  if (LOG_LEVEL(DEB)) {
    NUIS_LOG(DEB, "Ordered stack");
    for (int i = 0; i < fNParticles; i++) {
      NUIS_LOG(DEB, "Particle " << i << ". " << fParticlePDG[i] << " "
                            << fParticleMom[i][0] << " " << fParticleMom[i][1]
                            << " " << fParticleMom[i][2] << " "
                            << fParticleMom[i][3] << " " << fParticleState[i]);
    }
  }

  if (fNParticles != npart) {
    NUIS_ABORT("Dropped some particles when ordering the stack!");
  }

  return;
}

void FitEvent::Print() {
  if (LOG_LEVEL(FIT)) {
    NUIS_LOG(FIT, "FITEvent print");
    NUIS_LOG(FIT, "Mode: " << Mode << ", Weight: " << InputWeight);
    NUIS_LOG(FIT, "Particles: " << fNParticles);
    NUIS_LOG(FIT, " -> Particle Stack ");
    for (int i = 0; i < fNParticles; i++) {
      NUIS_LOG(FIT, " -> -> " << i << ". " << fParticlePDG[i] << " "
                          << fParticleState[i] << " "
                          << "  Mom(" << fParticleMom[i][0] << ", "
                          << fParticleMom[i][1] << ", " << fParticleMom[i][2]
                          << ", " << fParticleMom[i][3] << ").");
    }
  }
  return;
}

/* Read/Write own event class */
void FitEvent::SetBranchAddress(TChain *tn) {
  tn->SetBranchAddress("Mode", &Mode);

  tn->SetBranchAddress("EventNo", &fEventNo);
  tn->SetBranchAddress("TotCrs", &fTotCrs);
  tn->SetBranchAddress("TargetA", &fTargetA);
  tn->SetBranchAddress("TargetH", &fTargetH);
  tn->SetBranchAddress("Bound", &fBound);

  tn->SetBranchAddress("RWWeight", &SavedRWWeight);
  tn->SetBranchAddress("InputWeight", &InputWeight);
}

void FitEvent::AddBranchesToTree(TTree *tn) {
  tn->Branch("Mode", &Mode, "Mode/I");

  tn->Branch("EventNo", &fEventNo, "EventNo/i");
  tn->Branch("TotCrs", &fTotCrs, "TotCrs/D");
  tn->Branch("TargetA", &fTargetA, "TargetA/I");
  tn->Branch("TargetH", &fTargetH, "TargetH/I");
  tn->Branch("Bound", &fBound, "Bound/O");

  tn->Branch("RWWeight", &RWWeight, "RWWeight/D");
  tn->Branch("InputWeight", &InputWeight, "InputWeight/D");

  tn->Branch("NParticles", &fNParticles, "NParticles/I");
  tn->Branch("ParticleState", fOrigParticleState,
             "ParticleState[NParticles]/i");
  tn->Branch("ParticlePDG", fOrigParticlePDG, "ParticlePDG[NParticles]/I");
  tn->Branch("ParticleMom", fOrigParticleMom, "ParticleMom[NParticles][4]/D");
}

// ------- EVENT ACCESS FUNCTION --------- //
TLorentzVector FitEvent::GetParticleP4(int index) const {
  if (index == -1 or index >= fNParticles)
    return TLorentzVector();
  return TLorentzVector(fParticleMom[index][0], fParticleMom[index][1],
                        fParticleMom[index][2], fParticleMom[index][3]);
}

TVector3 FitEvent::GetParticleP3(int index) const {
  if (index == -1 or index >= fNParticles)
    return TVector3();
  return TVector3(fParticleMom[index][0], fParticleMom[index][1],
                  fParticleMom[index][2]);
}

double FitEvent::GetParticleMom(int index) const {
  if (index == -1 or index >= fNParticles)
    return 0.0;
  return sqrt(fParticleMom[index][0] * fParticleMom[index][0] +
              fParticleMom[index][1] * fParticleMom[index][1] +
              fParticleMom[index][2] * fParticleMom[index][2]);
}

double FitEvent::GetParticleMom2(int index) const {
  if (index == -1 or index >= fNParticles)
    return 0.0;
  return fabs((fParticleMom[index][0] * fParticleMom[index][0] +
               fParticleMom[index][1] * fParticleMom[index][1] +
               fParticleMom[index][2] * fParticleMom[index][2]));
}

double FitEvent::GetParticleE(int index) const {
  if (index == -1 or index >= fNParticles)
    return 0.0;
  return fParticleMom[index][3];
}

int FitEvent::GetParticleState(int index) const {
  if (index == -1 or index >= fNParticles)
    return kUndefinedState;
  return (fParticleState[index]);
}

int FitEvent::GetParticlePDG(int index) const {
  if (index == -1 or index >= fNParticles)
    return 0;
  return (fParticlePDG[index]);
}

FitParticle *FitEvent::GetParticle(int const i) {
  // Check Valid Index
  if (i == -1) {
    return NULL;
  }

  // Check Valid
  if (i > fNParticles) {
    NUIS_ABORT("Requesting particle beyond stack!"
           << std::endl
           << "i = " << i << " N = " << fNParticles << std::endl
           << "Mode = " << Mode);
  }

  if (!fParticleList[i]) {
    /*
    std::cout << "Creating particle with values i " << i << " ";
    std::cout << fParticleMom[i][0] << " " << fParticleMom[i][1] <<  " " <<
    fParticleMom[i][2] << " " << fParticleMom[i][3] << " ";
    std::cout << fParticlePDG[i] << " " << fParticleState[i] << std::endl;
    */
    fParticleList[i] = new FitParticle(fParticleMom[i][0], fParticleMom[i][1],
                                       fParticleMom[i][2], fParticleMom[i][3],
                                       fParticlePDG[i], fParticleState[i]);
  } else {
    /*
    std::cout << "Filling particle with values i " << i << " ";
    std::cout << fParticleMom[i][0] << " " << fParticleMom[i][1] <<  " " <<
    fParticleMom[i][2] << " " << fParticleMom[i][3] << " ";
    std::cout << fParticlePDG[i] << " "<< fParticleState[i] <<std::endl;
    */
    fParticleList[i]->SetValues(fParticleMom[i][0], fParticleMom[i][1],
                                fParticleMom[i][2], fParticleMom[i][3],
                                fParticlePDG[i], fParticleState[i]);
  }

  return fParticleList[i];
}

bool FitEvent::HasParticle(int const pdg, int const state) const {
  bool found = false;
  for (int i = 0; i < fNParticles; i++) {
    if (state != -1 && fParticleState[i] != (uint)state)
      continue;
    if (fParticlePDG[i] == pdg)
      found = true;
  }
  return found;
}

int FitEvent::NumParticle(int const pdg, int const state) const {
  int nfound = 0;
  for (int i = 0; i < fNParticles; i++) {
    if (state != -1 and fParticleState[i] != (uint)state)
      continue;
    if (pdg == 0 or fParticlePDG[i] == pdg)
      nfound += 1;
  }
  return nfound;
}

std::vector<int> FitEvent::GetAllParticleIndices(int const pdg,
                                                 int const state) const {
  std::vector<int> indexlist;
  for (int i = 0; i < fNParticles; i++) {
    if (state != -1 and fParticleState[i] != (uint)state)
      continue;
    if (pdg == 0 or fParticlePDG[i] == pdg) {
      indexlist.push_back(i);
    }
  }
  return indexlist;
}

std::vector<FitParticle *> FitEvent::GetAllParticle(int const pdg,
                                                    int const state) {
  std::vector<int> indexlist = GetAllParticleIndices(pdg, state);
  std::vector<FitParticle *> plist;
  for (std::vector<int>::iterator iter = indexlist.begin();
       iter != indexlist.end(); iter++) {
    plist.push_back(GetParticle((*iter)));
  }
  return plist;
}

int FitEvent::GetHMParticleIndex(int const pdg, int const state) const {
  double maxmom2 = -9999999.9;
  int maxind = -1;
  for (int i = 0; i < fNParticles; i++) {
    if (state != -1 and fParticleState[i] != (uint)state)
      continue;
    if (pdg == 0 or fParticlePDG[i] == pdg) {
      double newmom2 = GetParticleMom2(i);
      if (newmom2 > maxmom2) {
        maxind = i;
        maxmom2 = newmom2;
      }
    }
  }

  return maxind;
}

int FitEvent::GetBeamNeutrinoIndex(void) const {
  for (int i = 0; i < fNParticles; i++) {
    if (fParticleState[i] != kInitialState)
      continue;
    int pdg = abs(fParticlePDG[i]);
    if (pdg == 12 or pdg == 14 or pdg == 16) {
      return i;
    }
  }
  return 0;
}

int FitEvent::GetBeamElectronIndex(void) const {
  return GetHMISParticleIndex(11);
}

int FitEvent::GetBeamPionIndex(void) const {
  return GetHMISParticleIndex(PhysConst::pdg_pions);
}

int FitEvent::GetBeamPartIndex(void) const {
  return GetHMISParticleIndex(this->probe_pdg);
}

int FitEvent::NumFSMesons() {
  int nMesons = 0;

  for (int i = 0; i < fNParticles; i++) {
    if (fParticleState[i] != kFinalState)
      continue;
    if (abs(fParticlePDG[i]) >= 111 && abs(fParticlePDG[i]) <= 557)
      nMesons += 1;
  }

  return nMesons;
}

int FitEvent::NumFSLeptons(void) const {
  int nLeptons = 0;

  for (int i = 0; i < fNParticles; i++) {
    if (fParticleState[i] != kFinalState)
      continue;
    if (abs(fParticlePDG[i]) == 11 || abs(fParticlePDG[i]) == 13 ||
        abs(fParticlePDG[i]) == 15)
      nLeptons += 1;
  }

  return nLeptons;
}

// Get the outgoing lepton PDG depending on if it's a CC or NC event
int FitEvent::GetLeptonOutPDG() {
  // Make sure the outgoing lepton has the correct PDG
  int pdgnu = PDGnu();
  int PDGout;
  if (IsCC()) {
    if (pdgnu > 0) PDGout = pdgnu-1;
    else PDGout = pdgnu+1;
  } else {
    PDGout = pdgnu;
  }
  return PDGout;
}

//********************************************************************
// Returns the true Q2 of an event
double FitEvent::GetQ2() {
  FitParticle *neutrino = GetNeutrinoIn();
  FitParticle *lepton = GetLeptonOut();
  // Figure out why this happens
  if (!neutrino || !lepton) return 0.0;
  double Q2 =
    -1.0 * (lepton->P4() - neutrino->P4()) *
    (lepton->P4() - neutrino->P4()) / 1.E6;
  return Q2;
}
//********************************************************************
