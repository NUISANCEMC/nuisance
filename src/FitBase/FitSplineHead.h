#ifndef SPLINES_HEAD_H
#define SPLINES_HEAD_H

#include "TObject.h"
#include "FitEvent.h"
#include "FitParameters.h"
#include "PlotUtils.h"
#include "FitUtils.h"
#include "stdint.h"
#include "stdlib.h"
#include "FitSpline.h"
#include <list>

class FitSplineHead : public TObject {
 public:
  
  FitSplineHead(){current_offset = 1;};
  ~FitSplineHead(){};
  
  // Function Reconf (Given current dial values and names set the values for each function)
  double CalcWeight(const Double_t* incoeff);  
  void Reconfigure(std::vector<int> dial_enums, std::vector<double> dial_values);
  void SetupEventWeights(BaseFitEvt* event);
  void AddSpline(FitSpline* spl);
  int GetCurrentOffset();

  int ngen_events; // Number of events in the tree these were generated with (needed for scaling)
  int current_offset;
  std::list<FitSpline*> SplineObjects;
  
  ClassDef(FitSplineHead,1);
};

ClassImp(FitSplineHead);
#endif
