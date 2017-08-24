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

#ifndef EFFICIENCYAPPLICATOR_HXX_SEEN
#define EFFICIENCYAPPLICATOR_HXX_SEEN

#include "ISmearcepter.h"

#include "TRandom3.h"

#include <map>

class EfficiencyApplicator : public ISmearcepter {
public:
  enum DependVar { kMomentum, kKE, kTheta, kCosTheta, kPhi, kNoAxis };
private:
  struct EffMap {
    TH1D *EffCurve;
    bool Interpolate;
    // Need to work out how best to apply this.
    //bool ApplyAsWeight;
    int NDims;
    EfficiencyApplicator::DependVar DependVars[3];
    double AxisScales[3];
  };
  std::map<int, EffMap> Efficiencies;

  void SpecifcSetup(nuiskey &);

  TRandom3 rand;

 public:
  RecoInfo *Smearcept(FitEvent *);
  ~EfficiencyApplicator();
};

#endif
