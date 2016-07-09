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

#include "FitEventBase.h"


FitEventBase::FitEventBase(){

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

FitEventBase::~FitEventBase(){
};


FitEventBase::FitEventBase(const FitEventBase* obj){

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
ClassImp(FitEventBase);

