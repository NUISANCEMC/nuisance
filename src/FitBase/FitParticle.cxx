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

#include "FitParticle.h"

// NUANCE Particle
FitParticle::FitParticle(double x, double y, double z, double t, int pdg, Int_t state){

  // Set Momentum
  this->fP = TLorentzVector(x,
			    y,
			    z,
			    t);
  fPID = pdg;
  fStatus = state;

  // Set status manually from switch
  switch(state){
  case     kInitialState: fIsAlive= 0; fNEUTStatusCode=1; break; // Initial State
  case     kFinalState:   fIsAlive= 1; fNEUTStatusCode=0; break; // Final State
  case     kFSIState:     fIsAlive= 0; fNEUTStatusCode=2; break; // Intermediate State
  default: fIsAlive=-1; fNEUTStatusCode=3; break; // Other?
  }

  fMass = fP.Mag();
};
