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
#include <iostream>
#include "FitEvent.h"
#include "TObjArray.h"

//***************************************************
// Refill all the particle vectors etc for the event
void FitEvent::CalcKinematics(){
//***************************************************

#ifdef __NEUT_ENABLED__
  if ( fType == kNEUT  ) NeutKinematics();
#endif
  
#ifdef __NUWRO_ENABLED__
  if ( fType == kNUWRO ) NuwroKinematics();
#endif
  
#ifdef __GENIE_ENABLED__
  if ( fType == kGENIE ) GENIEKinematics();
#endif
  
#ifdef __NUANCE_ENABLED__
  if ( fType == kNUANCE ) NuanceKinematics();
#endif
  
#ifdef __GiBUU_ENABLED__
  if ( fType == kGiBUU ) GiBUUKinematics();
#endif

  return;
};

//***************************************************
void FitEvent::ResetEvent(){
//***************************************************

  // Sort Event Info
  fMode    = 9999;
  Mode     = 9999;
  fEventNo = -1;
  fTotCrs  = -1.0;
  fTargetA = -1;
  fTargetZ = -1;
  fTargetH = -1;
  fBound   = false;
  fNParticles    = 0;
  fCurParticleIndex = -1;

}

// NEUT GENERATOR SPECIFIC
#ifdef __NEUT_ENABLED__
//*************************************************** 
void FitEvent::SetEventAddress(NeutVect** tempevent){
//*************************************************** 
  fType = kNEUT;
  fNeutVect = *tempevent;
}

//*************************************************** 
void FitEvent::NeutKinematics(){
//*************************************************** 
  ResetEvent();

  // Get Event Info
  fMode    = fNeutVect->Mode;
  Mode     = fNeutVect->Mode;
  fEventNo = fNeutVect->EventNo;
  fTotCrs  = fNeutVect->Totcrs;
  fTargetA = fNeutVect->TargetA;
  fTargetZ = fNeutVect->TargetZ;
  fTargetH = fNeutVect->TargetH;
  fBound   = bool(fNeutVect->Ibound);

  // Check Particle Stack
  UInt_t npart = fNeutVect->Npart();
  if (npart > kMaxParticles){
    ERR(FTL) << "NEUT has too many particles" << endl;
    ERR(FTL) << "npart="<<npart<<" kMax="
	     << kMaxParticles << endl;
    throw;
  }

  // Fill Particle Stack
  fNParticles =  0;
  fCurParticleIndex = -1;
  
  for (UInt_t i = 0; i < npart; i++){
    NeutPart* part = fNeutVect->PartInfo(i);

    // State
    int state = kUndefinedState;
    if (part->fIsAlive == 0 and part->fStatus == -1){
      state = kInitialState;
    } else if (part->fIsAlive == 0 and part->fStatus == 2){
      state = kFSIState;
    } else if (part->fIsAlive == 1 and part->fStatus == 0){
      state = kFinalState;
    } else if (part->fIsAlive == 1 and part->fStatus == 2){
      state = kFinalState; //NC Neutrino
    } else if (part->fIsAlive == 1){
      ERR(WRN) << "Undefined NEUT state "
	       << " Alive: " << part->fIsAlive
	       << " Status: " << part->fStatus
	       << " PDG: " << part->fPID << endl;
    }
   
    // Remove Undefined
    //    if (kRemoveUndefParticles &&
    //	state == kUndefinedState) continue;
    
    // Remove FSI
    //    if (kRemoveFSIParticles &&
    //	state == kFSIState) continue;

    fParticleState[fNParticles] = state;
    
    // Mom
    fParticleMom[fNParticles][0] = part->fP.X();
    fParticleMom[fNParticles][1] = part->fP.Y();
    fParticleMom[fNParticles][2] = part->fP.Z();
    fParticleMom[fNParticles][3] = part->fP.T();

    // PDG
    fParticlePDG[fNParticles] = part->fPID;

    fNParticles++;
  }

  /*
    From MERGE -> Change to own loop
  
  // FSI Vertex Counting Should go here.
  this->fNFSIParticles = neut_event->NfsiPart();
  fsi_particles.clear();
  for (UInt_t i = 0; i < this->fNFSIParticles; i++) {
    this->fsi_particles.push_back(FitParticle(neut_event->FsiPartInfo(i)));
  }

>>>>>>> 11fb11c0eacec8ee8d48186e0c9c847ce29a1930
  */
  
  return;
};
#endif



// NUWRO GENERATOR SPECIFIC
#ifdef __NUWRO_ENABLED__
//*************************************************** 
void FitEvent::SetEventAddress(event** tempevent){
//*************************************************** 
  fType = kNUWRO;
  fNuwroEvent = *(tempevent);
  return;
}
//*************************************************** 
void FitEvent::NuwroKinematics(){
//*************************************************** 
  ResetEvent();

  // Sort Event Info
  fMode    = GeneratorUtils::ConvertNuwroMode(fNuwroEvent);
  Mode     = fMode;
  fEventNo = 0.0;
  fTotCrs  = 0.0;
  fTargetA = fNuwroEvent->par.nucleus_p + fNuwroEvent->par.nucleus_n;
  fTargetZ = fNuwroEvent->par.nucleus_p;
  fTargetH = 0;
  fBound   = (fTargetA) == 1;
  
  // Check Particle Stack
  UInt_t npart_in   = fNuwroEvent->in.size();
  UInt_t npart_out  = fNuwroEvent->out.size();
  UInt_t npart_post = fNuwroEvent->post.size();
  UInt_t npart = npart_in + npart_out + npart_post;

  if (npart > kMaxParticles){
    ERR(FTL) << "NUWRO has too many particles" << endl;
    ERR(FTL) << "npart="<<npart<<" kMax="
	     << kMaxParticles 
	     << " in,out,post = " << npart_in << ","
	     << npart_out << "," << npart_post << endl;
    throw;
  }

  // Incoming Particles
  fNParticles = 0;
  fCurParticleIndex = -1;
  
  for (UInt_t i = 0; i < npart_in; i++){
    particle* part = &fNuwroEvent->in[i];
    
    fParticleMom[fNParticles][0] = part->p4().x;
    fParticleMom[fNParticles][1] = part->p4().y;
    fParticleMom[fNParticles][2] = part->p4().z;
    fParticleMom[fNParticles][3] = part->p4().t;
    
    fParticleState[fNParticles] = kInitialState;
    fParticlePDG[fNParticles] = part->pdg;
    fNParticles++;
  }

  // FSI Particles
  if (!kRemoveFSIParticles){
    for (UInt_t i = 0; i < npart_out; i++){
      particle* part = &fNuwroEvent->out[i];
      
      fParticleMom[fNParticles][0] = part->p4().x;
      fParticleMom[fNParticles][1] = part->p4().y;
      fParticleMom[fNParticles][2] = part->p4().z;
      fParticleMom[fNParticles][3] = part->p4().t;
      
      fParticleState[fNParticles] = kFSIState;
      fParticlePDG[fNParticles] = part->pdg;
      fNParticles++;
    }
  }

  // FSI Particles
  for (UInt_t i = 0; i < npart_post; i++){
    particle* part = &fNuwroEvent->post[i];

    fParticleMom[fNParticles][0] = part->p4().x;
    fParticleMom[fNParticles][1] = part->p4().y;
    fParticleMom[fNParticles][2] = part->p4().z;
    fParticleMom[fNParticles][3] = part->p4().t;

    fParticleState[fNParticles] = kFinalState;
    fParticlePDG[fNParticles] = part->pdg;
    fNParticles++;
  }
  
  return;
};
#endif //< NuWro ifdef



// REQUIRED FUNCTIONS FOR GENIE
#ifdef __GENIE_ENABLED__
//***************************************************
void FitEvent::SetEventAddress(NtpMCEventRecord** tempevent){
//***************************************************
  fType = kGENIE;
  genie_event = *tempevent;
};

//***************************************************
void FitEvent::GENIEKinematics(){
//***************************************************
  ResetEvent();
  genie_record = static_cast<GHepRecord*>(genie_event->event);
  
  // Extra Check for MEC
  if (genie_record->Summary()->ProcInfo().IsMEC()){
    if      (pdg::IsNeutrino       (genie_record->Summary()->InitState().ProbePdg())){ fMode = 2;  }
    else if (pdg::IsAntiNeutrino   (genie_record->Summary()->InitState().ProbePdg())){ fMode = -2; }
  } else {
    fMode = utils::ghep::NeutReactionCode(genie_record);
  }

  Mode     = fMode;
  fEventNo = 0.0;
  fTotCrs  = genie_record->XSec();
  fTargetA = 0.0;
  fTargetZ = 0.0;
  fTargetH = 0;
  fBound   = 0.0;
  
  // Get N Particle Stack
  int npart = genie_record->GetEntries();
  if (npart > kMaxParticles){
    ERR(FTL) << "GENIE has too many particles" << endl;
    ERR(FTL) << "npart="<<npart<<" kMax="
	     << kMaxParticles << endl;

    throw;
  }
  
  // Fill Particle Stack
  GHepParticle * p = 0;
  TObjArrayIter iter(genie_record);

  fNParticles = 0;
  fCurParticleIndex = -1;

  // Loop over all particles
  while((p) = (dynamic_cast<genie::GHepParticle*>( (iter).Next() )) ){
    if (!p) continue;

    // State
    int state = kUndefinedState;
    switch (p->Status()){

    case genie::kIStInitialState:
      state = kInitialState;
      break;
      
    case genie::kIStStableFinalState:
      state = kFinalState;
      break;

    case genie::kIStIntermediateState:
      state = kFSIState;
      break;

    default:
      break;
    }

    // Flag to remove nuclear part in genie
    if (kRemoveGenieNuclear &&
	(p->Pdg() > 3000 || p->Pdg() < -3000)) {
      state = kUndefinedState;
    }

    // Remove Undefined
    if (kRemoveUndefParticles &&
	state == kUndefinedState) continue;

    // Remove FSI
    if (kRemoveFSIParticles &&
	state == kFSIState) continue;
    
    fParticleState[fNParticles] = state;

    // Mom
    fParticleMom[fNParticles][0] = p->Px() * 1.E3;
    fParticleMom[fNParticles][1] = p->Py() * 1.E3;
    fParticleMom[fNParticles][2] = p->Pz() * 1.E3;
    fParticleMom[fNParticles][3] = p->E() * 1.E3;
    
    // PDG
    fParticlePDG[fNParticles] = p->Pdg();

    fNParticles++;
  }

  return;
};
#endif  //< GENIE ifdef



// REQUIRED FUNCTIONS FOR GIBUU
#ifdef __GiBUU_ENABLED__
//***************************************************
void FitEvent::SetEventAddress(GiBUUStdHepReader* tempevent){
//***************************************************
  fType = kGiBUU;
  GiRead = tempevent;
}

//***************************************************
void FitEvent::GiBUUKinematics(){
//***************************************************
  ResetEvent();
  
  // std::cout << "Reading GiBUU Event: " << std::endl;
  // std::cout << WriteGiBUUEvent(*GiRead) << std::endl;

  fMode    = GiRead->GiBUU2NeutCode;
  Mode     = fMode;
  fEventNo = 0.0;
  fTotCrs  = 0;
  fTargetA = 0.0;
  fTargetZ = 0.0;
  fTargetH = 0;
  fBound   = 0.0;

  // Extra GiBUU Input Weight
  InputWeight = GiRead->EvtWght;
  
  // Check Stack N
  int npart = GiRead->StdHepN;
  if (npart > kMaxParticles){
    ERR(FTL) << "GiBUU has too many particles" << endl;
    ERR(FTL) << "npart="<<npart<<" kMax="
	     << kMaxParticles << endl;
    throw;
  }
  
  // Create Stack
  fNParticles = 0;
  fCurParticleIndex = -1;
  for (int i = 0; i < npart; i++){

    // State
    int state = kUndefinedState;
    switch(GiRead->StdHepStatus[i]){
      
    case 0:  // Incoming
    case 11: // Struck Nucleon
      state = kInitialState;
      break;
      
    case 1: // Good Final State
      state = kFinalState;
      break;
      
    default: // Other 
      break;
    }

    // Remove Nuclear States Flag
    if (kRemoveGiBUUNuclear &&
	(fParticlePDG[fNParticles] > 100000)) {
      state = kUndefinedState;
    }

    // Remove Undefined
    if (kRemoveUndefParticles &&
	state == kUndefinedState) continue;

    // Remove FSI
    if (kRemoveFSIParticles &&
	state == kFSIState) continue;
    
    fParticleState[fNParticles] = state;

    //Mom
    fParticleMom[fNParticles][0] = GiRead->StdHepP4[i][0] * 1.E3;
    fParticleMom[fNParticles][1] = GiRead->StdHepP4[i][1] * 1.E3;
    fParticleMom[fNParticles][2] = GiRead->StdHepP4[i][2] * 1.E3;
    fParticleMom[fNParticles][3] = GiRead->StdHepP4[i][3] * 1.E3;

    //PDG
    fParticlePDG[fNParticles] = GiRead->StdHepPdg[i];
    
    fNParticles++;
  }

  
}
#endif  //< GiBUU ifdef


// REQUIRED FUNCTIONS FOR NUANCE
#ifdef __NUANCE_ENABLED__
//***************************************************
void FitEvent::SetEventAddress(NuanceEvent** tempevent){
  //***************************************************
  fType = kNUANCE;
  nuance_event = *tempevent;
}

//***************************************************
void FitEvent::NuanceKinematics(){
//***************************************************
  ResetEvent();

  fMode    = GeneratorUtils::ConvertNuanceMode(nuance_event);
  Mode     = fMode;
  fEventNo = 0.0;
  fTotCrs  = 1.0;
  fTargetA = 0.0;
  fTargetZ = 0.0;
  fTargetH = 0;
  fBound   = 0.0;

  // Fill particle Stack
  fNParticles = 0;
  fCurParticleIndex = -1;

  // Check Particle Stack   
  UInt_t npart  = 2 + nuance_event->n_leptons + nuance_event->n_hadrons;

  if (npart > kMaxParticles){
    ERR(FTL) << "NUANCE has too many particles" << endl;
    ERR(FTL) << "npart="<<npart<<" kMax="
             << kMaxParticles << endl;
    throw;
  }

  // Fill Neutrino
  fParticleState[0]  = kInitialState;                 
  fParticleMom[0][0] = nuance_event->p_neutrino[0];
  fParticleMom[0][1] = nuance_event->p_neutrino[1];
  fParticleMom[0][2] = nuance_event->p_neutrino[2];
  fParticleMom[0][3] = nuance_event->p_neutrino[3];    
  fParticlePDG[0]    = nuance_event->neutrino;

  // Fill Target Nucleon
  fParticleState[1]  = kInitialState;
  fParticleMom[1][0] = nuance_event->p_targ[0];
  fParticleMom[1][1] = nuance_event->p_targ[1];
  fParticleMom[1][2] = nuance_event->p_targ[2];
  fParticleMom[1][3] = nuance_event->p_targ[3];
  fParticlePDG[1]    = nuance_event->target;
  fNParticles = 2;

  // Fill Outgoing Leptons
  for (int i = 0; i < nuance_event->n_leptons; i++){

    fParticleState[fNParticles]  = kFinalState;
    fParticleMom[fNParticles][0] = nuance_event->p_lepton[i][0];
    fParticleMom[fNParticles][1] = nuance_event->p_lepton[i][1];
    fParticleMom[fNParticles][2] = nuance_event->p_lepton[i][2];
    fParticleMom[fNParticles][3] = nuance_event->p_lepton[i][3];
    fParticlePDG[fNParticles]    = nuance_event->lepton[i];
    fNParticles++;
    
  }
    
  // Fill Outgoing Hadrons
  for (int i = 0; i < nuance_event->n_hadrons; i++){

    fParticleState[fNParticles]  = kFinalState;
    fParticleMom[fNParticles][0] = nuance_event->p_hadron[i][0];
    fParticleMom[fNParticles][1] = nuance_event->p_hadron[i][1];
    fParticleMom[fNParticles][2] = nuance_event->p_hadron[i][2];
    fParticleMom[fNParticles][3] = nuance_event->p_hadron[i][3];
    fParticlePDG[fNParticles]    = nuance_event->hadron[i];
    fNParticles++;

  }
}
#endif


/* Read/Write own event class */
void FitEvent::SetBranchAddress(TChain* tn){

  fType = kINPUTFITEVENT;
  
  tn->SetBranchAddress("Mode",    &fMode);
  tn->SetBranchAddress("Mode",    &Mode);
  tn->SetBranchAddress("EventNo", &fEventNo);
  tn->SetBranchAddress("TotCrs",  &fTotCrs);
  tn->SetBranchAddress("TargetA", &fTargetA);
  tn->SetBranchAddress("TargetH", &fTargetH);
  tn->SetBranchAddress("Bound",   &fBound);

  tn->SetBranchAddress("InputWeight", &InputWeight);

  tn->SetBranchAddress("NParticles",    &fNParticles);
  tn->SetBranchAddress("ParticleState", fParticleState);
  tn->SetBranchAddress("ParticlePDG",   fParticlePDG);
  tn->SetBranchAddress("ParticleMom",   fParticleMom);
  
}


void FitEvent::AddBranchesToTree(TTree* tn){

  tn->Branch("Mode",    &fMode,    "Mode/I");
  tn->Branch("EventNo", &fEventNo, "EventNo/i");
  tn->Branch("TotCrs",  &fTotCrs,  "TotCrs/D");
  tn->Branch("TargetA", &fTargetA, "TargetA/I");
  tn->Branch("TargetH", &fTargetH, "TargetH/I");
  tn->Branch("Bound",   &fBound,   "Bound/O");

  tn->Branch("InputWeight", &InputWeight, "InputWeight/D");
  
  tn->Branch("NParticles",    &fNParticles,   "NParticles/I");
  tn->Branch("ParticleState", fParticleState, "ParticleState[NParticles]/i");
  tn->Branch("ParticlePDG",   fParticlePDG,   "ParticlePDG[NParticles]/I");
  tn->Branch("ParticleMom",   fParticleMom,   "ParticleMom[NParticles][4]/D");

  tn->SetAlias("Enu","ParticleMom[0][4]");
  
  
}


/* Event Access Functions */
//***************************************************

FitParticle* FitEvent::PartInfo(UInt_t i){

  // Check its not just a repeated read.
  if (i != fCurParticleIndex){

    // Check Valid
    if (i > fNParticles or i < 0){
      ERR(FTL) << "Requesting particle beyond stack!" << endl;
      ERR(FTL) << "i = " << i << " N = " << fNParticles
	       << " currindex = " << fCurParticleIndex << endl;

      throw;
    }

    /*
    cout << "Getting particle " << i << endl;
    cout << fParticleMom[i][0] << " "
	 << fParticleMom[i][1] << " "
	 << fParticleMom[i][2] << " "
	 << fParticleMom[i][3] << " "
	 << fParticlePDG[i] << " "
	 << fParticleState[i] << endl;
    */
    
    // Create new particle for reading
    fCurParticleIndex = i;
    fCurParticle = FitParticle(fParticleMom[i][0],
			       fParticleMom[i][1],
			       fParticleMom[i][2],
			       fParticleMom[i][3],
			       fParticlePDG[i],
			       fParticleState[i]);

    return &fCurParticle;
  }

  // Return Created Particle
  return &fCurParticle;
}

int FitEvent::GetNeutrinoInPos(void) const{

  for (UInt_t i = 0; i < NPart(); i++){
    if (fParticleState[i] != kInitialState) continue;
    
    if (fParticlePDG[i] == 12 ||
	fParticlePDG[i] == 14 ||
	fParticlePDG[i] == 16) {
      return i;
    }
  }
  
}

int FitEvent::GetLeptonOutPos(void) const{

  for (UInt_t i = 0; i < NPart(); i++){
    if (fParticleState[i] != kFinalState) continue;

    if (fParticlePDG[i] == 11 ||
	fParticlePDG[i] == 13 ||
	fParticlePDG[i] == 15) {
      return i;
    }
  }
}

FitParticle* FitEvent::GetBeamPart(void){
  return PartInfo( GetBeamPartPos() );
}

FitParticle* FitEvent::GetNeutrinoIn(void){
  return PartInfo( GetNeutrinoInPos() );
}

FitParticle* FitEvent::GetLeptonOut(void){
  return PartInfo( GetLeptonOutPos() );
}


