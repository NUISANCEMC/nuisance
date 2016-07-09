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

  this->fType = EvtNEUT;
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

  this->fType = EvtNUWRO;
  nuwro_event = *(tempevent);

  return;
}

//*************************************************** 
int FitEvent::ND280_Mode (event * e)
//*************************************************** 
{


  Int_t proton_pdg, neutron_pdg, pion_pdg, pion_plus_pdg, pion_minus_pdg,
    lambda_pdg, eta_pdg, kaon_pdg, kaon_plus_pdg;
  proton_pdg = 2212;
  eta_pdg = 221;
  neutron_pdg = 2112;
  pion_pdg = 111;
  pion_plus_pdg = 211;
  pion_minus_pdg = -211;
  //O_16_pdg = 100069;   // oznacznie z Neuta
  lambda_pdg = 3122;
  kaon_pdg = 311;
  kaon_plus_pdg = 321;


  if (e->flag.qel)		// kwiazielastyczne oddziaływanie
    {
      if (e->flag.anty)		// jeśli jest to oddziaływanie z antyneutrinem
	{
	  if (e->flag.cc)
	    return -1;
	  else
	    {
	      if (e->nof (proton_pdg))
		return -51;
	      else if (e->nof (neutron_pdg))
		return -52;	// sprawdzam dodatkowo ?
	    }
	}
      else			// oddziaływanie z neutrinem
	{
	  if (e->flag.cc)
	    return 1;
	  else
	    {
	      if (e->nof (proton_pdg))
		return 51;
	      else if (e->nof (neutron_pdg))
		return 52;
	    }
	}
    }

  if (e->flag.mec){
    if (e->flag.anty) return -2;
    else return 2;
  }


  if (e->flag.res)		//rezonansowa produkcja: pojedynczy pion, pojed.eta, kaon, multipiony  
    {

      Int_t liczba_pionow, liczba_kaonow;

      liczba_pionow =
	e->nof (pion_pdg) + e->nof (pion_plus_pdg) + e->nof (pion_minus_pdg);
      liczba_kaonow = e->nof (kaon_pdg) + e->nof (kaon_pdg);

      if (liczba_pionow > 1 || liczba_pionow == 0)	// multipiony
	{
	  if (e->flag.anty)
	    {
	      if (e->flag.cc)
		return -21;
	      else
		return -41;
	    }
	  else
	    {
	      if (e->flag.cc)
		return 21;
	      else
		return 41;
	    }
	}

      if (liczba_pionow == 1)
	{
	  if (e->flag.anty)	// jeśli jest to oddziaływanie z antyneutrinem
	    {
	      if (e->flag.cc)
		{
		  if (e->nof (neutron_pdg) && e->nof (pion_minus_pdg))
		    return -11;
		  if (e->nof (neutron_pdg) && e->nof (pion_pdg))
		    return -12;
		  if (e->nof (proton_pdg) && e->nof (pion_minus_pdg))
		    return -13;
		}
	      else
		{
		  if (e->nof (proton_pdg))
		    {
		      if (e->nof (pion_minus_pdg))
			return -33;
		      else if (e->nof (pion_pdg))
			return -32;
		    }
		  else if (e->nof (neutron_pdg))
		    {
		      if (e->nof (pion_plus_pdg))
			return -34;
		      else if (e->nof (pion_pdg))
			return -31;
		    }
		}
	    }
	  else			// oddziaływanie z neutrinem
	    {
	      if (e->flag.cc)
		{
		  if (e->nof (proton_pdg) && e->nof (pion_plus_pdg))
		    return 11;
		  if (e->nof (proton_pdg) && e->nof (pion_pdg))
		    return 12;
		  if (e->nof (neutron_pdg) && e->nof (pion_plus_pdg))
		    return 13;
		}
	      else
		{
		  if (e->nof (proton_pdg))
		    {
		      if (e->nof (pion_minus_pdg))
			return 33;
		      else if (e->nof (pion_pdg))
			return 32;
		    }
		  else if (e->nof (neutron_pdg))
		    {
		      if (e->nof (pion_plus_pdg))
			return 34;
		      else if (e->nof (pion_pdg))
			return 31;
		    }
		}
	    }
	}

      if (e->nof (eta_pdg))	// produkcja rezonansowa ety
	{
	  if (e->flag.anty)	// jeśli jest to oddziaływanie z antyneutrinem
	    {
	      if (e->flag.cc)
		return -22;
	      else
		{
		  if (e->nof (neutron_pdg))
		    return -42;
		  else if (e->nof (proton_pdg))
		    return -43;	// sprawdzam dodatkowo ?
		}
	    }
	  else			// oddziaływanie z neutrinem
	    {
	      if (e->flag.cc)
		return 22;
	      else
		{
		  if (e->nof (neutron_pdg))
		    return 42;
		  else if (e->nof (proton_pdg))
		    return 43;
		}
	    }
	}

      if (e->nof (lambda_pdg) == 1 && liczba_kaonow == 1)	// produkcja rezonansowa kaonu
	{
	  if (e->flag.anty)	// jeśli jest to oddziaływanie z antyneutrinem
	    {
	      if (e->flag.cc && e->nof (kaon_pdg))
		return -23;
	      else
		{
		  if (e->nof (kaon_pdg))
		    return -44;
		  else if (e->nof (kaon_plus_pdg))
		    return -45;
		}
	    }
	  else			// oddziaływanie z neutrinem
	    {
	      if (e->flag.cc && e->nof (kaon_plus_pdg))
		return 23;
	      else
		{
		  if (e->nof (kaon_pdg))
		    return 44;
		  else if (e->nof (kaon_plus_pdg))
		    return 45;
		}
	    }


	}

    }

  if (e->flag.coh)		// koherentne  oddziaływanie tylko na O(16) 
    {
      Int_t _target;
      _target = e->par.nucleus_p + e->par.nucleus_n;	// liczba masowa  O(16) 

      if (_target == 16)
	{
	  if (e->flag.anty)	// jeśli jest to oddziaływanie z antyneutrinem
	    {
	      if (e->flag.cc && e->nof (pion_minus_pdg))
		return -16;
	      else if (e->nof (pion_pdg))
		return -36;
	    }
	  else			// oddziaływanie z neutrinem
	    {
	      if (e->flag.cc && e->nof (pion_plus_pdg))
		return 16;
	      else if (e->nof (pion_pdg))
		return 36;
	    }
	}
    }

  // gleboko nieelastyczne rozpraszanie               
  if (e->flag.dis)
    {
      if (e->flag.anty)
	{
	  if (e->flag.cc)
	    return -26;
	  else
	    return -46;
	}
      else
	{
	  if (e->flag.cc)
	    return 26;
	  else
	    return 46;
	}
    }

  return 9999;
}




//***************************************************       
void FitEvent::NuwroKinematics(){
//***************************************************       

  this->ResetEvent();

  // Sort Mode
  this->TotCrs = nuwro_event->weight;
  this->Mode = ND280_Mode(nuwro_event);

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
  for (int i = 0; i < nuwro_event->in.size(); i++)
    all_particles.push_back( FitParticle(&nuwro_event->in[i], 0) );
  
  // Intermediate Particles state 2
  for (int i = 0; i < nuwro_event->out.size(); i++)
    all_particles.push_back( FitParticle(&nuwro_event->out[i], 2) );

  // Outgoing Particles State 1
  for (int i = 0; i < nuwro_event->post.size(); i++)
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

  this->fType = EvtGENIE;
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
  if      ( fType == EvtNEUT  ) this->NeutKinematics();
  #endif

  #ifdef __NUWRO_ENABLED__
  if ( fType == EvtNUWRO ) this->NuwroKinematics();
  #endif

  #ifdef __GENIE_ENABLED__
  if ( fType == EvtGENIE ) this->GENIEKinematics();
  #endif

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

ClassImp(FitEvent);
