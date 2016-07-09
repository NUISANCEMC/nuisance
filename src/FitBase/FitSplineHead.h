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
  void SetupEventWeights(FitEventBase* event);
  void AddSpline(FitSpline* spl);
  int GetCurrentOffset();

  int ngen_events; // Number of events in the tree these were generated with (needed for scaling)
  int current_offset;
  std::list<FitSpline*> SplineObjects;
  
  ClassDef(FitSplineHead,1);
};

ClassImp(FitSplineHead);
#endif
