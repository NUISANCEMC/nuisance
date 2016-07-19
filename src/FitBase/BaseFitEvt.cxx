#include "BaseFitEvt.h"


BaseFitEvt::BaseFitEvt(){

#ifdef __NEUT_ENABLED__
  neut_event  = NULL;
#endif
  
#ifdef __NUWRO_ENABLED__
  nuwro_event = NULL;
#endif
  
#ifdef __GENIE_ENABLED__
  genie_event = NULL;
#endif

  X_VAR = 0.0;
  Y_VAR = 0.0;
  Z_VAR = 0.0;
  Mode = 0;
  E = 0.0;
  Weight = 0.0;
  Signal = false;
  Index = -1;
  BinIndex = -1;
  
  dial_coeff = NULL;
};

BaseFitEvt::~BaseFitEvt(){
};


BaseFitEvt::BaseFitEvt(const BaseFitEvt* obj){

  this->X_VAR    = obj->X_VAR;
  this->Y_VAR    = obj->Y_VAR;
  this->Z_VAR    = obj->Z_VAR;
  this->Mode     = obj->Mode;
  this->E    = obj->E;
  this->Weight   = obj->Weight;
  this->Signal   = obj->Signal;
  this->Index    = obj->Index;
  this->BinIndex = obj->Index;
  
#ifdef __NEUT_ENABLED__
  neut_event  = obj->neut_event;
#endif
  
#ifdef __NUWRO_ENABLED__
  nuwro_event = obj->nuwro_event;
#endif
  
#ifdef __GENIE_ENABLED__
  genie_event = obj->genie_event;
#endif
  
  if (obj->dial_coeff){
    if (obj->dial_coeff->GetSize() > 0){
      dial_coeff = new TArrayD(*obj->dial_coeff);
    }
  }
};

ClassImp(BaseFitEvt);

