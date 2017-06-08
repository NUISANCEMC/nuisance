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
#include "BaseFitEvt.h"

BaseFitEvt::BaseFitEvt() {

  Mode = 0;
  E    = 0.0;

  Weight        = 1.0;
  InputWeight   = 1.0;
  RWWeight      = 1.0;
  CustomWeight  = 1.0;
  SavedRWWeight = 1.0;

  fSplineCoeff = NULL;
  fSplineRead = NULL;

  fGenInfo = NULL;
  fType = 9999;

#ifdef __NEUT_ENABLED__
  fNeutVect = NULL;
#endif

#ifdef __NUWRO_ENABLED__
  fNuwroEvent = NULL;
#endif

#ifdef __GENIE_ENABLED__
  genie_event  = NULL;
  genie_record = NULL;
#endif

#ifdef __NUANCE_ENABLED__
  nuance_event = NULL;
#endif

#ifdef __GiBUU_ENABLED__
  GiRead = NULL;
#endif
};

BaseFitEvt::~BaseFitEvt(){

#ifdef __NEUT_ENABLED__
  if (fNeutVect) delete fNeutVect;
#endif

#ifdef __NUWRO_ENABLED__
  if (fNuwroEvent) delete fNuwroEvent;
#endif

#ifdef __GENIE_ENABLED__
  if (genie_event) delete genie_event;
#endif

#ifdef __NUANCE_ENABLED__
  if (nuance_event) delete nuance_event;
#endif

#ifdef __GiBUU_ENABLED__
  if (GiRead) delete GiRead;
#endif

};

BaseFitEvt::BaseFitEvt(const BaseFitEvt *obj) {
 
  this->Mode = obj->Mode;
  this->E    = obj->E;

  this->Weight        = obj->Weight;
  this->InputWeight   = obj->InputWeight;
  this->RWWeight      = obj->RWWeight;
  this->CustomWeight  = obj->CustomWeight;
  this->SavedRWWeight = obj->SavedRWWeight;

  this->fSplineCoeff = obj->fSplineCoeff;
  this->fSplineRead  = obj->fSplineRead;

  this->fGenInfo = obj->fGenInfo;
  this->fType    = obj->fType;

#ifdef __NEUT_ENABLED__
  fNeutVect = obj->fNeutVect;
#endif

#ifdef __NUWRO_ENABLED__
  fNuwroEvent = obj->fNuwroEvent;
#endif

#ifdef __GENIE_ENABLED__
  genie_event = obj->genie_event;
#endif

  #ifdef __NUANCE_ENABLED__
  nuance_event = obj->nuance_event;
#endif

#ifdef __GiBUU_ENABLED__
  GiRead = obj->GiRead;
#endif

};

void BaseFitEvt::ResetWeight(){
  InputWeight = 1.0;
}

double BaseFitEvt::GetWeight(){ 
  return InputWeight * RWWeight * CustomWeight; 
};


#ifdef __NEUT_ENABLED__
void BaseFitEvt::SetNeutVect(NeutVect* v){
  fType     = kNEUT;
  fNeutVect = v;
}
#endif

#ifdef __NUWRO_ENABLED__
void BaseFitEvt::SetNuwroEvent(event* e){
  fType = kNUWRO;
  fNuwroEvent = e;
}
#endif

#ifdef __GENIE_ENABLED__
void BaseFitEvt::SetGenieEvent(NtpMCEventRecord* ntpl){
  fType = kGENIE;
  this->genie_event = ntpl;
}
#endif

#ifdef __NUANCE_ENABLED__
void BaseFitEvt::SetNuanceEvent(NuanceEvent* e){
  fType = kNUANCE;
  nuance_event = e;
}
#endif

#ifdef __GiBUU_ENABLED__
void BaseFitEvt::SetGiBUUReader(GiBUUStdHepReader* g){
  fType = kGiBUU;
  GiRead = g;
}
#endif

void BaseFitEvt::SetInputFitEvent(){
  fType = kINPUTFITEVENT;
}

void BaseFitEvt::SetInputFitSpline(){
  fType = kNEWSPLINE;
}

void BaseFitEvt::SetInputHepMC(){
  fType = kHEPMC;
}



