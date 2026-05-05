// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

#ifndef ICARUS_NuMI_CCQELike_numu_2026_H_SEEN
#define ICARUS_NuMI_CCQELike_numu_2026_H_SEEN

#include "Measurement1D.h"

// Implemented May 2026 by Jaesung Kim
// jae.sung.kim.3426@gmail.com
// https://arxiv.org/abs/2604.24925
class ICARUS_NuMI_CCQELike_numu_2026 : public Measurement1D {

public:

enum Distribution {
    kMuonCos=0,
    kMuonProtonCos=1,
    kdeltaPT=2,
    kdeltaalphaT=3,
    kAngular=4,
    kTKI=5,
  };

  ICARUS_NuMI_CCQELike_numu_2026( nuiskey samplekey );
  virtual ~ICARUS_NuMI_CCQELike_numu_2026() {}

  void FillEventVariables( FitEvent* event );
  bool isSignal( FitEvent* event );

  /// override the base class method
  void FillHistograms();

private:

  bool ApplyMuonPCut{false};

  bool IsCorrelatedFit{false};
  double YVarOffset{-999.};

  Distribution fDist;
  std::string fVarName;

  double weight_numu{1.}, weight_numubar{1.};

};

#endif
