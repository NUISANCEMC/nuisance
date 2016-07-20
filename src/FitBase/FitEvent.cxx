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

#include "FitEvent.h"
#include "TObjArray.h"

//***************************************************
// NEUT GENERATOR SPECIFIC
#ifdef __NEUT_ENABLED__
void FitEvent::SetEventAddress(NeutVect** tempevent){

  this->fType = kNEUT;
  neut_event = *tempevent;

  return;
}

void FitEvent::NeutKinematics(){

  /*
    header information that needs filling

    UInt_t fMode;
    UInt_t EventNo;
    Double_t TotCrs;
    Double_t PFSurf;
    Double_t PFMax;
    UInt_t   TargetA;
    UInt_t   TargetZ;
    UInt_t   TargetH;
    UInt_t   Ibound;
    UInt_t fNParticles;
    UInt_t fNFSIParticles;
    UInt_t fNFinalParticles;
  */

  // Reset number of particles etc for the event
  this->ResetEvent();

  this->Mode     = neut_event->Mode;
  this->fEventNo = neut_event->EventNo;
  this->TotCrs   = neut_event->Totcrs;
  this->PFSurf   = neut_event->PFSurf;
  this->PFMax    = neut_event->PFMax;
  this->TargetA  = neut_event->TargetA;
  this->TargetZ  = neut_event->TargetZ;
  this->TargetH  = neut_event->TargetH;
  this->Ibound   = neut_event->Ibound;

  // Particle Counts
  this->fNParticles = neut_event->Npart();
  this->all_particles.clear();

  // count up other particles
  for (UInt_t i = 0; i < this->fNParticles; i++){
    this->all_particles.push_back(FitParticle(neut_event->PartInfo(i)));
  }

  // FSI Vertex Counting Should go here.


  return;
};
#endif
//***************************************************

//***************************************************
// NUWRO GENERATOR SPECIFIC
#ifdef __NUWRO_ENABLED__

//***************************************************
void FitEvent::SetEventAddress(event** tempevent){
//***************************************************

  this->fType = kNUWRO;
  nuwro_event = *(tempevent);

  return;
}



//***************************************************
void FitEvent::NuwroKinematics(){
//***************************************************

  this->ResetEvent();

  // Sort Mode
  this->TotCrs = nuwro_event->weight;
  this->Mode = GeneratorUtils::ConvertNuwroMode(nuwro_event);

  // These need to be set somehow...
  this->fEventNo = 0;
  this->PFSurf = nuwro_event->par.nucleus_kf;
  this->PFMax  = nuwro_event->par.nucleus_kf;
  this->TargetA = nuwro_event->par.nucleus_n;
  this->TargetZ = nuwro_event->par.nucleus_p;
  this->TargetH = 0;
  this->Ibound  = (this->TargetA + this->TargetZ) == 1;

  // Setup particles
  all_particles.clear();

  // Incoming particles state 0
  for (UInt_t i = 0; i < nuwro_event->in.size(); i++)
    all_particles.push_back( FitParticle(&nuwro_event->in[i], 0) );

  // Intermediate Particles state 2
  for (UInt_t i = 0; i < nuwro_event->out.size(); i++)
    all_particles.push_back( FitParticle(&nuwro_event->out[i], 2) );

  // Outgoing Particles State 1
  for (UInt_t i = 0; i < nuwro_event->post.size(); i++)
    all_particles.push_back( FitParticle(&nuwro_event->post[i], 1) );

  this->fNParticles = this->all_particles.size();

  return;

};
#endif //< NuWro ifdef
//***************************************************


//***************************************************
// REQUIRED FUNCTIONS FOR GENIE
#ifdef __GENIE_ENABLED__

//***************************************************
void FitEvent::SetEventAddress(NtpMCEventRecord** tempevent){
//***************************************************

  this->fType = kGENIE;
  genie_event = *tempevent;

};

//***************************************************
void FitEvent::GENIEKinematics(){
//***************************************************

  this->ResetEvent();

  // make the accessible record
  genie_record = static_cast<GHepRecord*>(genie_event->event);

  this->Mode = utils::ghep::NeutReactionCode(genie_record);
  this->TotCrs = genie_record->XSec();

  // These need to be set somehow...
  this->fEventNo = 0;
  this->PFSurf = 0;
  this->PFMax  = 0;
  this->TargetA = 0;
  this->TargetZ = 0;
  this->TargetH = 0;
  this->Ibound  = 0;

  this->fNParticles = genie_record->GetEntries();

  // State defines where, 0 = ALL, 1 = Incoming, 2 = FSI, 3 = Final
  int count = 0;
  GHepParticle * p = 0;
  TObjArrayIter iter(genie_record);
  all_particles.clear();
  while(p = dynamic_cast<genie::GHepParticle*>(iter.Next())){
    if (!p) continue;
    all_particles.push_back(FitParticle(p));
  }


  return;
};
#endif  //< GENIE ifdef
//***************************************************


//***************************************************
// Refill all the particle vectors etc for the event
void FitEvent::CalcKinematics(){
//***************************************************

  #ifdef __NEUT_ENABLED__
  if      ( fType == kNEUT  ) this->NeutKinematics();
  #endif

  #ifdef __NUWRO_ENABLED__
  if ( fType == kNUWRO ) this->NuwroKinematics();
  #endif

  #ifdef __GENIE_ENABLED__
  if ( fType == kGENIE ) this->GENIEKinematics();
  #endif

  if ( fType == kNUANCE ) this->NuanceKinematics();
  //  std::cout<<"Calling Nuance Kineamtics? "<<fType<<std::endl;

  return;
};

//***************************************************
void FitEvent::ResetEvent(){
//***************************************************

  this->Mode   = 999;
  this->fEventNo = 0;
  this->TotCrs  = 0.0;
  this->PFSurf  = 0.0;
  this->PFMax   = 0.0;
  this->TargetA = 0.0;
  this->TargetZ = 0.0;
  this->TargetH = 0.0;
  this->Ibound  = 0.0;

  this->fNParticles      = 0;
  this->fNFSIParticles   = 0;
  this->fNFinalParticles = 0;
  this->fCurrPartIndex = 999;

  return;
}

//***************************************************
FitParticle* FitEvent::PartInfo(UInt_t i){
//***************************************************

  if (i < all_particles.size()){

    return &(all_particles.at(i));

  } else {

    return NULL;

  }
}


//***************************************************
void FitEvent::NuanceKinematics(){
//***************************************************

  this->ResetEvent();
  //  std::cout<<"Calling Nuance Kinematics"<<endl;

  // Sort Mode
  this->TotCrs = 1.0; // NEEDS SORTING
  this->Mode = 1.0; // NEEDS SORTING 

  // These need to be set somehow...
  this->fEventNo = 0;
  this->PFSurf = 0.0;
  this->PFMax  = 0.0; 
  this->TargetA = 0.0;
  this->TargetZ = 0.0;
  this->TargetH = 0;
  this->Ibound  = 0.0;

  // Setup particles
  all_particles.clear();

  /*
  std::cout<<"Incoming Neutrino = "
	   <<  nuance_event->p_neutrino[0] << " "
	   <<  nuance_event->p_neutrino[1] << " "
	   <<  nuance_event->p_neutrino[2] << " "
	   <<  nuance_event->p_neutrino[3] << std::endl;
  */

  // incoming neutrino
  all_particles.push_back( FitParticle( nuance_event->p_neutrino[0],
					nuance_event->p_neutrino[1],
					nuance_event->p_neutrino[2],
					nuance_event->p_neutrino[3], 
					nuance_event->neutrino,
					0 ) );

  // incoming hadron
  all_particles.push_back( FitParticle( nuance_event->p_targ[0],
					nuance_event->p_targ[1],
					nuance_event->p_targ[2],
					nuance_event->p_targ[3], 
					nuance_event->target,
					0 ) );


  // for outgoing leptons
  //  std::cout<<"n Leptons = "<<nuance_event->n_leptons<<std::endl;
  for (int i = 0; i < nuance_event->n_leptons; i++){
    all_particles.push_back( FitParticle( nuance_event->p_lepton[i][0],
					  nuance_event->p_lepton[i][1],
					  nuance_event->p_lepton[i][2],
					  nuance_event->p_lepton[i][3], 
					  nuance_event->lepton[i], 1 ) );
    /*
    std::cout<<"Outgoing Lepton = "
	     <<  nuance_event->p_lepton[i][0] << " "
	     <<  nuance_event->p_lepton[i][1] << " "
	     <<  nuance_event->p_lepton[i][2] << " "
	     <<  nuance_event->p_lepton[i][3] << " "
	     <<  nuance_event->lepton[i] << std::endl;
    */
  }

  // for outgoing hadrons
  for (int i = 0; i < nuance_event->n_hadrons; i++){
    all_particles.push_back( FitParticle( nuance_event->p_hadron[i][0],
					  nuance_event->p_hadron[i][1],
					  nuance_event->p_hadron[i][2],
					  nuance_event->p_hadron[i][3],
					  nuance_event->hadron[i], 1 ) );
  }

  this->fNParticles = this->all_particles.size();
}

void FitEvent::SetEventAddress(NuanceEvent** tempevent){

  this->fType = kNUANCE;
  nuance_event = *tempevent;

}



