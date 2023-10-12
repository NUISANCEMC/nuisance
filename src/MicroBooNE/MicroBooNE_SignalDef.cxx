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

    //----------------------------------------//

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

    //----------------------------------------//

bool isCC1Mu2p(FitEvent* event, double EnuMin, double EnuMax) {
  // Check CC inclusive
  if (!SignalDef::isCCINC(event, 14, EnuMin, EnuMax)) return false;

  // Veto events which don't have exactly 1 FS muon
  if (event->NumFSMuon() != 1) return false;

  // Muon momentum range
  if (event->GetHMFSParticle(13)->fP.Vect().Mag() < 100) return false;
  if (event->GetHMFSParticle(13)->fP.Vect().Mag() > 1200) return false;
  
  // Check the existence of at least 2 protons in the final state
  int NFSProtons = event->NumFSParticle(2212);
  if (NFSProtons < 2) return false;

  int ProtonCounter = 0;
  std::vector<int> ProtonIndices = event->GetAllFSProtonIndices();

  for (int i = 0; i < NFSProtons; i++) {

    double mom = event->GetParticleMom( ProtonIndices.at(i) );
    if (mom > 300 && mom < 1000) { ProtonCounter++; }

  }

  if (ProtonCounter != 2) { return false; }

  // Reject events with neutral pions of any momenta
  if (event->NumFSParticle(111) != 0) return false;

  // Reject events with positively charged pions above 65 MeV/c
  if (event->NumFSParticle(211) != 0) {
    double ppiplus = event->GetHMFSParticle(211)->fP.Vect().Mag();
    if (ppiplus > 65) { return false; }
  }
  
  // Reject events with negatively charged pions above 65 MeV/c
  if (event->NumFSParticle(-211) != 0) {
    double ppiminus = event->GetHMFSParticle(-211)->fP.Vect().Mag();
    if (ppiminus > 65) { return false; }
  }
  
  return true;
}

    //----------------------------------------//

bool isCC1Mu1p(FitEvent* event, double EnuMin, double EnuMax) {
  // Check CC inclusive
  if (!SignalDef::isCCINC(event, 14, EnuMin, EnuMax)) return false;

  // Veto events which don't have exactly 1 FS muon
  if (event->NumFSMuon() != 1) return false;

  // Muon momentum range
  if (event->GetHMFSParticle(13)->fP.Vect().Mag() < 100) return false;
  if (event->GetHMFSParticle(13)->fP.Vect().Mag() > 1200) return false;
  
  // Check the existence of at least 1 proton in the final state
  int NFSProtons = event->NumFSParticle(2212);
  if (NFSProtons < 1) return false;

  int ProtonCounter = 0;
  std::vector<int> ProtonIndices = event->GetAllFSProtonIndices();

  for (int i = 0; i < NFSProtons; i++) {

    double mom = event->GetParticleMom( ProtonIndices.at(i) );
    if (mom > 300 && mom < 1000) { ProtonCounter++; }

  }

  if (ProtonCounter != 1) { return false; }

  // Reject events with neutral pions of any momenta
  if (event->NumFSParticle(111) != 0) return false;

  // Reject events with positively charged pions above 70 MeV/c
  if (event->NumFSParticle(211) != 0) {
    double ppiplus = event->GetHMFSParticle(211)->fP.Vect().Mag();
    if (ppiplus > 70) { return false; }
  }
  
  // Reject events with negatively charged pions above 70 MeV/c
  if (event->NumFSParticle(-211) != 0) {
    double ppiminus = event->GetHMFSParticle(-211)->fP.Vect().Mag();
    if (ppiminus > 70) { return false; }
  }
  
  return true;
}

    //----------------------------------------//


  }  // namespace MicroBooNE
}  // namespace SignalDef
