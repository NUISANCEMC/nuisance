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

#include "FitParticle.h"

// NEUT Constructor
#ifdef __NEUT_ENABLED__
FitParticle::FitParticle(NeutPart* part){

  // Set Momentum
  fP = TLorentzVector( part->fP.X(),
		       part->fP.Y(),
		       part->fP.Z(),
		       part->fP.T());

  fPID = part->fPID;
  fIsAlive = part->fIsAlive;
  fStatus = part->fStatus;
  fMass = part->fMass;
};
#endif

// NUWRO Constructor
#ifdef __NUWRO_ENABLED__
FitParticle::FitParticle(particle* nuwro_particle, Int_t state){

  // Set Momentum
  this->fP = TLorentzVector(nuwro_particle->p4().x,
			    nuwro_particle->p4().y,
			    nuwro_particle->p4().z,
			    nuwro_particle->p4().t);
  fPID = nuwro_particle->pdg;

  // Set status manually from switch
  switch(state){
  case 0:  fIsAlive= 0; fStatus=1; break; // Initial State
  case 1:  fIsAlive= 1; fStatus=0; break; // Final State
  case 2:  fIsAlive= 0; fStatus=2; break; // Intermediate State
  default: fIsAlive=-1; fStatus=3; break; // Other?
  }

  fMass = nuwro_particle->m();
};
#endif

// GENIE Constructor
#ifdef __GENIE_ENABLED__
FitParticle::FitParticle(genie::GHepParticle* genie_particle){
  
  this->fP = TLorentzVector(genie_particle->Px()*1000.0,
			    genie_particle->Py()*1000.0,
			    genie_particle->Pz()*1000.0,
			    genie_particle->E()*1000.0);
  
  fPID = genie_particle->Pdg();

  switch(genie_particle->Status()){
  case genie::kIStInitialState:      fIsAlive = 0; fStatus = 1; break; // Initial State
  case genie::kIStStableFinalState:  fIsAlive = 1; fStatus = 0; break; // Final State
  case genie::kIStIntermediateState: fIsAlive = 0; fStatus = 2; break; // Intermediate State
  default: fIsAlive = -1; fStatus = 3; break;
  }
  
  // Flag to remove nuclear part in genie
  if (fPID > 3000){
    fIsAlive = -1;
    fStatus  = 2;
  }

  fMass = genie_particle->Mass()*1000.0;

  // Additional flag to remove off shell particles
  if (fabs(fMass - fP.Mag()) > 0.001 ){
    fIsAlive = -1;
    fStatus = 2;
  }

};
#endif

FitParticle::FitParticle(UInt_t* i){

  (void) i;
  
  // A NULL event has been passed
  ERR(FTL)<<"NULL Event Passed to FitEvent.cxx"<<std::endl;
  
  return;
};

  
ClassImp(FitParticle);
