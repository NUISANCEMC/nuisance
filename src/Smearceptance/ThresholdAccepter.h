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

#ifndef THRESHOLDACCEPTER_HXX_SEEN
#define THRESHOLDACCEPTER_HXX_SEEN

#include "ISmearcepter.h"

#include <map>

class ThresholdAccepter : public ISmearcepter {
 public:
  enum KineVar {
    kMomentum,
    kKE,
    kCosTheta_Max,
    kCosTheta_Min,
    kAbsCosTheta_Max,
    kAbsCosTheta_Min,
    kNoVar
  };

  struct Thresh {
    KineVar ThresholdType;
    double ThresholdVal;
  };
  struct VisThresh : public Thresh {
    bool UseKE;
  };

 private:
  std::map<int, std::vector<Thresh> > ReconThresholds;
  std::map<int, VisThresh> VisThresholds;

  void SpecifcSetup(nuiskey &);

 public:
  RecoInfo *Smearcept(FitEvent *);
};

#endif
