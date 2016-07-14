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

void FitSplineHead::SetupEventWeights(BaseFitEvt* event){
  
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

