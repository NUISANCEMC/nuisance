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

#include "FitSplineHead.h"

double FitSplineHead::CalcWeight(const Double_t* incoeff){
  double weight = incoeff[0];
  double nom = weight;
  
  for (std::list<FitSpline*>::iterator iter = SplineObjects.begin();
       iter != SplineObjects.end(); iter++){
    weight *= (*iter)->DoEval(incoeff) * nom;
  }
  return weight;
};
  
void FitSplineHead::Reconfigure(std::vector<int> dial_enums, std::vector<double> dial_values){
  for (std::list<FitSpline*>::iterator iter = SplineObjects.begin();
       iter != SplineObjects.end(); iter++){
    (*iter)->ReconfigureSpline(dial_enums, dial_values);
  }
};

void FitSplineHead::SetupEventWeights(FitEventBase* event){
  
  int ncoeff = 1;
  for (std::list<FitSpline*>::iterator iter = SplineObjects.begin();
       iter != SplineObjects.end(); iter++){
    ncoeff += (*iter)->npar;
  }

  if (event->dial_coeff) delete event->dial_coeff;
  event->dial_coeff = new TArrayD(ncoeff);
  
}

void FitSplineHead::AddSpline(FitSpline* spl){
  spl->SetOffset( this->current_offset );
  this->SplineObjects.push_back(spl);
  this->current_offset += spl->npar;
}

int FitSplineHead::GetCurrentOffset(){
  int ncoeff = 1;
  for (std::list<FitSpline*>::iterator iter = SplineObjects.begin();
       iter != SplineObjects.end(); iter++){
    ncoeff += (*iter)->npar;
  }
  return ncoeff;
}

ClassImp(FitSplineHead);

