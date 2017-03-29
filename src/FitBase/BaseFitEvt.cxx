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

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>

#include "TRandom3.h"
#include "BaseFitEvt.h"


BaseFitEvt::BaseFitEvt() {

#ifdef __NEUT_ENABLED__
  fNeutVect = NULL;
#endif

#ifdef __NUWRO_ENABLED__
  fNuwroEvent = NULL;
#endif

#ifdef __GENIE_ENABLED__
  genie_event = NULL;
#endif

  Mode = 0;

  E = 0.0;
  Weight = 0.0;
  fGenInfo = NULL;
  fSplineCoeff = NULL;
  fSplineRead = NULL;
};

BaseFitEvt::~BaseFitEvt(){};

BaseFitEvt::BaseFitEvt(const BaseFitEvt *obj) {
 
  this->Mode = obj->Mode;
  this->E = obj->E;
  this->Weight = obj->Weight;
 
#ifdef __NEUT_ENABLED__
  fNeutVect = obj->fNeutVect;
#endif

#ifdef __NUWRO_ENABLED__
  fNuwroEvent = obj->fNuwroEvent;
#endif

#ifdef __GENIE_ENABLED__
  genie_event = obj->genie_event;
#endif

};

void BaseFitEvt::ResetWeight(){
  InputWeight = 1.0;
}

 double BaseFitEvt::GetWeight(){ return InputWeight * RWWeight * CustomWeight; };
