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

#include "MicroBooNE_SignalDef.h"
#include "FitUtils.h"

namespace SignalDef {
  namespace MicroBooNE {

bool isCC1MuNp(FitEvent* event, double EnuMin, double EnuMax) {
  // Check CC inclusive
  if (!SignalDef::isCCINC(event, 14, EnuMin, EnuMax)) return false;

  // Veto events which don't have exactly 1 FS muon
  if (event->NumFSMuon() != 1) return false;

  // Veto events with FS mesons
  if (event->NumFSPions() != 0) return false;

  // Veto events with FS electrons
  if (event->NumFSElectron() != 0) return false;

  // Veto events with FS photons
  if (event->NumFSPhoton() != 0) return false;

  // Muon momentum above threshold
  if (event->GetHMFSParticle(13)->fP.Vect().Mag() < 100) return false;

  // Leading proton within momentum range
  if (event->NumFSParticle(2212) == 0) return false;
  double plead = event->GetHMFSParticle(2212)->fP.Vect().Mag();
  if (plead > 300 && plead < 1200) return true;

  return false;
}

bool isCC1ENp(FitEvent* event, double EnuMin, double EnuMax) {
  // Check CC inclusive
  if (!SignalDef::isCCINC(event, 12, EnuMin, EnuMax)) return false;

  // Veto events which don't have 1 or more FS protons
  if (event->NumFSProton() == 0) return false;

  // Veto events which don't have exactly 1 FS electron
  if (event->NumFSElectron() != 1) return false;

  // Veto events with any charged or neutral pions with kinetic energy > 40MeV
  std::vector<FitParticle*> PiPlusParticles = event->GetAllPiPlus();
  std::vector<FitParticle*> PiMinusParticles = event->GetAllPiMinus();
  std::vector<FitParticle*> PiZeroParticles = event->GetAllPiZero();

  std::vector<FitParticle*> PiParticles;
  for (uint i=0;i<PiPlusParticles.size();i++) {PiParticles.push_back(PiPlusParticles[i]);}
  for (uint i=0;i<PiMinusParticles.size();i++) {PiParticles.push_back(PiMinusParticles[i]);}
  for (uint i=0;i<PiZeroParticles.size();i++) {PiParticles.push_back(PiZeroParticles[i]);}

  double KEThreshold = 40.0/1000; //Units in MeV

  uint nPionsWithKEAboveThreshold = 0;
  for (uint i=0;i<PiParticles.size();i++) {
    if (PiParticles[i]->KE()>=KEThreshold) {
      nPionsWithKEAboveThreshold += 1;
    }
  }

  if (nPionsWithKEAboveThreshold != 0) return false;

  return true;
}

  }  // namespace MicroBooNE
}  // namespace SignalDef

